/***************************************************************************
                         testqgslayout.cpp
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

#include "qgslayout.h"
#include "qgslayoutmeasurementconverter.h"
#include "qgis.h"
#include <QObject>
#include <QtTest/QtTest>

class TestQgsLayout: public QObject
{
    Q_OBJECT;

  private slots:
    void initTestCase();// will be called before the first testfunction is executed.
    void cleanupTestCase();// will be called after the last testfunction was executed.
    void init();// will be called before each testfunction is executed.
    void cleanup();// will be called after every testfunction.
    void creation(); //test creation of QgsLayout
    void context();
    void name();
    void dpi();
    void units();

  private:
    QString mReport;

};

void TestQgsLayout::initTestCase()
{
  mReport = "<h1>Layout Tests</h1>\n";
}

void TestQgsLayout::cleanupTestCase()
{
  QString myReportFile = QDir::tempPath() + QDir::separator() + "qgistest.html";
  QFile myFile( myReportFile );
  if ( myFile.open( QIODevice::WriteOnly | QIODevice::Append ) )
  {
    QTextStream myQTextStream( &myFile );
    myQTextStream << mReport;
    myFile.close();
  }
}

void TestQgsLayout::init()
{

}

void TestQgsLayout::cleanup()
{

}

void TestQgsLayout::creation()
{
  QgsLayout* layout = new QgsLayout();
  QVERIFY( layout );
  delete layout;
}

void TestQgsLayout::context()
{
  //test accessing layout context
  QgsLayout layout;
  QVERIFY( layout.context() );
}

void TestQgsLayout::name()
{
  QgsLayout layout;
  QString layoutName = "test name";
  layout.setName( layoutName );
  QCOMPARE( layout.name(), layoutName );
}

void TestQgsLayout::dpi()
{
  QgsLayout layout;
  layout.setDpi( 96.0 );
  QCOMPARE( layout.dpi(), 96.0 );

  QSignalSpy spyDpiChanged( &layout, SIGNAL( dpiChanged( const double ) ) );
  layout.setDpi( 100.0 );
  QVERIFY( spyDpiChanged.count() == 1 );
  layout.setDpi( 100.0 ); //should not emit a signal, as no change
  QVERIFY( spyDpiChanged.count() == 1 );
}

void TestQgsLayout::units()
{
  QgsLayout layout;
  QVERIFY( layout.measurementConverter() );
  layout.setUnits( QgsLayoutUnits::Centimeters );
  QCOMPARE( layout.units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( layout.convertToLayoutUnits( QgsLayoutMeasurement( 10.0, QgsLayoutUnits::Millimeters ) ), 1.0 );
  QCOMPARE( layout.convertToLayoutUnits( QgsLayoutSize( 10.0, 20.0, QgsLayoutUnits::Millimeters ) ), QSizeF( 1.0, 2.0 ) );
  QCOMPARE( layout.convertToLayoutUnits( QgsLayoutPoint( 10.0, 20.0, QgsLayoutUnits::Millimeters ) ), QPointF( 1.0, 2.0 ) );
  QCOMPARE( layout.convertFromLayoutUnits( 1.0, QgsLayoutUnits::Millimeters ), QgsLayoutMeasurement( 10.0, QgsLayoutUnits::Millimeters ) );
  QCOMPARE( layout.convertFromLayoutUnits( QSizeF( 1.0, 2.0 ), QgsLayoutUnits::Millimeters ), QgsLayoutSize( 10.0, 20.0, QgsLayoutUnits::Millimeters ) );
  QCOMPARE( layout.convertFromLayoutUnits( QPointF( 1.0, 2.0 ), QgsLayoutUnits::Millimeters ), QgsLayoutPoint( 10.0, 20.0, QgsLayoutUnits::Millimeters ) );

  //check with dpi conversion
  layout.setUnits( QgsLayoutUnits::Inches );
  layout.setDpi( 96.0 );
  QCOMPARE( layout.measurementConverter()->dpi(), 96.0 );
  QCOMPARE( layout.convertToLayoutUnits( QgsLayoutMeasurement( 96, QgsLayoutUnits::Pixels ) ), 1.0 );
  QCOMPARE( layout.convertToLayoutUnits( QgsLayoutSize( 96, 96, QgsLayoutUnits::Pixels ) ), QSizeF( 1.0, 1.0 ) );
  QCOMPARE( layout.convertToLayoutUnits( QgsLayoutPoint( 96, 96, QgsLayoutUnits::Pixels ) ), QPointF( 1.0, 1.0 ) );
  QgsLayoutMeasurement result = layout.convertFromLayoutUnits( 1.0, QgsLayoutUnits::Pixels );
  QCOMPARE( result.units(), QgsLayoutUnits::Pixels );
  QCOMPARE( result.length(), 96.0 );
  QgsLayoutSize sizeResult = layout.convertFromLayoutUnits( QSizeF( 1.0, 1.0 ), QgsLayoutUnits::Pixels );
  QCOMPARE( sizeResult.units(), QgsLayoutUnits::Pixels );
  QCOMPARE( sizeResult.width(), 96.0 );
  QCOMPARE( sizeResult.height(), 96.0 );
  QgsLayoutPoint pointResult = layout.convertFromLayoutUnits( QPointF( 1.0, 1.0 ), QgsLayoutUnits::Pixels );
  QCOMPARE( pointResult.units(), QgsLayoutUnits::Pixels );
  QCOMPARE( pointResult.x(), 96.0 );
  QCOMPARE( pointResult.y(), 96.0 );
  layout.setUnits( QgsLayoutUnits::Pixels );
  QCOMPARE( layout.convertToLayoutUnits( QgsLayoutMeasurement( 1, QgsLayoutUnits::Inches ) ), 96.0 );
  QCOMPARE( layout.convertToLayoutUnits( QgsLayoutSize( 1, 2, QgsLayoutUnits::Inches ) ), QSizeF( 96.0, 192.0 ) );
  result = layout.convertFromLayoutUnits( 96.0, QgsLayoutUnits::Inches );
  QCOMPARE( result.units(), QgsLayoutUnits::Inches );
  QCOMPARE( result.length(), 1.0 );
  sizeResult = layout.convertFromLayoutUnits( QSizeF( 96.0, 192.0 ), QgsLayoutUnits::Inches );
  QCOMPARE( sizeResult.units(), QgsLayoutUnits::Inches );
  QCOMPARE( sizeResult.width(), 1.0 );
  QCOMPARE( sizeResult.height(), 2.0 );
  pointResult = layout.convertFromLayoutUnits( QPointF( 96.0, 192.0 ), QgsLayoutUnits::Inches );
  QCOMPARE( pointResult.units(), QgsLayoutUnits::Inches );
  QCOMPARE( pointResult.x(), 1.0 );
  QCOMPARE( pointResult.y(), 2.0 );
}

QTEST_MAIN( TestQgsLayout )
#include "testqgslayout.moc"
