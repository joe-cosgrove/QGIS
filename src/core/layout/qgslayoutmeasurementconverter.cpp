/***************************************************************************
                         qgslayoutmeasurementconverter.cpp
                         ---------------------------------
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

#include "qgslayoutmeasurementconverter.h"


QgsLayoutMeasurementConverter::QgsLayoutMeasurementConverter()
    : mDpi( 300.0 )
{
}

QgsLayoutMeasurementConverter::~QgsLayoutMeasurementConverter()
{
}

QgsLayoutMeasurement QgsLayoutMeasurementConverter::convert( const QgsLayoutMeasurement &measurement, const QgsLayoutUnits::Units targetUnits ) const
{
  if ( measurement.units() == targetUnits )
  {
    return measurement;
  }

  switch ( targetUnits )
  {
    case QgsLayoutUnits::Millimeters:
      return QgsLayoutMeasurement( convertToMillimeters( measurement ), QgsLayoutUnits::Millimeters );
    case QgsLayoutUnits::Centimeters:
      return QgsLayoutMeasurement( convertToCentimeters( measurement ), QgsLayoutUnits::Centimeters );
    case QgsLayoutUnits::Meters:
      return QgsLayoutMeasurement( convertToMeters( measurement ), QgsLayoutUnits::Meters );
    case QgsLayoutUnits::Inches:
      return QgsLayoutMeasurement( convertToInches( measurement ), QgsLayoutUnits::Inches );
    case QgsLayoutUnits::Feet:
      return QgsLayoutMeasurement( convertToFeet( measurement ), QgsLayoutUnits::Feet );
    case QgsLayoutUnits::Points:
      return QgsLayoutMeasurement( convertToPoints( measurement ), QgsLayoutUnits::Points );
    case QgsLayoutUnits::Picas:
      return QgsLayoutMeasurement( convertToPicas( measurement ), QgsLayoutUnits::Picas );
    case QgsLayoutUnits::Pixels:
      return QgsLayoutMeasurement( convertToPixels( measurement ), QgsLayoutUnits::Pixels );
  }

  //will never be reached, but required to prevent warnings
  return QgsLayoutMeasurement( convertToMillimeters( measurement ), QgsLayoutUnits::Millimeters );
}

QgsLayoutSize QgsLayoutMeasurementConverter::convert( const QgsLayoutSize &size, const QgsLayoutUnits::Units targetUnits ) const
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
    case QgsLayoutUnits::Millimeters:
      result.setSize( convertToMillimeters( width ), convertToMillimeters( height ) );
      break;
    case QgsLayoutUnits::Centimeters:
      result.setSize( convertToCentimeters( width ), convertToCentimeters( height ) );
      break;
    case QgsLayoutUnits::Meters:
      result.setSize( convertToMeters( width ), convertToMeters( height ) );
      break;
    case QgsLayoutUnits::Inches:
      result.setSize( convertToInches( width ), convertToInches( height ) );
      break;
    case QgsLayoutUnits::Feet:
      result.setSize( convertToFeet( width ), convertToFeet( height ) );
      break;
    case QgsLayoutUnits::Points:
      result.setSize( convertToPoints( width ), convertToPoints( height ) );
      break;
    case QgsLayoutUnits::Picas:
      result.setSize( convertToPicas( width ), convertToPicas( height ) );
      break;
    case QgsLayoutUnits::Pixels:
      result.setSize( convertToPixels( width ), convertToPixels( height ) );
      break;
  }
  return result;
}

QgsLayoutPoint QgsLayoutMeasurementConverter::convert( const QgsLayoutPoint &point, const QgsLayoutUnits::Units targetUnits ) const
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
    case QgsLayoutUnits::Millimeters:
      result.setPoint( convertToMillimeters( x ), convertToMillimeters( y ) );
      break;
    case QgsLayoutUnits::Centimeters:
      result.setPoint( convertToCentimeters( x ), convertToCentimeters( y ) );
      break;
    case QgsLayoutUnits::Meters:
      result.setPoint( convertToMeters( x ), convertToMeters( y ) );
      break;
    case QgsLayoutUnits::Inches:
      result.setPoint( convertToInches( x ), convertToInches( y ) );
      break;
    case QgsLayoutUnits::Feet:
      result.setPoint( convertToFeet( x ), convertToFeet( y ) );
      break;
    case QgsLayoutUnits::Points:
      result.setPoint( convertToPoints( x ), convertToPoints( y ) );
      break;
    case QgsLayoutUnits::Picas:
      result.setPoint( convertToPicas( x ), convertToPicas( y ) );
      break;
    case QgsLayoutUnits::Pixels:
      result.setPoint( convertToPixels( x ), convertToPixels( y ) );
      break;
  }
  return result;
}

double QgsLayoutMeasurementConverter::convertToMillimeters( const QgsLayoutMeasurement &measurement ) const
{
  switch ( measurement.units() )
  {
    case QgsLayoutUnits::Millimeters:
      return measurement.length();
    case QgsLayoutUnits::Centimeters:
      return measurement.length() * 10.0;
    case QgsLayoutUnits::Meters:
      return measurement.length() * 1000.0;
    case QgsLayoutUnits::Inches:
      return measurement.length() * 25.4;
    case QgsLayoutUnits::Feet:
      return measurement.length() * 304.8;
    case QgsLayoutUnits::Points:
      return measurement.length() * 0.352777778;
    case QgsLayoutUnits::Picas:
      return measurement.length() * 4.23333333;
    case QgsLayoutUnits::Pixels:
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
