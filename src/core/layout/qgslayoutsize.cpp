/***************************************************************************
                         qgslayoutsize.cpp
                         -----------------
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

#include "qgslayoutsize.h"
#include "qgis.h"
#include <QStringList>

QgsLayoutSize::QgsLayoutSize( const double width, const double height, const QgsLayoutUnits::Units units )
    : mWidth( width )
    , mHeight( height )
    , mUnits( units )
{
}

QgsLayoutSize::QgsLayoutSize( const QgsLayoutUnits::Units units )
    : mWidth( 0.0 )
    , mHeight( 0.0 )
    , mUnits( units )
{

}

QgsLayoutSize::~QgsLayoutSize()
{
}

bool QgsLayoutSize::isEmpty() const
{
  return qgsDoubleNear( mWidth, 0 ) && qgsDoubleNear( mHeight, 0 );
}

QSizeF QgsLayoutSize::toQSizeF() const
{
  return QSizeF( mWidth, mHeight );
}

QString QgsLayoutSize::encodeSize() const
{
  return QString( "%1,%2,%3" ).arg( mWidth ).arg( mHeight ).arg( QgsLayoutUnits::encodeUnits( mUnits ) );
}

QgsLayoutSize QgsLayoutSize::decodeSize( const QString &string )
{
  QStringList parts = string.split( ',' );
  if ( parts.count() != 3 )
  {
    return QgsLayoutSize();
  }
  return QgsLayoutSize( parts[0].toDouble(), parts[1].toDouble(), QgsLayoutUnits::decodeUnits( parts[2] ) );

}

bool QgsLayoutSize::operator==( const QgsLayoutSize &other ) const
{
  return other.units() == mUnits && other.width() == mWidth && other.height() == mHeight;
}

bool QgsLayoutSize::operator!=( const QgsLayoutSize &other ) const
{
  return ( ! operator==( other ) );
}

QgsLayoutSize QgsLayoutSize::operator*( const double v ) const
{
  return QgsLayoutSize( mWidth * v, mHeight * v, mUnits );
}

QgsLayoutSize QgsLayoutSize::operator*=( const double v )
{
  *this = *this * v;
  return *this;
}

QgsLayoutSize QgsLayoutSize::operator/( const double v ) const
{
  return QgsLayoutSize( mWidth / v, mHeight / v, mUnits );
}

QgsLayoutSize QgsLayoutSize::operator/=( const double v )
{
  *this = *this / v;
  return *this;
}
