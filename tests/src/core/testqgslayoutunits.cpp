/***************************************************************************
                         testqgslayoutunits.cpp
                         -----------------------
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

#include "qgslayoutunits.h"
#include "qgslayoutmeasurement.h"
#include "qgslayoutpoint.h"
#include "qgslayoutsize.h"
#include "qgslayoutmeasurementconverter.h"
#include "qgis.h"
#include <QObject>
#include <QtTest>

class TestQgsLayoutUnits : public QObject
{
    Q_OBJECT;

  private slots:
    void initTestCase();// will be called before the first testfunction is executed.
    void cleanupTestCase();// will be called after the last testfunction was executed.
    void init();// will be called before each testfunction is executed.
    void cleanup();// will be called after every testfunction.

    //QgsLayoutMeasurement
    void create(); //test creating new measurement
    void gettersSetters(); //test getting/setting properties
    void copyConstructor(); //test copy constructor
    void equality(); //test measurement equality
    void assignment(); //test measurement assignment
    void operators(); //test measurement operators
    void unitTypes(); //test unit types

    //QgsLayoutSize
    void createSize(); //test creating new layout size
    void sizeGettersSetters(); //test getting/setting properties
    void sizeCopyConstructor(); //test copy constructor
    void sizeEquality(); //test size equality
    void sizeAssignment(); //test size assignment
    void sizeOperators(); //test size operators
    void isEmpty(); //test isEmpty method
    void toQSizeF(); //test conversion to QSizeF

    //QgsLayoutPoint
    void createPoint(); //test creating new layout point
    void pointGettersSetters(); //test getting/setting properties
    void pointCopyConstructor(); //test copy constructor
    void pointEquality(); //test point equality
    void pointAssignment(); //test point assignment
    void pointOperators(); //test point operators
    void isNull(); //test isEmpty method
    void toQPointF(); //test conversion to QPointF

    void converterCreate(); //test creating new converter
    void converterCopy(); //test creating new converter using copy constructor
    void converterGettersSetters(); //test getting/setting converter properties
    void conversionToMM(); //test conversion to mm
    void conversionToCM(); //test conversion to cm
    void conversionToM(); //test conversion to m
    void conversionToInches(); //test conversion to inches
    void conversionToFeet(); //test conversion to feet
    void conversionToPoints(); //test conversion to points
    void conversionToPicas(); //test conversion to picas
    void conversionFromPixels(); //test conversion from pixels and handling of dpi
    void conversionToPixels(); //test conversion to pixels and handling of dpi
    void sizeConversion(); //test conversion of QgsLayoutSizes
    void pointConversion(); //test conversion of QgsLayoutPoint

  private:

};

void TestQgsLayoutUnits::initTestCase()
{

}

void TestQgsLayoutUnits::cleanupTestCase()
{

}

void TestQgsLayoutUnits::init()
{

}

void TestQgsLayoutUnits::cleanup()
{

}

void TestQgsLayoutUnits::create()
{
  QgsLayoutMeasurement defaultUnits( 5.0 );
  //default units should be millimetres
  QCOMPARE( defaultUnits.units(), QgsLayoutUnits::Millimeters );
  QCOMPARE( defaultUnits.length(), 5.0 );

  //test overriding default unit
  QgsLayoutMeasurement inches( 7.0, QgsLayoutUnits::Inches );
  QCOMPARE( inches.units(), QgsLayoutUnits::Inches );
  QCOMPARE( inches.length(), 7.0 );
}

void TestQgsLayoutUnits::gettersSetters()
{
  QgsLayoutMeasurement measurement( 5.0 );

  measurement.setLength( 9.0 );
  QCOMPARE( measurement.length(), 9.0 );
  measurement.setUnits( QgsLayoutUnits::Inches );
  QCOMPARE( measurement.units(), QgsLayoutUnits::Inches );
}

void TestQgsLayoutUnits::copyConstructor()
{
  QgsLayoutMeasurement source( 6.0, QgsLayoutUnits::Inches );
  QgsLayoutMeasurement dest( source );
  QCOMPARE( source, dest );
}

void TestQgsLayoutUnits::equality()
{
  QgsLayoutMeasurement m1( 6.0, QgsLayoutUnits::Inches );
  QgsLayoutMeasurement m2( 6.0, QgsLayoutUnits::Inches );
  QgsLayoutMeasurement m3( 7.0, QgsLayoutUnits::Inches );
  QgsLayoutMeasurement m4( 6.0, QgsLayoutUnits::Points );

  QVERIFY( m1 == m2 );
  QVERIFY( !( m1 == m3 ) );
  QVERIFY( !( m1 == m4 ) );
  QVERIFY( !( m3 == m4 ) );
  QVERIFY( !( m1 != m2 ) );
  QVERIFY( m1 != m3 );
  QVERIFY( m1 != m4 );
  QVERIFY( m3 != m4 );
}

void TestQgsLayoutUnits::assignment()
{
  QgsLayoutMeasurement m1( 6.0, QgsLayoutUnits::Inches );
  QgsLayoutMeasurement m2( 9.0, QgsLayoutUnits::Points );
  m2 = m1;
  QCOMPARE( m2, m1 );
}

void TestQgsLayoutUnits::operators()
{
  // +
  QgsLayoutMeasurement m1( 6.0, QgsLayoutUnits::Inches );
  QgsLayoutMeasurement m2 = m1 + 3.0;
  QCOMPARE( m2.units(), m1.units() );
  QCOMPARE( m2.length(), 9.0 );

  // +=
  QgsLayoutMeasurement m4( 6.0, QgsLayoutUnits::Inches );
  m4 += 2.0;
  QCOMPARE( m4.units(), QgsLayoutUnits::Inches );
  QCOMPARE( m4.length(), 8.0 );

  // -
  QgsLayoutMeasurement m3 = m1 - 3.0;
  QCOMPARE( m3.units(), m1.units() );
  QCOMPARE( m3.length(), 3.0 );

  // -=
  QgsLayoutMeasurement m5( 6.0, QgsLayoutUnits::Inches );
  m5 -= 2.0;
  QCOMPARE( m5.units(), QgsLayoutUnits::Inches );
  QCOMPARE( m5.length(), 4.0 );

  // *
  QgsLayoutMeasurement m6 = m1 * 3.0;
  QCOMPARE( m6.units(), m1.units() );
  QCOMPARE( m6.length(), 18.0 );

  // *=
  QgsLayoutMeasurement m7( 6.0, QgsLayoutUnits::Inches );
  m7 *= 2.0;
  QCOMPARE( m7.units(), QgsLayoutUnits::Inches );
  QCOMPARE( m7.length(), 12.0 );

  // /
  QgsLayoutMeasurement m8 = m1 / 3.0;
  QCOMPARE( m8.units(), m1.units() );
  QCOMPARE( m8.length(), 2.0 );

  // /=
  QgsLayoutMeasurement m9( 6.0, QgsLayoutUnits::Inches );
  m9 /= 2.0;
  QCOMPARE( m9.units(), QgsLayoutUnits::Inches );
  QCOMPARE( m9.length(), 3.0 );
}

void TestQgsLayoutUnits::unitTypes()
{
  QCOMPARE( QgsLayoutUnits::unitType( QgsLayoutUnits::Centimeters ), QgsLayoutUnits::Paper );
  QCOMPARE( QgsLayoutUnits::unitType( QgsLayoutUnits::Pixels ), QgsLayoutUnits::Screen );
}

void TestQgsLayoutUnits::createSize()
{
  QgsLayoutSize defaultUnits( 5.0, 6.0 );
  //default units should be millimetres
  QCOMPARE( defaultUnits.units(), QgsLayoutUnits::Millimeters );
  QCOMPARE( defaultUnits.width(), 5.0 );
  QCOMPARE( defaultUnits.height(), 6.0 );

  //test overriding default unit
  QgsLayoutSize inches( 7.0, 8.0, QgsLayoutUnits::Inches );
  QCOMPARE( inches.units(), QgsLayoutUnits::Inches );
  QCOMPARE( inches.width(), 7.0 );
  QCOMPARE( inches.height(), 8.0 );

  //test empty constructor
  QgsLayoutSize empty( QgsLayoutUnits::Pixels );
  QCOMPARE( empty.units(), QgsLayoutUnits::Pixels );
  QCOMPARE( empty.width(), 0.0 );
  QCOMPARE( empty.height(), 0.0 );
}

void TestQgsLayoutUnits::sizeGettersSetters()
{
  QgsLayoutSize size( 5.0, 6.0, QgsLayoutUnits::Picas );

  size.setWidth( 9.0 );
  size.setHeight( 10.0 );
  QCOMPARE( size.width(), 9.0 );
  QCOMPARE( size.height(), 10.0 );
  QCOMPARE( size.units(), QgsLayoutUnits::Picas );

  size.setUnits( QgsLayoutUnits::Inches );
  QCOMPARE( size.width(), 9.0 );
  QCOMPARE( size.height(), 10.0 );
  QCOMPARE( size.units(), QgsLayoutUnits::Inches );

  size.setSize( 11.0, 12.0 );
  QCOMPARE( size.width(), 11.0 );
  QCOMPARE( size.height(), 12.0 );
  QCOMPARE( size.units(), QgsLayoutUnits::Inches );
}

void TestQgsLayoutUnits::sizeCopyConstructor()
{
  QgsLayoutSize source( 6.0, 7.0, QgsLayoutUnits::Inches );
  QgsLayoutSize dest( source );
  QCOMPARE( source, dest );
}

void TestQgsLayoutUnits::sizeEquality()
{
  QgsLayoutSize s1( 6.0, 7.0, QgsLayoutUnits::Inches );
  QgsLayoutSize s2( 6.0, 7.0, QgsLayoutUnits::Inches );
  QgsLayoutSize s3( 7.0, 8.0, QgsLayoutUnits::Inches );
  QgsLayoutSize s4( 6.0, 7.0, QgsLayoutUnits::Points );

  QVERIFY( s1 == s2 );
  QVERIFY( !( s1 == s3 ) );
  QVERIFY( !( s1 == s4 ) );
  QVERIFY( !( s3 == s4 ) );
  QVERIFY( !( s1 != s2 ) );
  QVERIFY( s1 != s3 );
  QVERIFY( s1 != s4 );
  QVERIFY( s3 != s4 );
}

void TestQgsLayoutUnits::sizeAssignment()
{
  QgsLayoutSize s1( 6.0, 7.0, QgsLayoutUnits::Inches );
  QgsLayoutSize s2( 9.0, 10.0, QgsLayoutUnits::Points );
  s2 = s1;
  QCOMPARE( s2, s1 );
}

void TestQgsLayoutUnits::sizeOperators()
{
  QgsLayoutSize s1( 6.0, 12.0, QgsLayoutUnits::Inches );

  // *
  QgsLayoutSize s2 = s1 * 3.0;
  QCOMPARE( s2.units(), s1.units() );
  QCOMPARE( s2.width(), 18.0 );
  QCOMPARE( s2.height(), 36.0 );

  // *=
  QgsLayoutSize s3( 6.0, 12.0, QgsLayoutUnits::Inches );
  s3 *= 2.0;
  QCOMPARE( s3.units(), QgsLayoutUnits::Inches );
  QCOMPARE( s3.width(), 12.0 );
  QCOMPARE( s3.height(), 24.0 );

  // /
  QgsLayoutSize s4 = s1 / 3.0;
  QCOMPARE( s4.units(), s1.units() );
  QCOMPARE( s4.width(), 2.0 );
  QCOMPARE( s4.height(), 4.0 );

  // /=
  QgsLayoutSize s5( 6.0, 12.0, QgsLayoutUnits::Inches );
  s5 /= 2.0;
  QCOMPARE( s5.units(), QgsLayoutUnits::Inches );
  QCOMPARE( s5.width(), 3.0 );
  QCOMPARE( s5.height(), 6.0 );
}

void TestQgsLayoutUnits::isEmpty()
{
  QgsLayoutSize size( 6.0, 12.0, QgsLayoutUnits::Inches );
  QVERIFY( !size.isEmpty() );
  size.setSize( 0, 0 );
  QVERIFY( size.isEmpty() );
  QgsLayoutSize size2( QgsLayoutUnits::Millimeters ); //test empty constructor
  QVERIFY( size2.isEmpty() );
}

void TestQgsLayoutUnits::toQSizeF()
{
  QgsLayoutSize size( 6.0, 12.0, QgsLayoutUnits::Inches );
  QSizeF converted = size.toQSizeF();
  QCOMPARE( converted.width(), size.width() );
  QCOMPARE( converted.height(), size.height() );
}

void TestQgsLayoutUnits::createPoint()
{
  QgsLayoutPoint defaultUnits( 5.0, 6.0 );
  //default units should be millimetres
  QCOMPARE( defaultUnits.units(), QgsLayoutUnits::Millimeters );
  QCOMPARE( defaultUnits.x(), 5.0 );
  QCOMPARE( defaultUnits.y(), 6.0 );

  //test overriding default unit
  QgsLayoutPoint inches( 7.0, 8.0, QgsLayoutUnits::Inches );
  QCOMPARE( inches.units(), QgsLayoutUnits::Inches );
  QCOMPARE( inches.x(), 7.0 );
  QCOMPARE( inches.y(), 8.0 );

  //test empty constructor
  QgsLayoutPoint empty( QgsLayoutUnits::Pixels );
  QCOMPARE( empty.units(), QgsLayoutUnits::Pixels );
  QCOMPARE( empty.x(), 0.0 );
  QCOMPARE( empty.y(), 0.0 );
}

void TestQgsLayoutUnits::pointGettersSetters()
{
  QgsLayoutPoint point( 5.0, 6.0, QgsLayoutUnits::Picas );

  point.setX( 9.0 );
  point.setY( 10.0 );
  QCOMPARE( point.x(), 9.0 );
  QCOMPARE( point.y(), 10.0 );
  QCOMPARE( point.units(), QgsLayoutUnits::Picas );

  point.setUnits( QgsLayoutUnits::Inches );
  QCOMPARE( point.x(), 9.0 );
  QCOMPARE( point.y(), 10.0 );
  QCOMPARE( point.units(), QgsLayoutUnits::Inches );

  point.setPoint( 11.0, 12.0 );
  QCOMPARE( point.x(), 11.0 );
  QCOMPARE( point.y(), 12.0 );
  QCOMPARE( point.units(), QgsLayoutUnits::Inches );
}

void TestQgsLayoutUnits::pointCopyConstructor()
{
  QgsLayoutPoint source( 6.0, 7.0, QgsLayoutUnits::Inches );
  QgsLayoutPoint dest( source );
  QCOMPARE( source, dest );
}

void TestQgsLayoutUnits::pointEquality()
{
  QgsLayoutPoint p1( 6.0, 7.0, QgsLayoutUnits::Inches );
  QgsLayoutPoint p2( 6.0, 7.0, QgsLayoutUnits::Inches );
  QgsLayoutPoint p3( 7.0, 8.0, QgsLayoutUnits::Inches );
  QgsLayoutPoint p4( 6.0, 7.0, QgsLayoutUnits::Points );

  QVERIFY( p1 == p2 );
  QVERIFY( !( p1 == p3 ) );
  QVERIFY( !( p1 == p4 ) );
  QVERIFY( !( p3 == p4 ) );
  QVERIFY( !( p1 != p2 ) );
  QVERIFY( p1 != p3 );
  QVERIFY( p1 != p4 );
  QVERIFY( p3 != p4 );
}

void TestQgsLayoutUnits::pointAssignment()
{
  QgsLayoutPoint p1( 6.0, 7.0, QgsLayoutUnits::Inches );
  QgsLayoutPoint p2( 9.0, 10.0, QgsLayoutUnits::Points );
  p2 = p1;
  QCOMPARE( p2, p1 );
}

void TestQgsLayoutUnits::pointOperators()
{
  QgsLayoutPoint p1( 6.0, 12.0, QgsLayoutUnits::Inches );

  // *
  QgsLayoutPoint p2 = p1 * 3.0;
  QCOMPARE( p2.units(), p1.units() );
  QCOMPARE( p2.x(), 18.0 );
  QCOMPARE( p2.y(), 36.0 );

  // *=
  QgsLayoutPoint p3( 6.0, 12.0, QgsLayoutUnits::Inches );
  p3 *= 2.0;
  QCOMPARE( p3.units(), QgsLayoutUnits::Inches );
  QCOMPARE( p3.x(), 12.0 );
  QCOMPARE( p3.y(), 24.0 );

  // /
  QgsLayoutPoint p4 = p1 / 3.0;
  QCOMPARE( p4.units(), p1.units() );
  QCOMPARE( p4.x(), 2.0 );
  QCOMPARE( p4.y(), 4.0 );

  // /=
  QgsLayoutPoint p5( 6.0, 12.0, QgsLayoutUnits::Inches );
  p5 /= 2.0;
  QCOMPARE( p5.units(), QgsLayoutUnits::Inches );
  QCOMPARE( p5.x(), 3.0 );
  QCOMPARE( p5.y(), 6.0 );
}

void TestQgsLayoutUnits::isNull()
{
  QgsLayoutPoint point( 6.0, 12.0, QgsLayoutUnits::Inches );
  QVERIFY( !point.isNull() );
  point.setPoint( 0, 0 );
  QVERIFY( point.isNull() );
  QgsLayoutPoint point2( QgsLayoutUnits::Millimeters ); //test empty constructor
  QVERIFY( point2.isNull() );
}

void TestQgsLayoutUnits::toQPointF()
{
  QgsLayoutPoint point( 6.0, 12.0, QgsLayoutUnits::Inches );
  QPointF converted = point.toQPointF();
  QCOMPARE( converted.x(), point.x() );
  QCOMPARE( converted.y(), point.y() );
}

void TestQgsLayoutUnits::converterCreate()
{
  QgsLayoutMeasurementConverter* converter = new QgsLayoutMeasurementConverter();
  QVERIFY( converter );
  delete converter;
}

void TestQgsLayoutUnits::converterCopy()
{
  QgsLayoutMeasurementConverter source;
  source.setDpi( 96.0 );
  QgsLayoutMeasurementConverter copy( source );
  QCOMPARE( copy.dpi(), source.dpi() );
}

void TestQgsLayoutUnits::conversionToMM()
{
  QgsLayoutMeasurement measurementInMillimeters( 1.0, QgsLayoutUnits::Millimeters );
  QgsLayoutMeasurement measurementInCentimeters( 1.0, QgsLayoutUnits::Centimeters );
  QgsLayoutMeasurement measurementInMeters( 1.0, QgsLayoutUnits::Meters );
  QgsLayoutMeasurement measurementInInches( 1.0, QgsLayoutUnits::Inches );
  QgsLayoutMeasurement measurementInFeet( 1.0, QgsLayoutUnits::Feet );
  QgsLayoutMeasurement measurementInPoints( 1.0, QgsLayoutUnits::Points );
  QgsLayoutMeasurement measurementInPicas( 1.0, QgsLayoutUnits::Picas );

  QgsLayoutMeasurementConverter converter;
  QgsLayoutMeasurement convertedFromMillimeters = converter.convert( measurementInMillimeters, QgsLayoutUnits::Millimeters );
  QgsLayoutMeasurement convertedFromCentimeters = converter.convert( measurementInCentimeters, QgsLayoutUnits::Millimeters );
  QgsLayoutMeasurement convertedFromMeters = converter.convert( measurementInMeters, QgsLayoutUnits::Millimeters );
  QgsLayoutMeasurement convertedFromInches = converter.convert( measurementInInches, QgsLayoutUnits::Millimeters );
  QgsLayoutMeasurement convertedFromFeet = converter.convert( measurementInFeet, QgsLayoutUnits::Millimeters );
  QgsLayoutMeasurement convertedFromPoints = converter.convert( measurementInPoints, QgsLayoutUnits::Millimeters );
  QgsLayoutMeasurement convertedFromPicas = converter.convert( measurementInPicas, QgsLayoutUnits::Millimeters );

  QCOMPARE( convertedFromMillimeters.units(), QgsLayoutUnits::Millimeters );
  QCOMPARE( convertedFromCentimeters.units(), QgsLayoutUnits::Millimeters );
  QCOMPARE( convertedFromMeters.units(), QgsLayoutUnits::Millimeters );
  QCOMPARE( convertedFromInches.units(), QgsLayoutUnits::Millimeters );
  QCOMPARE( convertedFromFeet.units(), QgsLayoutUnits::Millimeters );
  QCOMPARE( convertedFromPoints.units(), QgsLayoutUnits::Millimeters );
  QCOMPARE( convertedFromPicas.units(), QgsLayoutUnits::Millimeters );

  QCOMPARE( convertedFromMillimeters.length(), 1.0 );
  QCOMPARE( convertedFromCentimeters.length(), 10.0 );
  QCOMPARE( convertedFromMeters.length(), 1000.0 );
  QCOMPARE( convertedFromInches.length(), 25.4 );
  QCOMPARE( convertedFromFeet.length(), 304.8 );
  QCOMPARE( convertedFromPoints.length(), 0.352777778 );
  QCOMPARE( convertedFromPicas.length(), 4.23333333 );
}

void TestQgsLayoutUnits::conversionToCM()
{
  QgsLayoutMeasurement measurementInMillimeters( 1.0, QgsLayoutUnits::Millimeters );
  QgsLayoutMeasurement measurementInCentimeters( 1.0, QgsLayoutUnits::Centimeters );
  QgsLayoutMeasurement measurementInMeters( 1.0, QgsLayoutUnits::Meters );
  QgsLayoutMeasurement measurementInInches( 1.0, QgsLayoutUnits::Inches );
  QgsLayoutMeasurement measurementInFeet( 1.0, QgsLayoutUnits::Feet );
  QgsLayoutMeasurement measurementInPoints( 1.0, QgsLayoutUnits::Points );
  QgsLayoutMeasurement measurementInPicas( 1.0, QgsLayoutUnits::Picas );

  QgsLayoutMeasurementConverter converter;
  QgsLayoutMeasurement convertedFromMillimeters = converter.convert( measurementInMillimeters, QgsLayoutUnits::Centimeters );
  QgsLayoutMeasurement convertedFromCentimeters = converter.convert( measurementInCentimeters, QgsLayoutUnits::Centimeters );
  QgsLayoutMeasurement convertedFromMeters = converter.convert( measurementInMeters, QgsLayoutUnits::Centimeters );
  QgsLayoutMeasurement convertedFromInches = converter.convert( measurementInInches, QgsLayoutUnits::Centimeters );
  QgsLayoutMeasurement convertedFromFeet = converter.convert( measurementInFeet, QgsLayoutUnits::Centimeters );
  QgsLayoutMeasurement convertedFromPoints = converter.convert( measurementInPoints, QgsLayoutUnits::Centimeters );
  QgsLayoutMeasurement convertedFromPicas = converter.convert( measurementInPicas, QgsLayoutUnits::Centimeters );

  QCOMPARE( convertedFromMillimeters.units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( convertedFromCentimeters.units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( convertedFromMeters.units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( convertedFromInches.units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( convertedFromFeet.units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( convertedFromPoints.units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( convertedFromPicas.units(), QgsLayoutUnits::Centimeters );

  QCOMPARE( convertedFromMillimeters.length(), 0.1 );
  QCOMPARE( convertedFromCentimeters.length(), 1.0 );
  QCOMPARE( convertedFromMeters.length(), 100.0 );
  QCOMPARE( convertedFromInches.length(), 2.54 );
  QCOMPARE( convertedFromFeet.length(), 30.48 );
  QCOMPARE( convertedFromPoints.length(), 0.0352777778 );
  QCOMPARE( convertedFromPicas.length(), 0.423333333 );
}

void TestQgsLayoutUnits::conversionToM()
{
  QgsLayoutMeasurement measurementInMillimeters( 1.0, QgsLayoutUnits::Millimeters );
  QgsLayoutMeasurement measurementInCentimeters( 1.0, QgsLayoutUnits::Centimeters );
  QgsLayoutMeasurement measurementInMeters( 1.0, QgsLayoutUnits::Meters );
  QgsLayoutMeasurement measurementInInches( 1.0, QgsLayoutUnits::Inches );
  QgsLayoutMeasurement measurementInFeet( 1.0, QgsLayoutUnits::Feet );
  QgsLayoutMeasurement measurementInPoints( 1.0, QgsLayoutUnits::Points );
  QgsLayoutMeasurement measurementInPicas( 1.0, QgsLayoutUnits::Picas );

  QgsLayoutMeasurementConverter converter;
  QgsLayoutMeasurement convertedFromMillimeters = converter.convert( measurementInMillimeters, QgsLayoutUnits::Meters );
  QgsLayoutMeasurement convertedFromCentimeters = converter.convert( measurementInCentimeters, QgsLayoutUnits::Meters );
  QgsLayoutMeasurement convertedFromMeters = converter.convert( measurementInMeters, QgsLayoutUnits::Meters );
  QgsLayoutMeasurement convertedFromInches = converter.convert( measurementInInches, QgsLayoutUnits::Meters );
  QgsLayoutMeasurement convertedFromFeet = converter.convert( measurementInFeet, QgsLayoutUnits::Meters );
  QgsLayoutMeasurement convertedFromPoints = converter.convert( measurementInPoints, QgsLayoutUnits::Meters );
  QgsLayoutMeasurement convertedFromPicas = converter.convert( measurementInPicas, QgsLayoutUnits::Meters );

  QCOMPARE( convertedFromMillimeters.units(), QgsLayoutUnits::Meters );
  QCOMPARE( convertedFromCentimeters.units(), QgsLayoutUnits::Meters );
  QCOMPARE( convertedFromMeters.units(), QgsLayoutUnits::Meters );
  QCOMPARE( convertedFromInches.units(), QgsLayoutUnits::Meters );
  QCOMPARE( convertedFromFeet.units(), QgsLayoutUnits::Meters );
  QCOMPARE( convertedFromPoints.units(), QgsLayoutUnits::Meters );
  QCOMPARE( convertedFromPicas.units(), QgsLayoutUnits::Meters );

  QCOMPARE( convertedFromMillimeters.length(), 0.001 );
  QCOMPARE( convertedFromCentimeters.length(), 0.01 );
  QCOMPARE( convertedFromMeters.length(), 1.0 );
  QCOMPARE( convertedFromInches.length(), 0.0254 );
  QCOMPARE( convertedFromFeet.length(), 0.3048 );
  QCOMPARE( convertedFromPoints.length(), 0.000352777778 );
  QCOMPARE( convertedFromPicas.length(), 0.00423333333 );
}

void TestQgsLayoutUnits::conversionToInches()
{
  QgsLayoutMeasurement measurementInMillimeters( 1.0, QgsLayoutUnits::Millimeters );
  QgsLayoutMeasurement measurementInCentimeters( 1.0, QgsLayoutUnits::Centimeters );
  QgsLayoutMeasurement measurementInMeters( 1.0, QgsLayoutUnits::Meters );
  QgsLayoutMeasurement measurementInInches( 1.0, QgsLayoutUnits::Inches );
  QgsLayoutMeasurement measurementInFeet( 1.0, QgsLayoutUnits::Feet );
  QgsLayoutMeasurement measurementInPoints( 1.0, QgsLayoutUnits::Points );
  QgsLayoutMeasurement measurementInPicas( 1.0, QgsLayoutUnits::Picas );

  QgsLayoutMeasurementConverter converter;
  QgsLayoutMeasurement convertedFromMillimeters = converter.convert( measurementInMillimeters, QgsLayoutUnits::Inches );
  QgsLayoutMeasurement convertedFromCentimeters = converter.convert( measurementInCentimeters, QgsLayoutUnits::Inches );
  QgsLayoutMeasurement convertedFromMeters = converter.convert( measurementInMeters, QgsLayoutUnits::Inches );
  QgsLayoutMeasurement convertedFromInches = converter.convert( measurementInInches, QgsLayoutUnits::Inches );
  QgsLayoutMeasurement convertedFromFeet = converter.convert( measurementInFeet, QgsLayoutUnits::Inches );
  QgsLayoutMeasurement convertedFromPoints = converter.convert( measurementInPoints, QgsLayoutUnits::Inches );
  QgsLayoutMeasurement convertedFromPicas = converter.convert( measurementInPicas, QgsLayoutUnits::Inches );

  QCOMPARE( convertedFromMillimeters.units(), QgsLayoutUnits::Inches );
  QCOMPARE( convertedFromCentimeters.units(), QgsLayoutUnits::Inches );
  QCOMPARE( convertedFromMeters.units(), QgsLayoutUnits::Inches );
  QCOMPARE( convertedFromInches.units(), QgsLayoutUnits::Inches );
  QCOMPARE( convertedFromFeet.units(), QgsLayoutUnits::Inches );
  QCOMPARE( convertedFromPoints.units(), QgsLayoutUnits::Inches );
  QCOMPARE( convertedFromPicas.units(), QgsLayoutUnits::Inches );

  QVERIFY( qgsDoubleNear( convertedFromMillimeters.length(), 0.0393701, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromCentimeters.length(), 0.3937008, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromMeters.length(), 39.3700787, 0.000001 ) );
  QCOMPARE( convertedFromInches.length(), 1.0 );
  QCOMPARE( convertedFromFeet.length(), 12.0 );
  QVERIFY( qgsDoubleNear( convertedFromPoints.length(), 0.0138888889, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromPicas.length(), 0.166666667, 0.000001 ) );
}

void TestQgsLayoutUnits::conversionToFeet()
{
  QgsLayoutMeasurement measurementInMillimeters( 1.0, QgsLayoutUnits::Millimeters );
  QgsLayoutMeasurement measurementInCentimeters( 1.0, QgsLayoutUnits::Centimeters );
  QgsLayoutMeasurement measurementInMeters( 1.0, QgsLayoutUnits::Meters );
  QgsLayoutMeasurement measurementInInches( 1.0, QgsLayoutUnits::Inches );
  QgsLayoutMeasurement measurementInFeet( 1.0, QgsLayoutUnits::Feet );
  QgsLayoutMeasurement measurementInPoints( 1.0, QgsLayoutUnits::Points );
  QgsLayoutMeasurement measurementInPicas( 1.0, QgsLayoutUnits::Picas );

  QgsLayoutMeasurementConverter converter;
  QgsLayoutMeasurement convertedFromMillimeters = converter.convert( measurementInMillimeters, QgsLayoutUnits::Feet );
  QgsLayoutMeasurement convertedFromCentimeters = converter.convert( measurementInCentimeters, QgsLayoutUnits::Feet );
  QgsLayoutMeasurement convertedFromMeters = converter.convert( measurementInMeters, QgsLayoutUnits::Feet );
  QgsLayoutMeasurement convertedFromInches = converter.convert( measurementInInches, QgsLayoutUnits::Feet );
  QgsLayoutMeasurement convertedFromFeet = converter.convert( measurementInFeet, QgsLayoutUnits::Feet );
  QgsLayoutMeasurement convertedFromPoints = converter.convert( measurementInPoints, QgsLayoutUnits::Feet );
  QgsLayoutMeasurement convertedFromPicas = converter.convert( measurementInPicas, QgsLayoutUnits::Feet );

  QCOMPARE( convertedFromMillimeters.units(), QgsLayoutUnits::Feet );
  QCOMPARE( convertedFromCentimeters.units(), QgsLayoutUnits::Feet );
  QCOMPARE( convertedFromMeters.units(), QgsLayoutUnits::Feet );
  QCOMPARE( convertedFromInches.units(), QgsLayoutUnits::Feet );
  QCOMPARE( convertedFromFeet.units(), QgsLayoutUnits::Feet );
  QCOMPARE( convertedFromPoints.units(), QgsLayoutUnits::Feet );
  QCOMPARE( convertedFromPicas.units(), QgsLayoutUnits::Feet );

  QVERIFY( qgsDoubleNear( convertedFromMillimeters.length(), 0.0032808399, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromCentimeters.length(), 0.032808399, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromMeters.length(), 3.2808399, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromInches.length(), 0.0833333, 0.000001 ) );
  QCOMPARE( convertedFromFeet.length(), 1.0 );
  QVERIFY( qgsDoubleNear( convertedFromPoints.length(), 0.00115740741, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromPicas.length(), 0.0138888889, 0.000001 ) );
}

void TestQgsLayoutUnits::conversionToPoints()
{
  QgsLayoutMeasurement measurementInMillimeters( 1.0, QgsLayoutUnits::Millimeters );
  QgsLayoutMeasurement measurementInCentimeters( 1.0, QgsLayoutUnits::Centimeters );
  QgsLayoutMeasurement measurementInMeters( 1.0, QgsLayoutUnits::Meters );
  QgsLayoutMeasurement measurementInInches( 1.0, QgsLayoutUnits::Inches );
  QgsLayoutMeasurement measurementInFeet( 1.0, QgsLayoutUnits::Feet );
  QgsLayoutMeasurement measurementInPoints( 1.0, QgsLayoutUnits::Points );
  QgsLayoutMeasurement measurementInPicas( 1.0, QgsLayoutUnits::Picas );

  QgsLayoutMeasurementConverter converter;
  QgsLayoutMeasurement convertedFromMillimeters = converter.convert( measurementInMillimeters, QgsLayoutUnits::Points );
  QgsLayoutMeasurement convertedFromCentimeters = converter.convert( measurementInCentimeters, QgsLayoutUnits::Points );
  QgsLayoutMeasurement convertedFromMeters = converter.convert( measurementInMeters, QgsLayoutUnits::Points );
  QgsLayoutMeasurement convertedFromInches = converter.convert( measurementInInches, QgsLayoutUnits::Points );
  QgsLayoutMeasurement convertedFromFeet = converter.convert( measurementInFeet, QgsLayoutUnits::Points );
  QgsLayoutMeasurement convertedFromPoints = converter.convert( measurementInPoints, QgsLayoutUnits::Points );
  QgsLayoutMeasurement convertedFromPicas = converter.convert( measurementInPicas, QgsLayoutUnits::Points );

  QCOMPARE( convertedFromMillimeters.units(), QgsLayoutUnits::Points );
  QCOMPARE( convertedFromCentimeters.units(), QgsLayoutUnits::Points );
  QCOMPARE( convertedFromMeters.units(), QgsLayoutUnits::Points );
  QCOMPARE( convertedFromInches.units(), QgsLayoutUnits::Points );
  QCOMPARE( convertedFromFeet.units(), QgsLayoutUnits::Points );
  QCOMPARE( convertedFromPoints.units(), QgsLayoutUnits::Points );
  QCOMPARE( convertedFromPicas.units(), QgsLayoutUnits::Points );

  QVERIFY( qgsDoubleNear( convertedFromMillimeters.length(), 2.83464567, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromCentimeters.length(), 28.3464567, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromMeters.length(), 2834.64567, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromInches.length(), 72.0, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromFeet.length(), 864.0, 0.000001 ) );
  QCOMPARE( convertedFromPoints.length(), 1.0 );
  QVERIFY( qgsDoubleNear( convertedFromPicas.length(), 12.0, 0.000001 ) );
}

void TestQgsLayoutUnits::conversionToPicas()
{
  QgsLayoutMeasurement measurementInMillimeters( 1.0, QgsLayoutUnits::Millimeters );
  QgsLayoutMeasurement measurementInCentimeters( 1.0, QgsLayoutUnits::Centimeters );
  QgsLayoutMeasurement measurementInMeters( 1.0, QgsLayoutUnits::Meters );
  QgsLayoutMeasurement measurementInInches( 1.0, QgsLayoutUnits::Inches );
  QgsLayoutMeasurement measurementInFeet( 1.0, QgsLayoutUnits::Feet );
  QgsLayoutMeasurement measurementInPoints( 1.0, QgsLayoutUnits::Points );
  QgsLayoutMeasurement measurementInPicas( 1.0, QgsLayoutUnits::Picas );

  QgsLayoutMeasurementConverter converter;
  QgsLayoutMeasurement convertedFromMillimeters = converter.convert( measurementInMillimeters, QgsLayoutUnits::Picas );
  QgsLayoutMeasurement convertedFromCentimeters = converter.convert( measurementInCentimeters, QgsLayoutUnits::Picas );
  QgsLayoutMeasurement convertedFromMeters = converter.convert( measurementInMeters, QgsLayoutUnits::Picas );
  QgsLayoutMeasurement convertedFromInches = converter.convert( measurementInInches, QgsLayoutUnits::Picas );
  QgsLayoutMeasurement convertedFromFeet = converter.convert( measurementInFeet, QgsLayoutUnits::Picas );
  QgsLayoutMeasurement convertedFromPoints = converter.convert( measurementInPoints, QgsLayoutUnits::Picas );
  QgsLayoutMeasurement convertedFromPicas = converter.convert( measurementInPicas, QgsLayoutUnits::Picas );

  QCOMPARE( convertedFromMillimeters.units(), QgsLayoutUnits::Picas );
  QCOMPARE( convertedFromCentimeters.units(), QgsLayoutUnits::Picas );
  QCOMPARE( convertedFromMeters.units(), QgsLayoutUnits::Picas );
  QCOMPARE( convertedFromInches.units(), QgsLayoutUnits::Picas );
  QCOMPARE( convertedFromFeet.units(), QgsLayoutUnits::Picas );
  QCOMPARE( convertedFromPoints.units(), QgsLayoutUnits::Picas );
  QCOMPARE( convertedFromPicas.units(), QgsLayoutUnits::Picas );

  QVERIFY( qgsDoubleNear( convertedFromMillimeters.length(), 0.236220472, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromCentimeters.length(), 2.36220472, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromMeters.length(), 236.220472, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromInches.length(), 6.0, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromFeet.length(), 72.0, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromPoints.length(), 0.0833333333, 0.000001 ) );
  QCOMPARE( convertedFromPicas.length(), 1.0 );
}

void TestQgsLayoutUnits::converterGettersSetters()
{
  QgsLayoutMeasurementConverter converter;
  converter.setDpi( 96.0 );
  QCOMPARE( converter.dpi(), 96.0 );
}

void TestQgsLayoutUnits::conversionFromPixels()
{
  QgsLayoutMeasurement measurementInPixels( 300.0, QgsLayoutUnits::Pixels );

  QgsLayoutMeasurementConverter converter;
  //try initially with 300 dpi
  converter.setDpi( 300.0 );
  QgsLayoutMeasurement convertedToInches = converter.convert( measurementInPixels, QgsLayoutUnits::Inches );
  QCOMPARE( convertedToInches.units(), QgsLayoutUnits::Inches );
  QVERIFY( qgsDoubleNear( convertedToInches.length(), 1.0, 0.000001 ) );
  QgsLayoutMeasurement convertedToMM = converter.convert( measurementInPixels, QgsLayoutUnits::Millimeters );
  QCOMPARE( convertedToMM.units(), QgsLayoutUnits::Millimeters );
  QVERIFY( qgsDoubleNear( convertedToMM.length(), 25.4, 0.000001 ) );

  //try with 96 dpi
  converter.setDpi( 96.0 );
  convertedToInches = converter.convert( measurementInPixels, QgsLayoutUnits::Inches );
  QCOMPARE( convertedToInches.units(), QgsLayoutUnits::Inches );
  QVERIFY( qgsDoubleNear( convertedToInches.length(), 3.125, 0.000001 ) );
  convertedToMM = converter.convert( measurementInPixels, QgsLayoutUnits::Millimeters );
  QCOMPARE( convertedToMM.units(), QgsLayoutUnits::Millimeters );
  QVERIFY( qgsDoubleNear( convertedToMM.length(), 79.375, 0.000001 ) );
}

void TestQgsLayoutUnits::conversionToPixels()
{
  QgsLayoutMeasurement measurementInInches( 1.0, QgsLayoutUnits::Inches );
  QgsLayoutMeasurement measurementInMM( 1.0, QgsLayoutUnits::Millimeters );

  QgsLayoutMeasurementConverter converter;
  //try initially with 300 dpi
  converter.setDpi( 300.0 );
  QgsLayoutMeasurement convertedToPixels = converter.convert( measurementInInches, QgsLayoutUnits::Pixels );
  QCOMPARE( convertedToPixels.units(), QgsLayoutUnits::Pixels );
  QVERIFY( qgsDoubleNear( convertedToPixels.length(), 300.0, 0.000001 ) );
  convertedToPixels = converter.convert( measurementInMM, QgsLayoutUnits::Pixels );
  QCOMPARE( convertedToPixels.units(), QgsLayoutUnits::Pixels );
  QVERIFY( qgsDoubleNear( convertedToPixels.length(), 11.811023622, 0.000001 ) );

  //try with 96 dpi
  converter.setDpi( 96.0 );
  convertedToPixels = converter.convert( measurementInInches, QgsLayoutUnits::Pixels );
  QCOMPARE( convertedToPixels.units(), QgsLayoutUnits::Pixels );
  QVERIFY( qgsDoubleNear( convertedToPixels.length(), 96.0, 0.000001 ) );
  convertedToPixels = converter.convert( measurementInMM, QgsLayoutUnits::Pixels );
  QCOMPARE( convertedToPixels.units(), QgsLayoutUnits::Pixels );
  QVERIFY( qgsDoubleNear( convertedToPixels.length(), 3.77952755906, 0.000001 ) );
}

void TestQgsLayoutUnits::sizeConversion()
{
  QgsLayoutSize sizeInMM( 1.0, 2.0, QgsLayoutUnits::Millimeters );
  QgsLayoutMeasurementConverter converter;

  //test conversion to same units
  QgsLayoutSize convertedSize = converter.convert( sizeInMM, QgsLayoutUnits::Millimeters );
  QCOMPARE( convertedSize, sizeInMM );

  //convert to other units
  convertedSize = converter.convert( sizeInMM, QgsLayoutUnits::Centimeters );
  QgsLayoutSize expectedSize( 0.1, 0.2, QgsLayoutUnits::Centimeters );
  QCOMPARE( convertedSize, expectedSize );

  //pixel conversion
  converter.setDpi( 96.0 );
  QgsLayoutSize convertedToInches = converter.convert( QgsLayoutSize( 96, 192, QgsLayoutUnits::Pixels ), QgsLayoutUnits::Inches );
  QCOMPARE( convertedToInches.width(), 1.0 );
  QCOMPARE( convertedToInches.height(), 2.0 );
  QCOMPARE( convertedToInches.units(), QgsLayoutUnits::Inches );
}

void TestQgsLayoutUnits::pointConversion()
{
  QgsLayoutPoint pointInMM( 1.0, 2.0, QgsLayoutUnits::Millimeters );
  QgsLayoutMeasurementConverter converter;

  //test conversion to same units
  QgsLayoutPoint convertedPoint = converter.convert( pointInMM, QgsLayoutUnits::Millimeters );
  QCOMPARE( convertedPoint, pointInMM );

  //convert to other units
  convertedPoint = converter.convert( pointInMM, QgsLayoutUnits::Centimeters );
  QgsLayoutPoint expectedPoint( 0.1, 0.2, QgsLayoutUnits::Centimeters );
  QCOMPARE( convertedPoint, expectedPoint );

  //pixel conversion
  converter.setDpi( 96.0 );
  QgsLayoutPoint convertedToInches = converter.convert( QgsLayoutPoint( 96, 192, QgsLayoutUnits::Pixels ), QgsLayoutUnits::Inches );
  QCOMPARE( convertedToInches.x(), 1.0 );
  QCOMPARE( convertedToInches.y(), 2.0 );
  QCOMPARE( convertedToInches.units(), QgsLayoutUnits::Inches );
}

QTEST_MAIN( TestQgsLayoutUnits )
#include "testqgslayoutunits.moc"
