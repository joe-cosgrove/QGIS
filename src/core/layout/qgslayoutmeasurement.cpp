/***************************************************************************
                         qgslayoutmeasurement.cpp
                         --------------------
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

#include "qgslayoutmeasurement.h"

QgsLayoutMeasurement::QgsLayoutMeasurement( const double length, const QgsLayoutUnits::Units units )
    : mLength( length )
    , mUnits( units )
{
}

QgsLayoutMeasurement::~QgsLayoutMeasurement()
{
}

bool QgsLayoutMeasurement::operator==( const QgsLayoutMeasurement &other ) const
{
  return other.units() == mUnits && other.length() == mLength;
}

bool QgsLayoutMeasurement::operator!=( const QgsLayoutMeasurement &other ) const
{
  return ( ! operator==( other ) );
}

QgsLayoutMeasurement QgsLayoutMeasurement::operator+( const double v ) const
{
  return QgsLayoutMeasurement( mLength + v, mUnits );
}

QgsLayoutMeasurement QgsLayoutMeasurement::operator+=( const double v )
{
  *this = *this + v;
  return *this;
}

QgsLayoutMeasurement QgsLayoutMeasurement::operator-( const double v ) const
{
  return QgsLayoutMeasurement( mLength - v, mUnits );
}

QgsLayoutMeasurement QgsLayoutMeasurement::operator-=( const double v )
{
  *this = *this - v;
  return *this;
}

QgsLayoutMeasurement QgsLayoutMeasurement::operator*( const double v ) const
{
  return QgsLayoutMeasurement( mLength * v, mUnits );
}

QgsLayoutMeasurement QgsLayoutMeasurement::operator*=( const double v )
{
  *this = *this * v;
  return *this;
}

QgsLayoutMeasurement QgsLayoutMeasurement::operator/( const double v ) const
{
  return QgsLayoutMeasurement( mLength / v, mUnits );
}

QgsLayoutMeasurement QgsLayoutMeasurement::operator/=( const double v )
{
  *this = *this / v;
  return *this;
}
