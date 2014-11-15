/***************************************************************************
                         qgslayoutpoint.cpp
                         ------------------
    begin                : November 2014
    copyright            : (C) 2014 by Nyall Dawson
    email                : nyall dot dawson at gmail dot com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgslayoutpoint.h"
#include "qgis.h"

QgsLayoutPoint::QgsLayoutPoint( const double x, const double y, const QgsLayoutUnits::Units units )
    : mX( x )
    , mY( y )
    , mUnits( units )
{

}

QgsLayoutPoint::QgsLayoutPoint( const QgsLayoutUnits::Units units )
    : mX( 0.0 )
    , mY( 0.0 )
    , mUnits( units )
{

}

QgsLayoutPoint::~QgsLayoutPoint()
{

}

bool QgsLayoutPoint::isNull() const
{
  return qgsDoubleNear( mX, 0 ) && qgsDoubleNear( mY, 0 );
}

QPointF QgsLayoutPoint::toQPointF() const
{
  return QPointF( mX, mY );
}

bool QgsLayoutPoint::operator==( const QgsLayoutPoint &other ) const
{
  return other.units() == mUnits && other.x() == mX && other.y() == mY;
}

bool QgsLayoutPoint::operator!=( const QgsLayoutPoint &other ) const
{
  return ( ! operator==( other ) );
}

QgsLayoutPoint QgsLayoutPoint::operator*( const double v ) const
{
  return QgsLayoutPoint( mX * v, mY * v, mUnits );
}

QgsLayoutPoint QgsLayoutPoint::operator*=( const double v )
{
  *this = *this * v;
  return *this;
}

QgsLayoutPoint QgsLayoutPoint::operator/( const double v ) const
{
  return QgsLayoutPoint( mX / v, mY / v, mUnits );
}

QgsLayoutPoint QgsLayoutPoint::operator/=( const double v )
{
  *this = *this / v;
  return *this;
}
