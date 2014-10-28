/***************************************************************************
                         testqgslayoutmeasurement.cpp
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

#include "qgslayoutmeasurement.h"
#include "qgis.h"
#include <QObject>
#include <QtTest>

class TestQgsLayoutMeasurement : public QObject
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

    //QgsLayoutSize
    void createSize(); //test creating new layout size
    void sizeGettersSetters(); //test getting/setting properties
    void sizeCopyConstructor(); //test copy constructor
    void sizeEquality(); //test size equality
    void sizeAssignment(); //test size assignment
    void sizeOperators(); //test size operators
    void toQSizeF(); //test conversion to QSizeF

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

  private:

};

void TestQgsLayoutMeasurement::initTestCase()
{

}

void TestQgsLayoutMeasurement::cleanupTestCase()
{

}

void TestQgsLayoutMeasurement::init()
{

}

void TestQgsLayoutMeasurement::cleanup()
{

}

void TestQgsLayoutMeasurement::create()
{
  QgsLayoutMeasurement defaultUnits( 5.0 );
  //default units should be millimetres
  QCOMPARE( defaultUnits.units(), QgsLayoutMeasurement::Millimeters );
  QCOMPARE( defaultUnits.length(), 5.0 );

  //test overriding default unit
  QgsLayoutMeasurement inches( 7.0, QgsLayoutMeasurement::Inches );
  QCOMPARE( inches.units(), QgsLayoutMeasurement::Inches );
  QCOMPARE( inches.length(), 7.0 );
}

void TestQgsLayoutMeasurement::gettersSetters()
{
  QgsLayoutMeasurement measurement( 5.0 );

  measurement.setLength( 9.0 );
  QCOMPARE( measurement.length(), 9.0 );
  measurement.setUnits( QgsLayoutMeasurement::Inches );
  QCOMPARE( measurement.units(), QgsLayoutMeasurement::Inches );
}

void TestQgsLayoutMeasurement::copyConstructor()
{
  QgsLayoutMeasurement source( 6.0, QgsLayoutMeasurement::Inches );
  QgsLayoutMeasurement dest( source );
  QCOMPARE( source, dest );
}

void TestQgsLayoutMeasurement::equality()
{
  QgsLayoutMeasurement m1( 6.0, QgsLayoutMeasurement::Inches );
  QgsLayoutMeasurement m2( 6.0, QgsLayoutMeasurement::Inches );
  QgsLayoutMeasurement m3( 7.0, QgsLayoutMeasurement::Inches );
  QgsLayoutMeasurement m4( 6.0, QgsLayoutMeasurement::Points );

  QVERIFY( m1 == m2 );
  QVERIFY( !( m1 == m3 ) );
  QVERIFY( !( m1 == m4 ) );
  QVERIFY( !( m3 == m4 ) );
  QVERIFY( !( m1 != m2 ) );
  QVERIFY( m1 != m3 );
  QVERIFY( m1 != m4 );
  QVERIFY( m3 != m4 );
}

void TestQgsLayoutMeasurement::assignment()
{
  QgsLayoutMeasurement m1( 6.0, QgsLayoutMeasurement::Inches );
  QgsLayoutMeasurement m2( 9.0, QgsLayoutMeasurement::Points );
  m2 = m1;
  QCOMPARE( m2, m1 );
}

void TestQgsLayoutMeasurement::operators()
{
  // +
  QgsLayoutMeasurement m1( 6.0, QgsLayoutMeasurement::Inches );
  QgsLayoutMeasurement m2 = m1 + 3.0;
  QCOMPARE( m2.units(), m1.units() );
  QCOMPARE( m2.length(), 9.0 );

  // +=
  QgsLayoutMeasurement m4( 6.0, QgsLayoutMeasurement::Inches );
  m4 += 2.0;
  QCOMPARE( m4.units(), QgsLayoutMeasurement::Inches );
  QCOMPARE( m4.length(), 8.0 );

  // -
  QgsLayoutMeasurement m3 = m1 - 3.0;
  QCOMPARE( m3.units(), m1.units() );
  QCOMPARE( m3.length(), 3.0 );

  // -=
  QgsLayoutMeasurement m5( 6.0, QgsLayoutMeasurement::Inches );
  m5 -= 2.0;
  QCOMPARE( m5.units(), QgsLayoutMeasurement::Inches );
  QCOMPARE( m5.length(), 4.0 );

  // *
  QgsLayoutMeasurement m6 = m1 * 3.0;
  QCOMPARE( m6.units(), m1.units() );
  QCOMPARE( m6.length(), 18.0 );

  // *=
  QgsLayoutMeasurement m7( 6.0, QgsLayoutMeasurement::Inches );
  m7 *= 2.0;
  QCOMPARE( m7.units(), QgsLayoutMeasurement::Inches );
  QCOMPARE( m7.length(), 12.0 );

  // /
  QgsLayoutMeasurement m8 = m1 / 3.0;
  QCOMPARE( m8.units(), m1.units() );
  QCOMPARE( m8.length(), 2.0 );

  // /=
  QgsLayoutMeasurement m9( 6.0, QgsLayoutMeasurement::Inches );
  m9 /= 2.0;
  QCOMPARE( m9.units(), QgsLayoutMeasurement::Inches );
  QCOMPARE( m9.length(), 3.0 );
}

void TestQgsLayoutMeasurement::createSize()
{
  QgsLayoutSize defaultUnits( 5.0, 6.0 );
  //default units should be millimetres
  QCOMPARE( defaultUnits.units(), QgsLayoutMeasurement::Millimeters );
  QCOMPARE( defaultUnits.width(), 5.0 );
  QCOMPARE( defaultUnits.height(), 6.0 );

  //test overriding default unit
  QgsLayoutSize inches( 7.0, 8.0, QgsLayoutMeasurement::Inches );
  QCOMPARE( inches.units(), QgsLayoutMeasurement::Inches );
  QCOMPARE( inches.width(), 7.0 );
  QCOMPARE( inches.height(), 8.0 );
}

void TestQgsLayoutMeasurement::sizeGettersSetters()
{
  QgsLayoutSize size( 5.0, 6.0, QgsLayoutMeasurement::Picas );

  size.setWidth( 9.0 );
  size.setHeight( 10.0 );
  QCOMPARE( size.width(), 9.0 );
  QCOMPARE( size.height(), 10.0 );
  QCOMPARE( size.units(), QgsLayoutMeasurement::Picas );

  size.setUnits( QgsLayoutMeasurement::Inches );
  QCOMPARE( size.width(), 9.0 );
  QCOMPARE( size.height(), 10.0 );
  QCOMPARE( size.units(), QgsLayoutMeasurement::Inches );

  size.setSize( 11.0, 12.0 );
  QCOMPARE( size.width(), 11.0 );
  QCOMPARE( size.height(), 12.0 );
  QCOMPARE( size.units(), QgsLayoutMeasurement::Inches );
}

void TestQgsLayoutMeasurement::sizeCopyConstructor()
{
  QgsLayoutSize source( 6.0, 7.0, QgsLayoutMeasurement::Inches );
  QgsLayoutSize dest( source );
  QCOMPARE( source, dest );
}

void TestQgsLayoutMeasurement::sizeEquality()
{
  QgsLayoutSize s1( 6.0, 7.0, QgsLayoutMeasurement::Inches );
  QgsLayoutSize s2( 6.0, 7.0, QgsLayoutMeasurement::Inches );
  QgsLayoutSize s3( 7.0, 8.0, QgsLayoutMeasurement::Inches );
  QgsLayoutSize s4( 6.0, 7.0, QgsLayoutMeasurement::Points );

  QVERIFY( s1 == s2 );
  QVERIFY( !( s1 == s3 ) );
  QVERIFY( !( s1 == s4 ) );
  QVERIFY( !( s3 == s4 ) );
  QVERIFY( !( s1 != s2 ) );
  QVERIFY( s1 != s3 );
  QVERIFY( s1 != s4 );
  QVERIFY( s3 != s4 );
}

void TestQgsLayoutMeasurement::sizeAssignment()
{
  QgsLayoutSize s1( 6.0, 7.0, QgsLayoutMeasurement::Inches );
  QgsLayoutSize s2( 9.0, 10.0, QgsLayoutMeasurement::Points );
  s2 = s1;
  QCOMPARE( s2, s1 );
}

void TestQgsLayoutMeasurement::sizeOperators()
{
  QgsLayoutSize s1( 6.0, 12.0, QgsLayoutMeasurement::Inches );

  // *
  QgsLayoutSize s2 = s1 * 3.0;
  QCOMPARE( s2.units(), s1.units() );
  QCOMPARE( s2.width(), 18.0 );
  QCOMPARE( s2.height(), 36.0 );

  // *=
  QgsLayoutSize s3( 6.0, 12.0, QgsLayoutMeasurement::Inches );
  s3 *= 2.0;
  QCOMPARE( s3.units(), QgsLayoutMeasurement::Inches );
  QCOMPARE( s3.width(), 12.0 );
  QCOMPARE( s3.height(), 24.0 );

  // /
  QgsLayoutSize s4 = s1 / 3.0;
  QCOMPARE( s4.units(), s1.units() );
  QCOMPARE( s4.width(), 2.0 );
  QCOMPARE( s4.height(), 4.0 );

  // /=
  QgsLayoutSize s5( 6.0, 12.0, QgsLayoutMeasurement::Inches );
  s5 /= 2.0;
  QCOMPARE( s5.units(), QgsLayoutMeasurement::Inches );
  QCOMPARE( s5.width(), 3.0 );
  QCOMPARE( s5.height(), 6.0 );
}

void TestQgsLayoutMeasurement::toQSizeF()
{
  QgsLayoutSize size( 6.0, 12.0, QgsLayoutMeasurement::Inches );
  QSizeF converted = size.toQSizeF();
  QCOMPARE( converted.width(), size.width() );
  QCOMPARE( converted.height(), size.height() );
}

void TestQgsLayoutMeasurement::converterCreate()
{
  QgsLayoutMeasurementConverter* converter = new QgsLayoutMeasurementConverter();
  QVERIFY( converter );
  delete converter;
}

void TestQgsLayoutMeasurement::converterCopy()
{
  QgsLayoutMeasurementConverter source;
  source.setDpi( 96.0 );
  QgsLayoutMeasurementConverter copy( source );
  QCOMPARE( copy.dpi(), source.dpi() );
}

void TestQgsLayoutMeasurement::conversionToMM()
{
  QgsLayoutMeasurement measurementInMillimeters( 1.0, QgsLayoutMeasurement::Millimeters );
  QgsLayoutMeasurement measurementInCentimeters( 1.0, QgsLayoutMeasurement::Centimeters );
  QgsLayoutMeasurement measurementInMeters( 1.0, QgsLayoutMeasurement::Meters );
  QgsLayoutMeasurement measurementInInches( 1.0, QgsLayoutMeasurement::Inches );
  QgsLayoutMeasurement measurementInFeet( 1.0, QgsLayoutMeasurement::Feet );
  QgsLayoutMeasurement measurementInPoints( 1.0, QgsLayoutMeasurement::Points );
  QgsLayoutMeasurement measurementInPicas( 1.0, QgsLayoutMeasurement::Picas );

  QgsLayoutMeasurementConverter converter;
  QgsLayoutMeasurement convertedFromMillimeters = converter.convert( measurementInMillimeters, QgsLayoutMeasurement::Millimeters );
  QgsLayoutMeasurement convertedFromCentimeters = converter.convert( measurementInCentimeters, QgsLayoutMeasurement::Millimeters );
  QgsLayoutMeasurement convertedFromMeters = converter.convert( measurementInMeters, QgsLayoutMeasurement::Millimeters );
  QgsLayoutMeasurement convertedFromInches = converter.convert( measurementInInches, QgsLayoutMeasurement::Millimeters );
  QgsLayoutMeasurement convertedFromFeet = converter.convert( measurementInFeet, QgsLayoutMeasurement::Millimeters );
  QgsLayoutMeasurement convertedFromPoints = converter.convert( measurementInPoints, QgsLayoutMeasurement::Millimeters );
  QgsLayoutMeasurement convertedFromPicas = converter.convert( measurementInPicas, QgsLayoutMeasurement::Millimeters );

  QCOMPARE( convertedFromMillimeters.units(), QgsLayoutMeasurement::Millimeters );
  QCOMPARE( convertedFromCentimeters.units(), QgsLayoutMeasurement::Millimeters );
  QCOMPARE( convertedFromMeters.units(), QgsLayoutMeasurement::Millimeters );
  QCOMPARE( convertedFromInches.units(), QgsLayoutMeasurement::Millimeters );
  QCOMPARE( convertedFromFeet.units(), QgsLayoutMeasurement::Millimeters );
  QCOMPARE( convertedFromPoints.units(), QgsLayoutMeasurement::Millimeters );
  QCOMPARE( convertedFromPicas.units(), QgsLayoutMeasurement::Millimeters );

  QCOMPARE( convertedFromMillimeters.length(), 1.0 );
  QCOMPARE( convertedFromCentimeters.length(), 10.0 );
  QCOMPARE( convertedFromMeters.length(), 1000.0 );
  QCOMPARE( convertedFromInches.length(), 25.4 );
  QCOMPARE( convertedFromFeet.length(), 304.8 );
  QCOMPARE( convertedFromPoints.length(), 0.352777778 );
  QCOMPARE( convertedFromPicas.length(), 4.23333333 );
}

void TestQgsLayoutMeasurement::conversionToCM()
{
  QgsLayoutMeasurement measurementInMillimeters( 1.0, QgsLayoutMeasurement::Millimeters );
  QgsLayoutMeasurement measurementInCentimeters( 1.0, QgsLayoutMeasurement::Centimeters );
  QgsLayoutMeasurement measurementInMeters( 1.0, QgsLayoutMeasurement::Meters );
  QgsLayoutMeasurement measurementInInches( 1.0, QgsLayoutMeasurement::Inches );
  QgsLayoutMeasurement measurementInFeet( 1.0, QgsLayoutMeasurement::Feet );
  QgsLayoutMeasurement measurementInPoints( 1.0, QgsLayoutMeasurement::Points );
  QgsLayoutMeasurement measurementInPicas( 1.0, QgsLayoutMeasurement::Picas );

  QgsLayoutMeasurementConverter converter;
  QgsLayoutMeasurement convertedFromMillimeters = converter.convert( measurementInMillimeters, QgsLayoutMeasurement::Centimeters );
  QgsLayoutMeasurement convertedFromCentimeters = converter.convert( measurementInCentimeters, QgsLayoutMeasurement::Centimeters );
  QgsLayoutMeasurement convertedFromMeters = converter.convert( measurementInMeters, QgsLayoutMeasurement::Centimeters );
  QgsLayoutMeasurement convertedFromInches = converter.convert( measurementInInches, QgsLayoutMeasurement::Centimeters );
  QgsLayoutMeasurement convertedFromFeet = converter.convert( measurementInFeet, QgsLayoutMeasurement::Centimeters );
  QgsLayoutMeasurement convertedFromPoints = converter.convert( measurementInPoints, QgsLayoutMeasurement::Centimeters );
  QgsLayoutMeasurement convertedFromPicas = converter.convert( measurementInPicas, QgsLayoutMeasurement::Centimeters );

  QCOMPARE( convertedFromMillimeters.units(), QgsLayoutMeasurement::Centimeters );
  QCOMPARE( convertedFromCentimeters.units(), QgsLayoutMeasurement::Centimeters );
  QCOMPARE( convertedFromMeters.units(), QgsLayoutMeasurement::Centimeters );
  QCOMPARE( convertedFromInches.units(), QgsLayoutMeasurement::Centimeters );
  QCOMPARE( convertedFromFeet.units(), QgsLayoutMeasurement::Centimeters );
  QCOMPARE( convertedFromPoints.units(), QgsLayoutMeasurement::Centimeters );
  QCOMPARE( convertedFromPicas.units(), QgsLayoutMeasurement::Centimeters );

  QCOMPARE( convertedFromMillimeters.length(), 0.1 );
  QCOMPARE( convertedFromCentimeters.length(), 1.0 );
  QCOMPARE( convertedFromMeters.length(), 100.0 );
  QCOMPARE( convertedFromInches.length(), 2.54 );
  QCOMPARE( convertedFromFeet.length(), 30.48 );
  QCOMPARE( convertedFromPoints.length(), 0.0352777778 );
  QCOMPARE( convertedFromPicas.length(), 0.423333333 );
}

void TestQgsLayoutMeasurement::conversionToM()
{
  QgsLayoutMeasurement measurementInMillimeters( 1.0, QgsLayoutMeasurement::Millimeters );
  QgsLayoutMeasurement measurementInCentimeters( 1.0, QgsLayoutMeasurement::Centimeters );
  QgsLayoutMeasurement measurementInMeters( 1.0, QgsLayoutMeasurement::Meters );
  QgsLayoutMeasurement measurementInInches( 1.0, QgsLayoutMeasurement::Inches );
  QgsLayoutMeasurement measurementInFeet( 1.0, QgsLayoutMeasurement::Feet );
  QgsLayoutMeasurement measurementInPoints( 1.0, QgsLayoutMeasurement::Points );
  QgsLayoutMeasurement measurementInPicas( 1.0, QgsLayoutMeasurement::Picas );

  QgsLayoutMeasurementConverter converter;
  QgsLayoutMeasurement convertedFromMillimeters = converter.convert( measurementInMillimeters, QgsLayoutMeasurement::Meters );
  QgsLayoutMeasurement convertedFromCentimeters = converter.convert( measurementInCentimeters, QgsLayoutMeasurement::Meters );
  QgsLayoutMeasurement convertedFromMeters = converter.convert( measurementInMeters, QgsLayoutMeasurement::Meters );
  QgsLayoutMeasurement convertedFromInches = converter.convert( measurementInInches, QgsLayoutMeasurement::Meters );
  QgsLayoutMeasurement convertedFromFeet = converter.convert( measurementInFeet, QgsLayoutMeasurement::Meters );
  QgsLayoutMeasurement convertedFromPoints = converter.convert( measurementInPoints, QgsLayoutMeasurement::Meters );
  QgsLayoutMeasurement convertedFromPicas = converter.convert( measurementInPicas, QgsLayoutMeasurement::Meters );

  QCOMPARE( convertedFromMillimeters.units(), QgsLayoutMeasurement::Meters );
  QCOMPARE( convertedFromCentimeters.units(), QgsLayoutMeasurement::Meters );
  QCOMPARE( convertedFromMeters.units(), QgsLayoutMeasurement::Meters );
  QCOMPARE( convertedFromInches.units(), QgsLayoutMeasurement::Meters );
  QCOMPARE( convertedFromFeet.units(), QgsLayoutMeasurement::Meters );
  QCOMPARE( convertedFromPoints.units(), QgsLayoutMeasurement::Meters );
  QCOMPARE( convertedFromPicas.units(), QgsLayoutMeasurement::Meters );

  QCOMPARE( convertedFromMillimeters.length(), 0.001 );
  QCOMPARE( convertedFromCentimeters.length(), 0.01 );
  QCOMPARE( convertedFromMeters.length(), 1.0 );
  QCOMPARE( convertedFromInches.length(), 0.0254 );
  QCOMPARE( convertedFromFeet.length(), 0.3048 );
  QCOMPARE( convertedFromPoints.length(), 0.000352777778 );
  QCOMPARE( convertedFromPicas.length(), 0.00423333333 );
}

void TestQgsLayoutMeasurement::conversionToInches()
{
  QgsLayoutMeasurement measurementInMillimeters( 1.0, QgsLayoutMeasurement::Millimeters );
  QgsLayoutMeasurement measurementInCentimeters( 1.0, QgsLayoutMeasurement::Centimeters );
  QgsLayoutMeasurement measurementInMeters( 1.0, QgsLayoutMeasurement::Meters );
  QgsLayoutMeasurement measurementInInches( 1.0, QgsLayoutMeasurement::Inches );
  QgsLayoutMeasurement measurementInFeet( 1.0, QgsLayoutMeasurement::Feet );
  QgsLayoutMeasurement measurementInPoints( 1.0, QgsLayoutMeasurement::Points );
  QgsLayoutMeasurement measurementInPicas( 1.0, QgsLayoutMeasurement::Picas );

  QgsLayoutMeasurementConverter converter;
  QgsLayoutMeasurement convertedFromMillimeters = converter.convert( measurementInMillimeters, QgsLayoutMeasurement::Inches );
  QgsLayoutMeasurement convertedFromCentimeters = converter.convert( measurementInCentimeters, QgsLayoutMeasurement::Inches );
  QgsLayoutMeasurement convertedFromMeters = converter.convert( measurementInMeters, QgsLayoutMeasurement::Inches );
  QgsLayoutMeasurement convertedFromInches = converter.convert( measurementInInches, QgsLayoutMeasurement::Inches );
  QgsLayoutMeasurement convertedFromFeet = converter.convert( measurementInFeet, QgsLayoutMeasurement::Inches );
  QgsLayoutMeasurement convertedFromPoints = converter.convert( measurementInPoints, QgsLayoutMeasurement::Inches );
  QgsLayoutMeasurement convertedFromPicas = converter.convert( measurementInPicas, QgsLayoutMeasurement::Inches );

  QCOMPARE( convertedFromMillimeters.units(), QgsLayoutMeasurement::Inches );
  QCOMPARE( convertedFromCentimeters.units(), QgsLayoutMeasurement::Inches );
  QCOMPARE( convertedFromMeters.units(), QgsLayoutMeasurement::Inches );
  QCOMPARE( convertedFromInches.units(), QgsLayoutMeasurement::Inches );
  QCOMPARE( convertedFromFeet.units(), QgsLayoutMeasurement::Inches );
  QCOMPARE( convertedFromPoints.units(), QgsLayoutMeasurement::Inches );
  QCOMPARE( convertedFromPicas.units(), QgsLayoutMeasurement::Inches );

  QVERIFY( qgsDoubleNear( convertedFromMillimeters.length(), 0.0393701, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromCentimeters.length(), 0.3937008, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromMeters.length(), 39.3700787, 0.000001 ) );
  QCOMPARE( convertedFromInches.length(), 1.0 );
  QCOMPARE( convertedFromFeet.length(), 12.0 );
  QVERIFY( qgsDoubleNear( convertedFromPoints.length(), 0.0138888889, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromPicas.length(), 0.166666667, 0.000001 ) );
}

void TestQgsLayoutMeasurement::conversionToFeet()
{
  QgsLayoutMeasurement measurementInMillimeters( 1.0, QgsLayoutMeasurement::Millimeters );
  QgsLayoutMeasurement measurementInCentimeters( 1.0, QgsLayoutMeasurement::Centimeters );
  QgsLayoutMeasurement measurementInMeters( 1.0, QgsLayoutMeasurement::Meters );
  QgsLayoutMeasurement measurementInInches( 1.0, QgsLayoutMeasurement::Inches );
  QgsLayoutMeasurement measurementInFeet( 1.0, QgsLayoutMeasurement::Feet );
  QgsLayoutMeasurement measurementInPoints( 1.0, QgsLayoutMeasurement::Points );
  QgsLayoutMeasurement measurementInPicas( 1.0, QgsLayoutMeasurement::Picas );

  QgsLayoutMeasurementConverter converter;
  QgsLayoutMeasurement convertedFromMillimeters = converter.convert( measurementInMillimeters, QgsLayoutMeasurement::Feet );
  QgsLayoutMeasurement convertedFromCentimeters = converter.convert( measurementInCentimeters, QgsLayoutMeasurement::Feet );
  QgsLayoutMeasurement convertedFromMeters = converter.convert( measurementInMeters, QgsLayoutMeasurement::Feet );
  QgsLayoutMeasurement convertedFromInches = converter.convert( measurementInInches, QgsLayoutMeasurement::Feet );
  QgsLayoutMeasurement convertedFromFeet = converter.convert( measurementInFeet, QgsLayoutMeasurement::Feet );
  QgsLayoutMeasurement convertedFromPoints = converter.convert( measurementInPoints, QgsLayoutMeasurement::Feet );
  QgsLayoutMeasurement convertedFromPicas = converter.convert( measurementInPicas, QgsLayoutMeasurement::Feet );

  QCOMPARE( convertedFromMillimeters.units(), QgsLayoutMeasurement::Feet );
  QCOMPARE( convertedFromCentimeters.units(), QgsLayoutMeasurement::Feet );
  QCOMPARE( convertedFromMeters.units(), QgsLayoutMeasurement::Feet );
  QCOMPARE( convertedFromInches.units(), QgsLayoutMeasurement::Feet );
  QCOMPARE( convertedFromFeet.units(), QgsLayoutMeasurement::Feet );
  QCOMPARE( convertedFromPoints.units(), QgsLayoutMeasurement::Feet );
  QCOMPARE( convertedFromPicas.units(), QgsLayoutMeasurement::Feet );

  QVERIFY( qgsDoubleNear( convertedFromMillimeters.length(), 0.0032808399, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromCentimeters.length(), 0.032808399, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromMeters.length(), 3.2808399, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromInches.length(), 0.0833333, 0.000001 ) );
  QCOMPARE( convertedFromFeet.length(), 1.0 );
  QVERIFY( qgsDoubleNear( convertedFromPoints.length(), 0.00115740741, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromPicas.length(), 0.0138888889, 0.000001 ) );
}

void TestQgsLayoutMeasurement::conversionToPoints()
{
  QgsLayoutMeasurement measurementInMillimeters( 1.0, QgsLayoutMeasurement::Millimeters );
  QgsLayoutMeasurement measurementInCentimeters( 1.0, QgsLayoutMeasurement::Centimeters );
  QgsLayoutMeasurement measurementInMeters( 1.0, QgsLayoutMeasurement::Meters );
  QgsLayoutMeasurement measurementInInches( 1.0, QgsLayoutMeasurement::Inches );
  QgsLayoutMeasurement measurementInFeet( 1.0, QgsLayoutMeasurement::Feet );
  QgsLayoutMeasurement measurementInPoints( 1.0, QgsLayoutMeasurement::Points );
  QgsLayoutMeasurement measurementInPicas( 1.0, QgsLayoutMeasurement::Picas );

  QgsLayoutMeasurementConverter converter;
  QgsLayoutMeasurement convertedFromMillimeters = converter.convert( measurementInMillimeters, QgsLayoutMeasurement::Points );
  QgsLayoutMeasurement convertedFromCentimeters = converter.convert( measurementInCentimeters, QgsLayoutMeasurement::Points );
  QgsLayoutMeasurement convertedFromMeters = converter.convert( measurementInMeters, QgsLayoutMeasurement::Points );
  QgsLayoutMeasurement convertedFromInches = converter.convert( measurementInInches, QgsLayoutMeasurement::Points );
  QgsLayoutMeasurement convertedFromFeet = converter.convert( measurementInFeet, QgsLayoutMeasurement::Points );
  QgsLayoutMeasurement convertedFromPoints = converter.convert( measurementInPoints, QgsLayoutMeasurement::Points );
  QgsLayoutMeasurement convertedFromPicas = converter.convert( measurementInPicas, QgsLayoutMeasurement::Points );

  QCOMPARE( convertedFromMillimeters.units(), QgsLayoutMeasurement::Points );
  QCOMPARE( convertedFromCentimeters.units(), QgsLayoutMeasurement::Points );
  QCOMPARE( convertedFromMeters.units(), QgsLayoutMeasurement::Points );
  QCOMPARE( convertedFromInches.units(), QgsLayoutMeasurement::Points );
  QCOMPARE( convertedFromFeet.units(), QgsLayoutMeasurement::Points );
  QCOMPARE( convertedFromPoints.units(), QgsLayoutMeasurement::Points );
  QCOMPARE( convertedFromPicas.units(), QgsLayoutMeasurement::Points );

  QVERIFY( qgsDoubleNear( convertedFromMillimeters.length(), 2.83464567, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromCentimeters.length(), 28.3464567, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromMeters.length(), 2834.64567, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromInches.length(), 72.0, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromFeet.length(), 864.0, 0.000001 ) );
  QCOMPARE( convertedFromPoints.length(), 1.0 );
  QVERIFY( qgsDoubleNear( convertedFromPicas.length(), 12.0, 0.000001 ) );
}

void TestQgsLayoutMeasurement::conversionToPicas()
{
  QgsLayoutMeasurement measurementInMillimeters( 1.0, QgsLayoutMeasurement::Millimeters );
  QgsLayoutMeasurement measurementInCentimeters( 1.0, QgsLayoutMeasurement::Centimeters );
  QgsLayoutMeasurement measurementInMeters( 1.0, QgsLayoutMeasurement::Meters );
  QgsLayoutMeasurement measurementInInches( 1.0, QgsLayoutMeasurement::Inches );
  QgsLayoutMeasurement measurementInFeet( 1.0, QgsLayoutMeasurement::Feet );
  QgsLayoutMeasurement measurementInPoints( 1.0, QgsLayoutMeasurement::Points );
  QgsLayoutMeasurement measurementInPicas( 1.0, QgsLayoutMeasurement::Picas );

  QgsLayoutMeasurementConverter converter;
  QgsLayoutMeasurement convertedFromMillimeters = converter.convert( measurementInMillimeters, QgsLayoutMeasurement::Picas );
  QgsLayoutMeasurement convertedFromCentimeters = converter.convert( measurementInCentimeters, QgsLayoutMeasurement::Picas );
  QgsLayoutMeasurement convertedFromMeters = converter.convert( measurementInMeters, QgsLayoutMeasurement::Picas );
  QgsLayoutMeasurement convertedFromInches = converter.convert( measurementInInches, QgsLayoutMeasurement::Picas );
  QgsLayoutMeasurement convertedFromFeet = converter.convert( measurementInFeet, QgsLayoutMeasurement::Picas );
  QgsLayoutMeasurement convertedFromPoints = converter.convert( measurementInPoints, QgsLayoutMeasurement::Picas );
  QgsLayoutMeasurement convertedFromPicas = converter.convert( measurementInPicas, QgsLayoutMeasurement::Picas );

  QCOMPARE( convertedFromMillimeters.units(), QgsLayoutMeasurement::Picas );
  QCOMPARE( convertedFromCentimeters.units(), QgsLayoutMeasurement::Picas );
  QCOMPARE( convertedFromMeters.units(), QgsLayoutMeasurement::Picas );
  QCOMPARE( convertedFromInches.units(), QgsLayoutMeasurement::Picas );
  QCOMPARE( convertedFromFeet.units(), QgsLayoutMeasurement::Picas );
  QCOMPARE( convertedFromPoints.units(), QgsLayoutMeasurement::Picas );
  QCOMPARE( convertedFromPicas.units(), QgsLayoutMeasurement::Picas );

  QVERIFY( qgsDoubleNear( convertedFromMillimeters.length(), 0.236220472, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromCentimeters.length(), 2.36220472, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromMeters.length(), 236.220472, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromInches.length(), 6.0, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromFeet.length(), 72.0, 0.000001 ) );
  QVERIFY( qgsDoubleNear( convertedFromPoints.length(), 0.0833333333, 0.000001 ) );
  QCOMPARE( convertedFromPicas.length(), 1.0 );
}

void TestQgsLayoutMeasurement::converterGettersSetters()
{
  QgsLayoutMeasurementConverter converter;
  converter.setDpi( 96.0 );
  QCOMPARE( converter.dpi(), 96.0 );
}

void TestQgsLayoutMeasurement::conversionFromPixels()
{
  QgsLayoutMeasurement measurementInPixels( 300.0, QgsLayoutMeasurement::Pixels );

  QgsLayoutMeasurementConverter converter;
  //try initially with 300 dpi
  converter.setDpi( 300.0 );
  QgsLayoutMeasurement convertedToInches = converter.convert( measurementInPixels, QgsLayoutMeasurement::Inches );
  QCOMPARE( convertedToInches.units(), QgsLayoutMeasurement::Inches );
  QVERIFY( qgsDoubleNear( convertedToInches.length(), 1.0, 0.000001 ) );
  QgsLayoutMeasurement convertedToMM = converter.convert( measurementInPixels, QgsLayoutMeasurement::Millimeters );
  QCOMPARE( convertedToMM.units(), QgsLayoutMeasurement::Millimeters );
  QVERIFY( qgsDoubleNear( convertedToMM.length(), 25.4, 0.000001 ) );

  //try with 96 dpi
  converter.setDpi( 96.0 );
  convertedToInches = converter.convert( measurementInPixels, QgsLayoutMeasurement::Inches );
  QCOMPARE( convertedToInches.units(), QgsLayoutMeasurement::Inches );
  QVERIFY( qgsDoubleNear( convertedToInches.length(), 3.125, 0.000001 ) );
  convertedToMM = converter.convert( measurementInPixels, QgsLayoutMeasurement::Millimeters );
  QCOMPARE( convertedToMM.units(), QgsLayoutMeasurement::Millimeters );
  QVERIFY( qgsDoubleNear( convertedToMM.length(), 79.375, 0.000001 ) );
}

void TestQgsLayoutMeasurement::conversionToPixels()
{
  QgsLayoutMeasurement measurementInInches( 1.0, QgsLayoutMeasurement::Inches );
  QgsLayoutMeasurement measurementInMM( 1.0, QgsLayoutMeasurement::Millimeters );

  QgsLayoutMeasurementConverter converter;
  //try initially with 300 dpi
  converter.setDpi( 300.0 );
  QgsLayoutMeasurement convertedToPixels = converter.convert( measurementInInches, QgsLayoutMeasurement::Pixels );
  QCOMPARE( convertedToPixels.units(), QgsLayoutMeasurement::Pixels );
  QVERIFY( qgsDoubleNear( convertedToPixels.length(), 300.0, 0.000001 ) );
  convertedToPixels = converter.convert( measurementInMM, QgsLayoutMeasurement::Pixels );
  QCOMPARE( convertedToPixels.units(), QgsLayoutMeasurement::Pixels );
  QVERIFY( qgsDoubleNear( convertedToPixels.length(), 11.811023622, 0.000001 ) );

  //try with 96 dpi
  converter.setDpi( 96.0 );
  convertedToPixels = converter.convert( measurementInInches, QgsLayoutMeasurement::Pixels );
  QCOMPARE( convertedToPixels.units(), QgsLayoutMeasurement::Pixels );
  QVERIFY( qgsDoubleNear( convertedToPixels.length(), 96.0, 0.000001 ) );
  convertedToPixels = converter.convert( measurementInMM, QgsLayoutMeasurement::Pixels );
  QCOMPARE( convertedToPixels.units(), QgsLayoutMeasurement::Pixels );
  QVERIFY( qgsDoubleNear( convertedToPixels.length(), 3.77952755906, 0.000001 ) );
}

void TestQgsLayoutMeasurement::sizeConversion()
{
  QgsLayoutSize sizeInMM( 1.0, 2.0, QgsLayoutMeasurement::Millimeters );
  QgsLayoutMeasurementConverter converter;

  //test conversion to same units
  QgsLayoutSize convertedSize = converter.convert( sizeInMM, QgsLayoutMeasurement::Millimeters );
  QCOMPARE( convertedSize, sizeInMM );

  //convert to other units
  convertedSize = converter.convert( sizeInMM, QgsLayoutMeasurement::Centimeters );
  QgsLayoutSize expectedSize( 0.1, 0.2, QgsLayoutMeasurement::Centimeters );
  QCOMPARE( convertedSize, expectedSize );

  //pixel conversion
  converter.setDpi( 96.0 );
  QgsLayoutSize convertedToInches = converter.convert( QgsLayoutSize( 96, 192, QgsLayoutMeasurement::Pixels ), QgsLayoutMeasurement::Inches );
  QCOMPARE( convertedToInches.width(), 1.0 );
  QCOMPARE( convertedToInches.height(), 2.0 );
  QCOMPARE( convertedToInches.units(), QgsLayoutMeasurement::Inches );
}

QTEST_MAIN( TestQgsLayoutMeasurement )
#include "testqgslayoutmeasurement.moc"
