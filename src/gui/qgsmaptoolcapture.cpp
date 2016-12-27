/***************************************************************************
    qgsmaptoolcapture.cpp  -  map tool for capturing points, lines, polygons
    ---------------------
    begin                : January 2006
    copyright            : (C) 2006 by Martin Dobias
    email                : wonder.sk at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsmaptoolcapture.h"

#include "qgscursors.h"
#include "qgscsexception.h"
#include "qgsfeatureiterator.h"
#include "qgsgeometryvalidator.h"
#include "qgslayertreeview.h"
#include "qgslinestring.h"
#include "qgslogger.h"
#include "qgsmapcanvas.h"
#include "qgsmapcanvastracer.h"
#include "qgsmapmouseevent.h"
#include "qgspolygon.h"
#include "qgsrubberband.h"
#include "qgsvectorlayer.h"
#include "qgsvertexmarker.h"

#include <QAction>
#include <QCursor>
#include <QPixmap>
#include <QMouseEvent>
#include <QStatusBar>


QgsMapToolCapture::QgsMapToolCapture( QgsMapCanvas* canvas, QgsAdvancedDigitizingDockWidget* cadDockWidget, CaptureMode mode )
    : QgsMapToolAdvancedDigitizing( canvas, cadDockWidget )
    , mRubberBand( nullptr )
    , mTempRubberBand( nullptr )
    , mValidator( nullptr )
    , mSnappingMarker( nullptr )
#ifdef Q_OS_WIN
    , mSkipNextContextMenuEvent( 0 )
#endif
{
  mCaptureMode = mode;

  // enable the snapping on mouse move / release
  mSnapOnMove = true;
  mSnapOnRelease = true;
  mSnapOnDoubleClick = false;
  mSnapOnPress = false;

  mCaptureModeFromLayer = mode == CaptureNone;
  mCapturing = false;

  QPixmap mySelectQPixmap = QPixmap(( const char ** ) capture_point_cursor );
  setCursor( QCursor( mySelectQPixmap, 8, 8 ) );

  connect( canvas, SIGNAL( currentLayerChanged( QgsMapLayer * ) ),
           this, SLOT( currentLayerChanged( QgsMapLayer * ) ) );
}

QgsMapToolCapture::~QgsMapToolCapture()
{
  delete mSnappingMarker;

  stopCapturing();

  if ( mValidator )
  {
    mValidator->deleteLater();
    mValidator = nullptr;
  }
}

void QgsMapToolCapture::activate()
{
  if ( mTempRubberBand )
    mTempRubberBand->show();

  QgsMapToolAdvancedDigitizing::activate();
}

void QgsMapToolCapture::deactivate()
{
  if ( mTempRubberBand )
    mTempRubberBand->hide();

  delete mSnappingMarker;
  mSnappingMarker = nullptr;

  QgsMapToolAdvancedDigitizing::deactivate();
}

void QgsMapToolCapture::validationFinished()
{
  emit messageDiscarded();
  QString msgFinished = tr( "Validation finished" );
  if ( !mValidationWarnings.isEmpty() )
  {
    emit messageEmitted( mValidationWarnings.join( QStringLiteral( "\n" ) ).append( "\n" ).append( msgFinished ), QgsMessageBar::WARNING );
  }
}

void QgsMapToolCapture::currentLayerChanged( QgsMapLayer *layer )
{
  if ( !mCaptureModeFromLayer )
    return;

  mCaptureMode = CaptureNone;

  QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( layer );
  if ( !vlayer )
  {
    return;
  }

  switch ( vlayer->geometryType() )
  {
    case QgsWkbTypes::PointGeometry:
      mCaptureMode = CapturePoint;
      break;
    case QgsWkbTypes::LineGeometry:
      mCaptureMode = CaptureLine;
      break;
    case QgsWkbTypes::PolygonGeometry:
      mCaptureMode = CapturePolygon;
      break;
    default:
      mCaptureMode = CaptureNone;
      break;
  }
}


bool QgsMapToolCapture::tracingEnabled()
{
  QgsMapCanvasTracer* tracer = QgsMapCanvasTracer::tracerForCanvas( mCanvas );
  return tracer && tracer->actionEnableTracing() && tracer->actionEnableTracing()->isChecked();
}


QgsPoint QgsMapToolCapture::tracingStartPoint()
{
  return !mCapturedNodes.isEmpty() ? mCapturedNodes.at( mCapturedNodes.size() - 1 ) : QgsPoint();
}


bool QgsMapToolCapture::tracingMouseMove( QgsMapMouseEvent* e )
{
  mTracedPoints.clear();

  if ( !e->isSnapped() )
    return false;

  QgsPoint pt0 = tracingStartPoint();
  if ( pt0 == QgsPoint() )
    return false;

  QgsMapCanvasTracer* tracer = QgsMapCanvasTracer::tracerForCanvas( mCanvas );
  if ( !tracer )
    return false;  // this should not happen!

  QgsTracer::PathError err;
  mTracedPoints = tracer->findShortestPath( pt0, e->mapPoint(), &err );

  if ( mTracedPoints.isEmpty() )
  {
    tracer->reportError( err, false );
    return false;
  }

  tracer->reportError( QgsTracer::ErrNone, false ); // clear messagebar if there was any error
  return true;
}


bool QgsMapToolCapture::tracingAddVertex( const QgsPoint& point )
{
  QgsMapCanvasTracer* tracer = QgsMapCanvasTracer::tracerForCanvas( mCanvas );
  if ( !tracer )
    return false;  // this should not happen!

  if ( mCapturedNodes.isEmpty() )
  {
    if ( !tracer->init() )
    {
      tracer->reportError( QgsTracer::ErrTooManyFeatures, true );
      return false;
    }

    // only accept first point if it is snapped to the graph (to vertex or edge)
    bool res = tracer->isPointSnapped( point );
    if ( res )
    {
      QgsPointV2 layerPoint;
      nextPoint( QgsPointV2( point ), layerPoint ); // assuming the transform went fine earlier
      mCapturedNodes << point;
      mRubberBand->addPoint( point );
      mCaptureCurve.addVertex( layerPoint );
    }
    return res;
  }

  QgsPoint pt0 = tracingStartPoint();
  if ( pt0 == QgsPoint() )
    return false;

  QgsTracer::PathError err;
  QVector<QgsPoint> points = tracer->findShortestPath( pt0, point, &err );
  if ( points.isEmpty() )
    return false; // ignore the vertex - can't find path to the end point!

  // transform points
  QgsPointSequence layerPoints;
  QgsPointV2 lp; // in layer coords
  for ( int i = 1; i < points.count(); ++i )
  {
    mCapturedNodes << points.at( i );
    if ( nextPoint( QgsPointV2( points.at( i ) ), lp ) != 0 )
      return false;
    layerPoints << lp;
  }

  for ( int i = 1; i < points.count(); ++i )
  {
    if ( points[i] == points[i-1] )
      continue; // avoid duplicate vertices if there are any
    mRubberBand->addPoint( points[i], i == points.count() - 1 );
    mCaptureCurve.addVertex( layerPoints[i-1] );
  }

  tracer->reportError( QgsTracer::ErrNone, true ); // clear messagebar if there was any error
  return true;
}


void QgsMapToolCapture::cadCanvasMoveEvent( QgsMapMouseEvent * e )
{
  QgsMapToolAdvancedDigitizing::cadCanvasMoveEvent( e );
  bool snapped = e->isSnapped();
  QgsPoint point = e->mapPoint();
  mLastMousePoint = point;

  if ( !snapped )
  {
    delete mSnappingMarker;
    mSnappingMarker = nullptr;
  }
  else
  {
    if ( !mSnappingMarker )
    {
      mSnappingMarker = new QgsVertexMarker( mCanvas );
      mSnappingMarker->setIconType( QgsVertexMarker::ICON_CROSS );
      mSnappingMarker->setColor( Qt::magenta );
      mSnappingMarker->setPenWidth( 3 );
    }
    mSnappingMarker->setCenter( point );
  }

  if ( mCaptureMode != CapturePoint && !mCapturedNodes.isEmpty() && mCapturing )
  {
    if ( tracingEnabled() )
    {
      tracingMouseMove( e );
    }

    updateTemporaryRubberBand( point );
  }
}


int QgsMapToolCapture::nextPoint( const QgsPointV2& mapPoint, QgsPointV2& layerPoint )
{
  QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( mCanvas->currentLayer() );
  if ( !vlayer )
  {
    QgsDebugMsg( "no vector layer" );
    return 1;
  }
  try
  {
    QgsPoint mapP( mapPoint.x(), mapPoint.y() );
    layerPoint = QgsPointV2( toLayerCoordinates( vlayer, mapP ) ); //transform snapped point back to layer crs
    if ( QgsWkbTypes::hasZ( vlayer->wkbType() ) )
      layerPoint.addZValue( 0.0 );
    if ( QgsWkbTypes::hasM( vlayer->wkbType() ) )
      layerPoint.addMValue( 0.0 );
  }
  catch ( QgsCsException &cse )
  {
    Q_UNUSED( cse );
    QgsDebugMsg( "transformation to layer coordinate failed" );
    return 2;
  }

  return 0;
}

int QgsMapToolCapture::nextPoint( QPoint p, QgsPointV2 &layerPoint, QgsPointV2 &mapPoint )
{
  mapPoint = QgsPointV2( toMapCoordinates( p ) );
  return nextPoint( mapPoint, layerPoint );
}

int QgsMapToolCapture::fetchLayerPoint( const QgsPointLocator::Match& match , QgsPointV2 &layerPoint )
{
  QgsVectorLayer* vlayer = qobject_cast<QgsVectorLayer *>( mCanvas->currentLayer() );
  QgsVectorLayer* sourceLayer = match.layer();
  if ( match.isValid() && match.hasVertex() && sourceLayer &&
       ( sourceLayer->crs() == vlayer->crs() ) )
  {
    QgsFeature f;
    QgsFeatureRequest request;
    request.setFilterFid( match.featureId() );
    bool fetched = match.layer()->getFeatures( request ).nextFeature( f );
    if ( fetched )
    {
      QgsVertexId vId;
      if ( !f.geometry().vertexIdFromVertexNr( match.vertexIndex(), vId ) )
        return 2;
      layerPoint = f.geometry().geometry()->vertexAt( vId );
      return 0;
    }
    else
    {
      return 2;
    }
  }
  else
  {
    return 1;
  }
}

int QgsMapToolCapture::addVertex( const QgsPoint& point )
{
  return addVertex( point, QgsPointLocator::Match() );
}

int QgsMapToolCapture::addVertex( const QgsPoint& point, const QgsPointLocator::Match& match )
{
  if ( mode() == CaptureNone )
  {
    QgsDebugMsg( "invalid capture mode" );
    return 2;
  }

  int res;
  QgsPointV2 layerPoint;
  res = fetchLayerPoint( match, layerPoint );
  if ( res != 0 )
  {
    res = nextPoint( QgsPointV2( point ), layerPoint );
    if ( res != 0 )
    {
      return res;
    }
  }

  if ( !mRubberBand )
  {
    mRubberBand = createRubberBand( mCaptureMode == CapturePolygon ? QgsWkbTypes::PolygonGeometry : QgsWkbTypes::LineGeometry );
  }

  bool traceCreated = false;
  if ( tracingEnabled() )
  {
    traceCreated = tracingAddVertex( point );
  }

  if ( !traceCreated )
  {
    // ordinary digitizing
    mRubberBand->addPoint( point );
    mCapturedNodes << point;
    mCaptureCurve.addVertex( layerPoint );
  }

  updateTemporaryRubberBand( point );

  validateGeometry();

  return 0;
}

int QgsMapToolCapture::addCurve( QgsCurve* c )
{
  if ( !c )
  {
    return 1;
  }

  if ( !mRubberBand )
  {
    mRubberBand = createRubberBand( mCaptureMode == CapturePolygon ? QgsWkbTypes::PolygonGeometry : QgsWkbTypes::LineGeometry );
  }

  QgsLineString* lineString = c->curveToLine();
  QgsPointSequence linePoints;
  lineString->points( linePoints );
  delete lineString;
  QgsPointSequence::const_iterator ptIt = linePoints.constBegin();
  for ( ; ptIt != linePoints.constEnd(); ++ptIt )
  {
    mRubberBand->addPoint( QgsPoint( ptIt->x(), ptIt->y() ) );
    mCapturedNodes << QgsPoint( ptIt->x(), ptIt->y() );
  }

  //transform back to layer CRS in case map CRS and layer CRS are different
  QgsVectorLayer* vlayer = qobject_cast<QgsVectorLayer *>( mCanvas->currentLayer() );
  QgsCoordinateTransform ct =  mCanvas->mapSettings().layerTransform( vlayer );
  if ( ct.isValid() )
  {
    c->transform( ct, QgsCoordinateTransform::ReverseTransform );
  }
  mCaptureCurve.addCurve( c );

  return 0;
}


void QgsMapToolCapture::undo()
{
  if ( mRubberBand )
  {
    int rubberBandSize = mRubberBand->numberOfVertices();
    int captureListSize = size();

    if ( rubberBandSize < 1 || captureListSize < 1 )
    {
      return;
    }

    mRubberBand->removePoint( -1 );

    QgsVertexId vertexToRemove;
    vertexToRemove.part = 0;
    vertexToRemove.ring = 0;
    vertexToRemove.vertex = size() - 1;
    if ( vertexToRemove.vertex == 1 )
    {
      // have to record first point before removing second last vertex, or it will be lost
      QgsPointV2 firstPoint;
      QgsVertexId::VertexType type;
      mCaptureCurve.pointAt( 0, firstPoint, type );
      mCaptureCurve.deleteVertex( vertexToRemove );
      mCaptureCurve.addVertex( firstPoint );
    }
    else
    {
      mCaptureCurve.deleteVertex( vertexToRemove );
    }
    mCapturedNodes.removeLast();

    updateTemporaryRubberBand( mLastMousePoint );

    validateGeometry();
  }
}

void QgsMapToolCapture::keyPressEvent( QKeyEvent* e )
{
  if ( e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Delete )
  {
    undo();

    // Override default shortcut management in MapCanvas
    e->ignore();
  }
  else if ( e->key() == Qt::Key_Escape )
  {
    stopCapturing();

    // Override default shortcut management in MapCanvas
    e->ignore();
  }
}

void QgsMapToolCapture::startCapturing()
{
  mCapturing = true;
}

bool QgsMapToolCapture::isCapturing() const
{
  return mCapturing;
}

void QgsMapToolCapture::stopCapturing()
{
  if ( mRubberBand )
  {
    delete mRubberBand;
    mRubberBand = nullptr;
  }

  if ( mTempRubberBand )
  {
    delete mTempRubberBand;
    mTempRubberBand = nullptr;
  }

  while ( !mGeomErrorMarkers.isEmpty() )
  {
    delete mGeomErrorMarkers.takeFirst();
  }

  mGeomErrors.clear();

#ifdef Q_OS_WIN
  Q_FOREACH ( QWidget *w, qApp->topLevelWidgets() )
  {
    if ( w->objectName() == "QgisApp" )
    {
      if ( mSkipNextContextMenuEvent++ == 0 )
        w->installEventFilter( this );
      break;
    }
  }
#endif

  mCapturing = false;
  mCaptureCurve.clear();
  mCapturedNodes.clear();
  if ( currentVectorLayer() )
    currentVectorLayer()->triggerRepaint();
}

void QgsMapToolCapture::deleteTempRubberBand()
{
  if ( mTempRubberBand )
  {
    delete mTempRubberBand;
    mTempRubberBand = nullptr;
  }
}

void QgsMapToolCapture::closePolygon()
{
  mCaptureCurve.close();
}

QList<QgsPoint> QgsMapToolCapture::temporaryRubberBandPoints( const QVector<QgsPoint>& clickedPoints, const QgsPoint& mousePoint , const QVector<QgsPoint>& tracedPoints ) const
{
  QList<QgsPoint> result;

  if ( mCaptureMode == CapturePolygon && !clickedPoints.isEmpty() )
    result << clickedPoints.at( 0 );

  if ( !clickedPoints.isEmpty() )
    result << clickedPoints.last();

  Q_FOREACH ( const QgsPoint& tracePoint, tracedPoints )
  {
    result << tracePoint;
  }

  result << mousePoint;

  return result;
}

void QgsMapToolCapture::updateTemporaryRubberBand( const QgsPoint& mousePoint )
{
  if ( !mTempRubberBand && !mCapturedNodes.isEmpty() )
  {
    mTempRubberBand = createRubberBand( mCaptureMode == CapturePolygon ? QgsWkbTypes::PolygonGeometry : QgsWkbTypes::LineGeometry, true );

  }
  else if ( mTempRubberBand )
  {
    mTempRubberBand->reset( mCaptureMode == CapturePolygon ? QgsWkbTypes::PolygonGeometry : QgsWkbTypes::LineGeometry );
  }
  else
  {
    return;
  }

  QList< QgsPoint > rbPoints = temporaryRubberBandPoints( mCapturedNodes, mousePoint, mTracedPoints );

  for ( int i = 0; i < rbPoints.size(); ++i )
  {
    mTempRubberBand->addPoint( rbPoints.at( i ), i == rbPoints.size() - 1 );
  }
}

void QgsMapToolCapture::validateGeometry()
{
  QSettings settings;
  if ( settings.value( QStringLiteral( "/qgis/digitizing/validate_geometries" ), 1 ).toInt() == 0 )
    return;

  if ( mValidator )
  {
    mValidator->deleteLater();
    mValidator = nullptr;
  }

  mValidationWarnings.clear();
  mGeomErrors.clear();
  while ( !mGeomErrorMarkers.isEmpty() )
  {
    delete mGeomErrorMarkers.takeFirst();
  }

  QScopedPointer<QgsGeometry> g;

  switch ( mCaptureMode )
  {
    case CaptureNone:
    case CapturePoint:
      return;
    case CaptureSegment:
    case CaptureLine:
      if ( size() < 2  || ( mCaptureMode == CaptureSegment && size() > 2 ) )
        return;
      g.reset( new QgsGeometry( mCaptureCurve.curveToLine() ) );
      break;
    case CapturePolygon:
      if ( size() < 3 )
        return;
      QgsLineString* exteriorRing = mCaptureCurve.curveToLine();
      exteriorRing->close();
      QgsPolygonV2* polygon = new QgsPolygonV2();
      polygon->setExteriorRing( exteriorRing );
      g.reset( new QgsGeometry( polygon ) );
      break;
  }

  if ( !g.data() )
    return;

  mValidator = new QgsGeometryValidator( g.data() );
  connect( mValidator, SIGNAL( errorFound( QgsGeometry::Error ) ), this, SLOT( addError( QgsGeometry::Error ) ) );
  connect( mValidator, SIGNAL( finished() ), this, SLOT( validationFinished() ) );
  mValidator->start();
  messageEmitted( tr( "Validation started" ) );
}

void QgsMapToolCapture::addError( QgsGeometry::Error e )
{
  mGeomErrors << e;
  QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( mCanvas->currentLayer() );
  if ( !vlayer )
    return;

  mValidationWarnings << e.what();

  if ( e.hasWhere() )
  {
    QgsVertexMarker *vm =  new QgsVertexMarker( mCanvas );
    vm->setCenter( mCanvas->mapSettings().layerToMapCoordinates( vlayer, e.where() ) );
    vm->setIconType( QgsVertexMarker::ICON_X );
    vm->setPenWidth( 2 );
    vm->setToolTip( e.what() );
    vm->setColor( Qt::green );
    vm->setZValue( vm->zValue() + 1 );
    mGeomErrorMarkers << vm;
  }

  emit messageDiscarded();
  emit messageEmitted( mValidationWarnings.join( QStringLiteral( "\n" ) ), QgsMessageBar::WARNING );
}

int QgsMapToolCapture::size()
{
  return mCaptureCurve.numPoints();
}

QList<QgsPoint> QgsMapToolCapture::points()
{
  QgsPointSequence pts;
  QList<QgsPoint> points;
  mCaptureCurve.points( pts );
  QgsGeometry::convertPointList( pts, points );
  return points;
}

void QgsMapToolCapture::setPoints( const QList<QgsPoint>& pointList )
{
  QgsPointSequence pts;
  QgsGeometry::convertPointList( pointList, pts );

  QgsLineString* line = new QgsLineString();
  line->setPoints( pts );

  mCaptureCurve.clear();
  mCaptureCurve.addCurve( line );
}

#ifdef Q_OS_WIN
bool QgsMapToolCapture::eventFilter( QObject *obj, QEvent *event )
{
  if ( event->type() != QEvent::ContextMenu )
    return false;

  if ( --mSkipNextContextMenuEvent == 0 )
    obj->removeEventFilter( this );

  return mSkipNextContextMenuEvent >= 0;
}
#endif
