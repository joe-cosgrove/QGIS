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

QgsMapToolCreateRectangle::QgsMapToolCreateRectangle(QgsMapCanvas* canvas)
  : QgsMapToolAddFeature( canvas, CapturePolygon )
  , mRectRubberBand( nullptr )
{
  mToolName = tr( "Add rectangle" );
}

void QgsMapToolCreateRectangle::canvasReleaseEventAddNode( QgsMapMouseEvent* e )
{
  startCapturing();

  int res;
  QgsPointV2 layerPoint;
  res = fetchLayerPoint( e->mapPointMatch(), layerPoint );
  if ( res != 0 )
  {
    res = nextPoint( QgsPointV2( e->mapPoint() ), layerPoint );
    if ( res != 0 )
    {
      return;
    }
  }

  if ( !mRectRubberBand.data() )
  {
    mRectRubberBand.reset( createRubberBand( mCaptureMode == CapturePolygon ? QgsWkbTypes::PolygonGeometry : QgsWkbTypes::LineGeometry, true ) );
  }
  else
  {
    mRectRubberBand->reset( mCaptureMode == CapturePolygon ? QgsWkbTypes::PolygonGeometry : QgsWkbTypes::LineGeometry );
  }

  if ( mPoints.isEmpty() )
  {
    // adding first corner
    mPoints << layerPoint;
    for ( int i = 0; i < 6; ++i )
      mRectRubberBand->addPoint( e->mapPoint() );
  }
  else
  {



  }

}
