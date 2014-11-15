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
#include "qgis.h"

//
// QgsLayoutMeasurement
//

QgsLayoutMeasurement::QgsLayoutMeasurement( const double length, const QgsLayoutMeasurement::Units units )
    : mLength( length )
    , mUnits( units )
{
}

QgsLayoutMeasurement::~QgsLayoutMeasurement()
{
}

QgsLayoutMeasurement::UnitType QgsLayoutMeasurement::unitType( const QgsLayoutMeasurement::Units units )
{
  switch ( units )
  {
    case Pixels:
      return QgsLayoutMeasurement::Screen;
    case  Millimeters:
    case Centimeters:
    case Meters:
    case Inches:
    case Feet:
    case Points:
    case Picas:
    default:
      return QgsLayoutMeasurement::Paper;
  }
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


//
// QgsLayoutSize
//

QgsLayoutSize::QgsLayoutSize( const double width, const double height, const QgsLayoutMeasurement::Units units )
    : mWidth( width )
    , mHeight( height )
    , mUnits( units )
{
}

QgsLayoutSize::QgsLayoutSize( const QgsLayoutMeasurement::Units units )
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


//
// QgsLayoutPoint
//

QgsLayoutPoint::QgsLayoutPoint( const double x, const double y, const QgsLayoutMeasurement::Units units )
    : mX( x )
    , mY( y )
    , mUnits( units )
{

}

QgsLayoutPoint::QgsLayoutPoint( const QgsLayoutMeasurement::Units units )
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


//
// QgsLayoutMeasurementConvertor
//

QgsLayoutMeasurementConverter::QgsLayoutMeasurementConverter()
    : mDpi( 300.0 )
{
}

QgsLayoutMeasurementConverter::~QgsLayoutMeasurementConverter()
{
}

QgsLayoutMeasurement QgsLayoutMeasurementConverter::convert( const QgsLayoutMeasurement &measurement, const QgsLayoutMeasurement::Units targetUnits ) const
{
  if ( measurement.units() == targetUnits )
  {
    return measurement;
  }

  switch ( targetUnits )
  {
    case QgsLayoutMeasurement::Millimeters:
      return QgsLayoutMeasurement( convertToMillimeters( measurement ), QgsLayoutMeasurement::Millimeters );
    case QgsLayoutMeasurement::Centimeters:
      return QgsLayoutMeasurement( convertToCentimeters( measurement ), QgsLayoutMeasurement::Centimeters );
    case QgsLayoutMeasurement::Meters:
      return QgsLayoutMeasurement( convertToMeters( measurement ), QgsLayoutMeasurement::Meters );
    case QgsLayoutMeasurement::Inches:
      return QgsLayoutMeasurement( convertToInches( measurement ), QgsLayoutMeasurement::Inches );
    case QgsLayoutMeasurement::Feet:
      return QgsLayoutMeasurement( convertToFeet( measurement ), QgsLayoutMeasurement::Feet );
    case QgsLayoutMeasurement::Points:
      return QgsLayoutMeasurement( convertToPoints( measurement ), QgsLayoutMeasurement::Points );
    case QgsLayoutMeasurement::Picas:
      return QgsLayoutMeasurement( convertToPicas( measurement ), QgsLayoutMeasurement::Picas );
    case QgsLayoutMeasurement::Pixels:
      return QgsLayoutMeasurement( convertToPixels( measurement ), QgsLayoutMeasurement::Pixels );
  }

  //will never be reached, but required to prevent warnings
  return QgsLayoutMeasurement( convertToMillimeters( measurement ), QgsLayoutMeasurement::Millimeters );
}

QgsLayoutSize QgsLayoutMeasurementConverter::convert( const QgsLayoutSize &size, const QgsLayoutMeasurement::Units targetUnits ) const
{
  if ( size.units() == targetUnits )
  {
    return size;
  }

  QgsLayoutSize result( size );
  result.setUnits( targetUnits );
  QgsLayoutMeasurement width = QgsLayoutMeasurement( size.width(), size.units() );
  QgsLayoutMeasurement height = QgsLayoutMeasurement( size.height(), size.units() );
  switch ( targetUnits )
  {
    case QgsLayoutMeasurement::Millimeters:
      result.setSize( convertToMillimeters( width ), convertToMillimeters( height ) );
      break;
    case QgsLayoutMeasurement::Centimeters:
      result.setSize( convertToCentimeters( width ), convertToCentimeters( height ) );
      break;
    case QgsLayoutMeasurement::Meters:
      result.setSize( convertToMeters( width ), convertToMeters( height ) );
      break;
    case QgsLayoutMeasurement::Inches:
      result.setSize( convertToInches( width ), convertToInches( height ) );
      break;
    case QgsLayoutMeasurement::Feet:
      result.setSize( convertToFeet( width ), convertToFeet( height ) );
      break;
    case QgsLayoutMeasurement::Points:
      result.setSize( convertToPoints( width ), convertToPoints( height ) );
      break;
    case QgsLayoutMeasurement::Picas:
      result.setSize( convertToPicas( width ), convertToPicas( height ) );
      break;
    case QgsLayoutMeasurement::Pixels:
      result.setSize( convertToPixels( width ), convertToPixels( height ) );
      break;
  }
  return result;
}

QgsLayoutPoint QgsLayoutMeasurementConverter::convert( const QgsLayoutPoint &point, const QgsLayoutMeasurement::Units targetUnits ) const
{
  if ( point.units() == targetUnits )
  {
    return point;
  }

  QgsLayoutPoint result( point );
  result.setUnits( targetUnits );
  QgsLayoutMeasurement x = QgsLayoutMeasurement( point.x(), point.units() );
  QgsLayoutMeasurement y = QgsLayoutMeasurement( point.y(), point.units() );
  switch ( targetUnits )
  {
    case QgsLayoutMeasurement::Millimeters:
      result.setPoint( convertToMillimeters( x ), convertToMillimeters( y ) );
      break;
    case QgsLayoutMeasurement::Centimeters:
      result.setPoint( convertToCentimeters( x ), convertToCentimeters( y ) );
      break;
    case QgsLayoutMeasurement::Meters:
      result.setPoint( convertToMeters( x ), convertToMeters( y ) );
      break;
    case QgsLayoutMeasurement::Inches:
      result.setPoint( convertToInches( x ), convertToInches( y ) );
      break;
    case QgsLayoutMeasurement::Feet:
      result.setPoint( convertToFeet( x ), convertToFeet( y ) );
      break;
    case QgsLayoutMeasurement::Points:
      result.setPoint( convertToPoints( x ), convertToPoints( y ) );
      break;
    case QgsLayoutMeasurement::Picas:
      result.setPoint( convertToPicas( x ), convertToPicas( y ) );
      break;
    case QgsLayoutMeasurement::Pixels:
      result.setPoint( convertToPixels( x ), convertToPixels( y ) );
      break;
  }
  return result;
}

double QgsLayoutMeasurementConverter::convertToMillimeters( const QgsLayoutMeasurement &measurement ) const
{
  switch ( measurement.units() )
  {
    case QgsLayoutMeasurement::Millimeters:
      return measurement.length();
    case QgsLayoutMeasurement::Centimeters:
      return measurement.length() * 10.0;
    case QgsLayoutMeasurement::Meters:
      return measurement.length() * 1000.0;
    case QgsLayoutMeasurement::Inches:
      return measurement.length() * 25.4;
    case QgsLayoutMeasurement::Feet:
      return measurement.length() * 304.8;
    case QgsLayoutMeasurement::Points:
      return measurement.length() * 0.352777778;
    case QgsLayoutMeasurement::Picas:
      return measurement.length() * 4.23333333;
    case QgsLayoutMeasurement::Pixels:
      return measurement.length() * 25.4 / mDpi;
  }

  //will never be reached, but required to prevent warnings
  return measurement.length();
}

double QgsLayoutMeasurementConverter::convertToCentimeters( const QgsLayoutMeasurement &measurement ) const
{
  return convertToMillimeters( measurement ) / 10.0;
}

double QgsLayoutMeasurementConverter::convertToMeters( const QgsLayoutMeasurement &measurement ) const
{
  return convertToMillimeters( measurement ) / 1000.0;
}

double QgsLayoutMeasurementConverter::convertToInches( const QgsLayoutMeasurement &measurement ) const
{
  return convertToMillimeters( measurement ) / 25.4;
}

double QgsLayoutMeasurementConverter::convertToFeet( const QgsLayoutMeasurement &measurement ) const
{
  return convertToMillimeters( measurement ) / 304.8;
}

double QgsLayoutMeasurementConverter::convertToPoints( const QgsLayoutMeasurement &measurement ) const
{
  return convertToMillimeters( measurement ) * 2.83464567;
}

double QgsLayoutMeasurementConverter::convertToPicas( const QgsLayoutMeasurement &measurement ) const
{
  return convertToMillimeters( measurement ) * 0.236220472;
}

double QgsLayoutMeasurementConverter::convertToPixels( const QgsLayoutMeasurement &measurement ) const
{
  return convertToMillimeters( measurement ) * mDpi / 25.4;
}
