/***************************************************************************
    qgsmaptoolcreaterectangle.cpp
    -----------------------------
    begin                : Dec 2016
    copyright            : (C) 2016 by Nyall Dawson
    email                : nyall dot dawson at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsmaptoolcreaterectangle.h"
#include "qgsvectordataprovider.h"

QgsMapToolCreateRectangle::QgsMapToolCreateRectangle( QgsMapCanvas* canvas )
    : QgsMapToolAddFeature( canvas, CapturePolygon )
    , mRectRubberBand( nullptr )
{
  mToolName = tr( "Add rectangle" );
}

QgsGeometry QgsMapToolCreateRectangle::captureCurve() const
{
  QgsGeometry g = QgsMapToolAddFeature::captureCurve();
  QgsRectangle r( g.vertexAt( 0 ), g.vertexAt( 1 ) );
  return QgsGeometry::fromRect( r );
}

QList<QgsPoint> QgsMapToolCreateRectangle::temporaryRubberBandPoints( const QVector<QgsPoint>& clickedPoints, const QgsPoint& mousePoint, const QVector<QgsPoint>& , QgsMapMouseEvent* e ) const
{
  QList<QgsPoint> result;

  if ( clickedPoints.isEmpty() )
    return result;

  QgsPoint newCorner = mousePoint;

  // shift modifer = square
  if ( e && e->modifiers() & Qt::ShiftModifier )
  {
    double dx = newCorner.x() - clickedPoints.at( 0 ).x();
    double dy = newCorner.y() - clickedPoints.at( 0 ).y();

    if ( qAbs( dx ) > qAbs( dy ) )
      newCorner.setY( clickedPoints.at( 0 ).y() + ( dy > 0 ? 1 : -1 ) * qAbs( dx ) );
    \
    else
      newCorner.setX( clickedPoints.at( 0 ).x() + ( dx > 0 ? 1 : -1 ) * qAbs( dy ) );
    \
  }

  // create rectangle using first clicked point and mouse point as opposite corners
  QgsRectangle r( clickedPoints.at( 0 ), newCorner );

  result << QgsPoint( r.xMinimum(), r.yMinimum() );
  result << QgsPoint( r.xMinimum(), r.yMaximum() );
  result << QgsPoint( r.xMaximum(), r.yMaximum() );
  result << QgsPoint( r.xMaximum(), r.yMinimum() );
  return result;
}
