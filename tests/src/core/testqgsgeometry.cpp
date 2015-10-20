/***************************************************************************
     testqgsgeometry.cpp
     --------------------------------------
    Date                 : 20 Jan 2008
    Copyright            : (C) 2008 by Tim Sutton
    Email                : tim @ linfiniti.com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <QtTest/QtTest>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include <QDesktopServices>
#include <QVector>
#include <QPointF>
#include <QImage>
#include <QPainter>

//qgis includes...
#include <qgsapplication.h>
#include <qgsgeometry.h>
#include <qgspoint.h>
#include "qgspointv2.h"
#include "qgsgeometrycollectionv2.h"
#include "qgscurvepolygonv2.h"
#include "qgscurvev2.h"

//qgs unit test utility class
#include "qgsrenderchecker.h"

/** \ingroup UnitTests
 * This is a unit test for the different geometry operations on vector features.
 */
class TestQgsGeometry : public QObject
{
    Q_OBJECT

  public:
    TestQgsGeometry();

  private slots:
    void initTestCase();// will be called before the first testfunction is executed.
    void cleanupTestCase();// will be called after the last testfunction was executed.
    void init();// will be called before each testfunction is executed.
    void cleanup();// will be called after every testfunction.

    void copy();
    void assignment();
    void asVariant(); //test conversion to and from a QVariant
    void isEmpty();

    void fromQgsPoint();
    void fromQPoint();
    void fromQPolygonF();
    void asQPointF();
    void asQPolygonF();

    void comparePolylines();
    void comparePolygons();

    void checkGeometries_data();
    void checkGeometries();

    // MK, Disabled 14.11.2014
    // Too unclear what exactly should be tested and which variations are allowed for the line
#if 0
    void simplifyCheck1();
#endif
    void intersectionCheck1();
    void intersectionCheck2();
    void translateCheck1();
    void rotateCheck1();
    void unionCheck1();
    void unionCheck2();
    void differenceCheck1();
    void differenceCheck2();
    void bufferCheck();
    void smoothCheck();

    void dataStream();

  private:
    /** A helper method to do a render check to see if the geometry op is as expected */
    bool renderCheck( const QString& theTestName, const QString& theComment = "", int mismatchCount = 0 );
    /** A helper method to dump to qdebug the geometry of a multipolygon */
    void dumpMultiPolygon( QgsMultiPolygon &theMultiPolygon );
    /** A helper method to dump to qdebug the geometry of a polygon */
    void dumpPolygon( QgsPolygon &thePolygon );
    /** A helper method to dump to qdebug the geometry of a polyline */
    void dumpPolyline( QgsPolyline &thePolyline );
    QgsPoint mPoint1;
    QgsPoint mPoint2;
    QgsPoint mPoint3;
    QgsPoint mPoint4;
    QgsPoint mPointA;
    QgsPoint mPointB;
    QgsPoint mPointC;
    QgsPoint mPointD;
    QgsPoint mPointW;
    QgsPoint mPointX;
    QgsPoint mPointY;
    QgsPoint mPointZ;
    QgsPolyline mPolylineA;
    QgsPolyline mPolylineB;
    QgsPolyline mPolylineC;
    QgsGeometry * mpPolylineGeometryD;
    QgsPolygon mPolygonA;
    QgsPolygon mPolygonB;
    QgsPolygon mPolygonC;
    QgsGeometry * mpPolygonGeometryA;
    QgsGeometry * mpPolygonGeometryB;
    QgsGeometry * mpPolygonGeometryC;
    QString mWktLine;
    QString mTestDataDir;
    QImage mImage;
    QPainter * mpPainter;
    QPen mPen1;
    QPen mPen2;
    QString mReport;
};

TestQgsGeometry::TestQgsGeometry()
    : mpPolylineGeometryD( NULL )
    , mpPolygonGeometryA( NULL )
    , mpPolygonGeometryB( NULL )
    , mpPolygonGeometryC( NULL )
    , mpPainter( NULL )
{

}


void TestQgsGeometry::init()
{
  //
  // Reset / reinitialise the geometries before each test is run
  //
  mPoint1 = QgsPoint( 20.0, 20.0 );
  mPoint2 = QgsPoint( 80.0, 20.0 );
  mPoint3 = QgsPoint( 80.0, 80.0 );
  mPoint4 = QgsPoint( 20.0, 80.0 );
  mPointA = QgsPoint( 40.0, 40.0 );
  mPointB = QgsPoint( 100.0, 40.0 );
  mPointC = QgsPoint( 100.0, 100.0 );
  mPointD = QgsPoint( 40.0, 100.0 );
  mPointW = QgsPoint( 200.0, 200.0 );
  mPointX = QgsPoint( 240.0, 200.0 );
  mPointY = QgsPoint( 240.0, 240.0 );
  mPointZ = QgsPoint( 200.0, 240.0 );

  mWktLine = QString( "LINESTRING(117.623198 35.198654, 117.581274 35.198654, 117.078178 35.324427, 116.868555 35.534051, 116.617007 35.869448, 116.491233 35.953297, 116.155836 36.288694, 116.071987 36.372544, 115.443117 36.749865, 114.814247 37.043338, 114.311152 37.169112, 113.388810 37.378735, 113.095337 37.378735, 112.592241 37.378735, 111.753748 37.294886, 111.502201 37.252961, 111.082954 37.127187, 110.747557 37.127187, 110.160612 36.917564, 110.034838 36.833715, 109.741366 36.749865, 109.573667 36.666016, 109.238270 36.498317, 109.070571 36.414468, 108.819023 36.288694, 108.693250 36.246770, 108.483626 36.162920, 107.645134 35.911372, 106.597017 35.869448, 106.051997 35.701749, 105.800449 35.617900, 105.590826 35.575975, 105.297354 35.575975, 104.961956 35.575975, 104.710409 35.534051, 104.458861 35.492126, 103.871916 35.492126, 103.788066 35.492126, 103.326895 35.408277, 102.949574 35.408277, 102.488402 35.450201, 102.069156 35.450201, 101.482211 35.450201, 100.937191 35.659825, 100.308321 35.869448, 100.056773 36.037146, 99.050582 36.079071, 97.667069 35.743674, 97.163973 35.617900, 96.115857 35.534051, 95.612761 35.534051, 94.396947 35.911372, 93.684228 36.288694, 92.929584 36.833715, 92.258790 37.169112, 91.629920 37.504509, 90.414105 37.881831, 90.414105 37.881831, 90.246407 37.923755, 89.491763 37.839906, 89.156366 37.672207, 88.485572 37.504509, 87.814778 37.252961, 87.563230 37.169112, 87.143983 37.043338, 85.970093 36.875639, 85.802395 36.875639, 84.083484 36.959489, 84.041560 37.043338, 82.951519 37.546433, 82.699971 37.630283)" );

  mPolygonA.clear();
  mPolygonB.clear();
  mPolygonC.clear();
  mPolylineA.clear();
  mPolylineB.clear();
  mPolylineC.clear();
  mPolylineA << mPoint1 << mPoint2 << mPoint3 << mPoint4 << mPoint1;
  mPolygonA << mPolylineA;
  //Polygon B intersects Polygon A
  mPolylineB << mPointA << mPointB << mPointC << mPointD << mPointA;
  mPolygonB << mPolylineB;
  // Polygon C should intersect no other polys
  mPolylineC << mPointW << mPointX << mPointY << mPointZ << mPointW;
  mPolygonC << mPolylineC;

  mpPolylineGeometryD = QgsGeometry::fromWkt( mWktLine );

  //polygon: first item of the list is outer ring,
  // inner rings (if any) start from second item
  mpPolygonGeometryA = QgsGeometry::fromPolygon( mPolygonA );
  mpPolygonGeometryB = QgsGeometry::fromPolygon( mPolygonB );
  mpPolygonGeometryC = QgsGeometry::fromPolygon( mPolygonC );

  mImage = QImage( 250, 250, QImage::Format_RGB32 );
  mImage.fill( qRgb( 152, 219, 249 ) );
  mpPainter = new QPainter( &mImage );

  // Draw the test shapes first
  mPen1 = QPen();
  mPen1.setWidth( 5 );
  mPen1.setBrush( Qt::green );
  mpPainter->setPen( mPen1 );
  dumpPolygon( mPolygonA );
  mPen1.setBrush( Qt::red );
  mpPainter->setPen( mPen1 );
  dumpPolygon( mPolygonB );
  mPen1.setBrush( Qt::blue );
  mpPainter->setPen( mPen1 );
  dumpPolygon( mPolygonC );

  mPen2 = QPen();
  mPen2.setWidth( 1 );
  mPen2.setBrush( Qt::black );
  QBrush myBrush( Qt::DiagCrossPattern );


  //set the pen to a different color -
  //any test outs will be drawn in pen2
  mpPainter->setPen( mPen2 );
  mpPainter->setBrush( myBrush );
}

void TestQgsGeometry::cleanup()
{
  // will be called after every testfunction.
  delete mpPolygonGeometryA;
  delete mpPolygonGeometryB;
  delete mpPolygonGeometryC;
  delete mpPolylineGeometryD;
  delete mpPainter;
}

void TestQgsGeometry::copy()
{
  //create a point geometry
  QgsGeometry original( new QgsPointV2( 1.0, 2.0 ) );
  QCOMPARE( original.geometry()->vertexAt( QgsVertexId( 0, 0, 0 ) ).x(), 1.0 );
  QCOMPARE( original.geometry()->vertexAt( QgsVertexId( 0, 0, 0 ) ).y(), 2.0 );

  //implicitly shared copy
  QgsGeometry copy( original );
  QCOMPARE( copy.geometry()->vertexAt( QgsVertexId( 0, 0, 0 ) ).x(), 1.0 );
  QCOMPARE( copy.geometry()->vertexAt( QgsVertexId( 0, 0, 0 ) ).y(), 2.0 );

  //trigger a detach
  copy.setGeometry( new QgsPointV2( 3.0, 4.0 ) );
  QCOMPARE( copy.geometry()->vertexAt( QgsVertexId( 0, 0, 0 ) ).x(), 3.0 );
  QCOMPARE( copy.geometry()->vertexAt( QgsVertexId( 0, 0, 0 ) ).y(), 4.0 );

  //make sure original was untouched
  QCOMPARE( original.geometry()->vertexAt( QgsVertexId( 0, 0, 0 ) ).x(), 1.0 );
  QCOMPARE( original.geometry()->vertexAt( QgsVertexId( 0, 0, 0 ) ).y(), 2.0 );
}

void TestQgsGeometry::assignment()
{
  //create a point geometry
  QgsGeometry original( new QgsPointV2( 1.0, 2.0 ) );
  QCOMPARE( original.geometry()->vertexAt( QgsVertexId( 0, 0, 0 ) ).x(), 1.0 );
  QCOMPARE( original.geometry()->vertexAt( QgsVertexId( 0, 0, 0 ) ).y(), 2.0 );

  //assign to implicitly shared copy
  QgsGeometry copy;
  copy = original;
  QCOMPARE( copy.geometry()->vertexAt( QgsVertexId( 0, 0, 0 ) ).x(), 1.0 );
  QCOMPARE( copy.geometry()->vertexAt( QgsVertexId( 0, 0, 0 ) ).y(), 2.0 );

  //trigger a detach
  copy.setGeometry( new QgsPointV2( 3.0, 4.0 ) );
  QCOMPARE( copy.geometry()->vertexAt( QgsVertexId( 0, 0, 0 ) ).x(), 3.0 );
  QCOMPARE( copy.geometry()->vertexAt( QgsVertexId( 0, 0, 0 ) ).y(), 4.0 );

  //make sure original was untouched
  QCOMPARE( original.geometry()->vertexAt( QgsVertexId( 0, 0, 0 ) ).x(), 1.0 );
  QCOMPARE( original.geometry()->vertexAt( QgsVertexId( 0, 0, 0 ) ).y(), 2.0 );
}

void TestQgsGeometry::asVariant()
{
  //create a point geometry
  QgsGeometry original( new QgsPointV2( 1.0, 2.0 ) );
  QCOMPARE( original.geometry()->vertexAt( QgsVertexId( 0, 0, 0 ) ).x(), 1.0 );
  QCOMPARE( original.geometry()->vertexAt( QgsVertexId( 0, 0, 0 ) ).y(), 2.0 );

  //convert to and from a QVariant
  QVariant var = QVariant::fromValue( original );
  QVERIFY( var.isValid() );

  QgsGeometry fromVar = qvariant_cast<QgsGeometry>( var );
  QCOMPARE( fromVar.geometry()->vertexAt( QgsVertexId( 0, 0, 0 ) ).x(), 1.0 );
  QCOMPARE( fromVar.geometry()->vertexAt( QgsVertexId( 0, 0, 0 ) ).y(), 2.0 );

  //also check copying variant
  QVariant var2 = var;
  QVERIFY( var2.isValid() );
  QgsGeometry fromVar2 = qvariant_cast<QgsGeometry>( var2 );
  QCOMPARE( fromVar2.geometry()->vertexAt( QgsVertexId( 0, 0, 0 ) ).x(), 1.0 );
  QCOMPARE( fromVar2.geometry()->vertexAt( QgsVertexId( 0, 0, 0 ) ).y(), 2.0 );

  //modify original and check detachment
  original.setGeometry( new QgsPointV2( 3.0, 4.0 ) );
  QgsGeometry fromVar3 = qvariant_cast<QgsGeometry>( var );
  QCOMPARE( fromVar3.geometry()->vertexAt( QgsVertexId( 0, 0, 0 ) ).x(), 1.0 );
  QCOMPARE( fromVar3.geometry()->vertexAt( QgsVertexId( 0, 0, 0 ) ).y(), 2.0 );
}

void TestQgsGeometry::isEmpty()
{
  QgsGeometry geom;
  QVERIFY( geom.isEmpty() );

  geom.setGeometry( new QgsPointV2( 1.0, 2.0 ) );
  QVERIFY( !geom.isEmpty() );

  geom.setGeometry( 0 );
  QVERIFY( geom.isEmpty() );
}

void TestQgsGeometry::fromQgsPoint()
{
  QgsPoint point( 1.0, 2.0 );
  QSharedPointer<QgsGeometry> result( QgsGeometry::fromPoint( point ) );
  QCOMPARE( result->wkbType(), QGis::WKBPoint );
  QgsPoint resultPoint = result->asPoint();
  QCOMPARE( resultPoint, point );
}

void TestQgsGeometry::fromQPoint()
{
  QPointF point( 1.0, 2.0 );
  QSharedPointer<QgsGeometry> result( QgsGeometry::fromQPointF( point ) );
  QCOMPARE( result->wkbType(), QGis::WKBPoint );
  QgsPoint resultPoint = result->asPoint();
  QCOMPARE( resultPoint.x(), 1.0 );
  QCOMPARE( resultPoint.y(), 2.0 );
}

void TestQgsGeometry::fromQPolygonF()
{
  //test with a polyline
  QPolygonF polyline;
  polyline << QPointF( 1.0, 2.0 ) << QPointF( 4.0, 6.0 ) << QPointF( 4.0, 3.0 ) << QPointF( 2.0, 2.0 );
  QSharedPointer<QgsGeometry> result( QgsGeometry::fromQPolygonF( polyline ) );
  QCOMPARE( result->wkbType(), QGis::WKBLineString );
  QgsPolyline resultLine = result->asPolyline();
  QCOMPARE( resultLine.size(), 4 );
  QCOMPARE( resultLine.at( 0 ), QgsPoint( 1.0, 2.0 ) );
  QCOMPARE( resultLine.at( 1 ), QgsPoint( 4.0, 6.0 ) );
  QCOMPARE( resultLine.at( 2 ), QgsPoint( 4.0, 3.0 ) );
  QCOMPARE( resultLine.at( 3 ), QgsPoint( 2.0, 2.0 ) );

  //test with a closed polygon
  QPolygonF polygon;
  polygon << QPointF( 1.0, 2.0 ) << QPointF( 4.0, 6.0 ) << QPointF( 4.0, 3.0 ) << QPointF( 2.0, 2.0 ) << QPointF( 1.0, 2.0 );
  QSharedPointer<QgsGeometry> result2( QgsGeometry::fromQPolygonF( polygon ) );
  QCOMPARE( result2->wkbType(), QGis::WKBPolygon );
  QgsPolygon resultPolygon = result2->asPolygon();
  QCOMPARE( resultPolygon.size(), 1 );
  QCOMPARE( resultPolygon.at( 0 ).at( 0 ), QgsPoint( 1.0, 2.0 ) );
  QCOMPARE( resultPolygon.at( 0 ).at( 1 ), QgsPoint( 4.0, 6.0 ) );
  QCOMPARE( resultPolygon.at( 0 ).at( 2 ), QgsPoint( 4.0, 3.0 ) );
  QCOMPARE( resultPolygon.at( 0 ).at( 3 ), QgsPoint( 2.0, 2.0 ) );
  QCOMPARE( resultPolygon.at( 0 ).at( 4 ), QgsPoint( 1.0, 2.0 ) );
}

void TestQgsGeometry::asQPointF()
{
  QPointF point( 1.0, 2.0 );
  QSharedPointer<QgsGeometry> geom( QgsGeometry::fromQPointF( point ) );
  QPointF resultPoint = geom->asQPointF();
  QCOMPARE( resultPoint, point );

  //non point geom
  QPointF badPoint = mpPolygonGeometryA->asQPointF();
  QVERIFY( badPoint.isNull() );
}

void TestQgsGeometry::asQPolygonF()
{
  //test polygon
  QPolygonF fromPoly = mpPolygonGeometryA->asQPolygonF();
  QVERIFY( fromPoly.isClosed() );
  QCOMPARE( fromPoly.size(), 5 );
  QCOMPARE( fromPoly.at( 0 ).x(), mPoint1.x() );
  QCOMPARE( fromPoly.at( 0 ).y(), mPoint1.y() );
  QCOMPARE( fromPoly.at( 1 ).x(), mPoint2.x() );
  QCOMPARE( fromPoly.at( 1 ).y(), mPoint2.y() );
  QCOMPARE( fromPoly.at( 2 ).x(), mPoint3.x() );
  QCOMPARE( fromPoly.at( 2 ).y(), mPoint3.y() );
  QCOMPARE( fromPoly.at( 3 ).x(), mPoint4.x() );
  QCOMPARE( fromPoly.at( 3 ).y(), mPoint4.y() );
  QCOMPARE( fromPoly.at( 4 ).x(), mPoint1.x() );
  QCOMPARE( fromPoly.at( 4 ).y(), mPoint1.y() );

  //test polyline
  QgsPolyline testline;
  testline << mPoint1 << mPoint2 << mPoint3;
  QSharedPointer<QgsGeometry> lineGeom( QgsGeometry::fromPolyline( testline ) );
  QPolygonF fromLine = lineGeom->asQPolygonF();
  QVERIFY( !fromLine.isClosed() );
  QCOMPARE( fromLine.size(), 3 );
  QCOMPARE( fromLine.at( 0 ).x(), mPoint1.x() );
  QCOMPARE( fromLine.at( 0 ).y(), mPoint1.y() );
  QCOMPARE( fromLine.at( 1 ).x(), mPoint2.x() );
  QCOMPARE( fromLine.at( 1 ).y(), mPoint2.y() );
  QCOMPARE( fromLine.at( 2 ).x(), mPoint3.x() );
  QCOMPARE( fromLine.at( 2 ).y(), mPoint3.y() );

  //test a bad geometry
  QSharedPointer<QgsGeometry> badGeom( QgsGeometry::fromPoint( mPoint1 ) );
  QPolygonF fromBad = badGeom->asQPolygonF();
  QVERIFY( fromBad.isEmpty() );
}

void TestQgsGeometry::comparePolylines()
{
  QgsPolyline line1;
  line1 << mPoint1 << mPoint2 << mPoint3;
  QgsPolyline line2;
  line2 << mPoint1 << mPoint2 << mPoint3;
  QVERIFY( QgsGeometry::compare( line1, line2 ) );

  //different number of nodes
  QgsPolyline line3;
  line3 << mPoint1 << mPoint2 << mPoint3 << mPoint4;
  QVERIFY( !QgsGeometry::compare( line1, line3 ) );

  //different nodes
  QgsPolyline line4;
  line3 << mPoint1 << mPointA << mPoint3 << mPoint4;
  QVERIFY( !QgsGeometry::compare( line3, line4 ) );
}

void TestQgsGeometry::comparePolygons()
{
  QgsPolyline ring1;
  ring1 << mPoint1 << mPoint2 << mPoint3 << mPoint1;
  QgsPolyline ring2;
  ring2 << mPoint4 << mPointA << mPointB << mPoint4;
  QgsPolygon poly1;
  poly1 << ring1 << ring2;
  QgsPolygon poly2;
  poly2 << ring1 << ring2;
  QVERIFY( QgsGeometry::compare( poly1, poly2 ) );

  //different number of rings
  QgsPolygon poly3;
  poly3 << ring1;
  QVERIFY( !QgsGeometry::compare( poly1, poly3 ) );

  //different rings
  QgsPolygon poly4;
  poly4 << ring2;
  QVERIFY( !QgsGeometry::compare( poly3, poly4 ) );
}




void TestQgsGeometry::checkGeometries_data()
{
  QTest::addColumn<QString>( "wkt" );
  QTest::addColumn<QString>( "validWkt" );
  QTest::addColumn<int>( "numPoints" );
  QTest::addColumn<int>( "numGeometries" );
  QTest::addColumn<int>( "numRings" );
  QTest::addColumn<bool>( "isClosed" );
  QTest::addColumn<QString>( "centroid" );
  QTest::addColumn<double>( "xMin" );
  QTest::addColumn<double>( "yMin" );
  QTest::addColumn<double>( "xMax" );
  QTest::addColumn<double>( "yMax" );
  QTest::addColumn<double>( "area" );
  QTest::addColumn<double>( "length" );
  QTest::addColumn<double>( "perimeter" );
  QTest::newRow( "geometry 0" ) << "CIRCULARSTRING(268 415,227 505,227 406)"
  << "CIRCULARSTRING(268 415,227 505,227 406)"
  << 3 << 1 << 0
  << false
  << "Point (235.96400242068907005 463.05287766717373188)" << 186.995121893 << 406.0 << 288.248780547 << 506.126829327
  << 0.0 << 274.81542454 << 0.0;
  QTest::newRow( "geometry 1" ) << "CIRCULARSTRINGZ(268 415 1,227 505 2,227 406 3)"
  << "CIRCULARSTRINGZ(268 415 1,227 505 2,227 406 3)"
  << 3 << 1 << 0
  << false
  << "Point (235.96400242068907005 463.05287766717373188)" << 186.995121893 << 406.0 << 288.248780547 << 506.126829327
  << 0.0 << 274.81542454 << 0.0;
  QTest::newRow( "geometry 2" ) << "CIRCULARSTRINGM(268 415 4,227 505 5,227 406 6)"
  << "CIRCULARSTRINGM(268 415 4,227 505 5,227 406 6)"
  << 3 << 1 << 0
  << false
  << "Point (235.96400242068907005 463.05287766717373188)" << 186.995121893 << 406.0 << 288.248780547 << 506.126829327
  << 0.0 << 274.81542454 << 0.0;
  QTest::newRow( "geometry 3" ) << "CIRCULARSTRINGZM(268 415 1 6,227 505 2 8,227 406 3 9)"
  << "CIRCULARSTRINGZM(268 415 1 6,227 505 2 8,227 406 3 9)"
  << 3 << 1 << 0
  << false
  << "Point (235.96400242068907005 463.05287766717373188)" << 186.995121893 << 406.0 << 288.248780547 << 506.126829327
  << 0.0 << 274.81542454 << 0.0;
  QTest::newRow( "geometry 4" ) << "CIRCULARSTRING(268 415 1,227 505 2,227 406 3)"
  << "CIRCULARSTRINGZ(268 415 1,227 505 2,227 406 3)"
  << 3 << 1 << 0
  << false
  << "Point (235.96400242068907005 463.05287766717373188)" << 186.995121893 << 406.0 << 288.248780547 << 506.126829327
  << 0.0 << 274.81542454 << 0.0;
  QTest::newRow( "geometry 5" ) << "CIRCULARSTRING(268 415 1 6,227 505 2 8,227 406 3 9)"
  << "CIRCULARSTRINGZM(268 415 1 6,227 505 2 8,227 406 3 9)"
  << 3 << 1 << 0
  << false
  << "Point (235.96400242068907005 463.05287766717373188)" << 186.995121893 << 406.0 << 288.248780547 << 506.126829327
  << 0.0 << 274.81542454 << 0.0;
  QTest::newRow( "geometry 6" ) << "COMPOUNDCURVE((5 3, 5 13), CIRCULARSTRING(5 13, 7 15, 9 13), (9 13, 9 3), CIRCULARSTRING(9 3, 7 1, 5 3))"
  << "COMPOUNDCURVE((5 3, 5 13), CIRCULARSTRING(5 13, 7 15, 9 13), (9 13, 9 3), CIRCULARSTRING(9 3, 7 1, 5 3))"
  << 7 << 1 << 0
  << true
  << "POINT(7 8)" << 5.0 << 1.0 << 9.0 << 15.0
  << 0.0 << 32.5663706144 << 0.0;
  QTest::newRow( "geometry 7" ) << "COMPOUNDCURVEZ((5 3 1, 5 13 2), CIRCULARSTRINGZ(5 13 2, 7 15 3, 9 13 4), (9 13 4, 9 3 5), CIRCULARSTRINGZ(9 3 5, 7 1 6, 5 3 1))"
  << "COMPOUNDCURVEZ((5 3 1, 5 13 2), CIRCULARSTRINGZ(5 13 2, 7 15 3, 9 13 4), (9 13 4, 9 3 5), CIRCULARSTRINGZ(9 3 5, 7 1 6, 5 3 1))"
  << 7 << 1 << 0
  << true
  << "POINT(7 8)" << 5.0 << 1.0 << 9.0 << 15.0
  << 0.0 << 32.5663706144 << 0.0;
  QTest::newRow( "geometry 8" ) << "COMPOUNDCURVEM((5 3 1, 5 13 2), CIRCULARSTRINGM(5 13 2, 7 15 3, 9 13 4), (9 13 4, 9 3 5), CIRCULARSTRINGM(9 3 5, 7 1 6, 5 3 1))"
  << "COMPOUNDCURVEM((5 3 1, 5 13 2), CIRCULARSTRINGM(5 13 2, 7 15 3, 9 13 4), (9 13 4, 9 3 5), CIRCULARSTRINGM(9 3 5, 7 1 6, 5 3 1))"
  << 7 << 1 << 0
  << true
  << "POINT(7 8)" << 5.0 << 1.0 << 9.0 << 15.0
  << 0.0 << 32.5663706144 << 0.0;
  QTest::newRow( "geometry 9" ) << "COMPOUNDCURVEZM((5 3 1 11, 5 13 2 12), CIRCULARSTRINGZM(5 13 2 12, 7 15 3 13, 9 13 4 14), (9 13 4 14, 9 3 5 15), CIRCULARSTRINGZM(9 3 5 15, 7 1 6 16, 5 3 1 11))"
  << "COMPOUNDCURVEZM((5 3 1 11, 5 13 2 12), CIRCULARSTRINGZM(5 13 2 12, 7 15 3 13, 9 13 4 14), (9 13 4 14, 9 3 5 15), CIRCULARSTRINGZM(9 3 5 15, 7 1 6 16, 5 3 1 11))"
  << 7 << 1 << 0
  << true
  << "POINT(7 8)" << 5.0 << 1.0 << 9.0 << 15.0
  << 0.0 << 32.5663706144 << 0.0;
  QTest::newRow( "geometry 10" ) << "COMPOUNDCURVE((5 3 1, 5 13 2), CIRCULARSTRING(5 13 2, 7 15 3, 9 13 4), (9 13 4, 9 3 5), CIRCULARSTRING(9 3 5, 7 1 6, 5 3 1))"
  << "COMPOUNDCURVEZ((5 3 1, 5 13 2), CIRCULARSTRINGZ(5 13 2, 7 15 3, 9 13 4), (9 13 4, 9 3 5), CIRCULARSTRINGZ(9 3 5, 7 1 6, 5 3 1))"
  << 7 << 1 << 0
  << true
  << "POINT(7 8)" << 5.0 << 1.0 << 9.0 << 15.0
  << 0.0 << 32.5663706144 << 0.0;
  QTest::newRow( "geometry 11" ) << "COMPOUNDCURVE((5 3 1 11, 5 13 2 12), CIRCULARSTRING(5 13 2 12, 7 15 3 13, 9 13 4 14), (9 13 4 14, 9 3 5 15), CIRCULARSTRING(9 3 5 15, 7 1 6 16, 5 3 1 11))"
  << "COMPOUNDCURVEZM((5 3 1 11, 5 13 2 12), CIRCULARSTRINGZM(5 13 2 12, 7 15 3 13, 9 13 4 14), (9 13 4 14, 9 3 5 15), CIRCULARSTRINGZM(9 3 5 15, 7 1 6 16, 5 3 1 11))"
  << 7 << 1 << 0
  << true
  << "POINT(7 8)" << 5.0 << 1.0 << 9.0 << 15.0
  << 0.0 << 32.5663706144 << 0.0;
  QTest::newRow( "geometry 12" ) << "CURVEPOLYGON(CIRCULARSTRING(1 3, 3 5, 4 7, 7 3, 1 3))"
  << "CURVEPOLYGON(CIRCULARSTRING(1 3, 3 5, 4 7, 7 3, 1 3))"
  << 5 << 1 << 0
  << false
  << "Point (4.40851485241232677 3.56861296120545823)" << 1.0 << 0.75 << 7.125 << 7.0
  << 24.9520887446 << 0.0 << 18.926681924;
  QTest::newRow( "geometry 13" ) << "CURVEPOLYGONZ(CIRCULARSTRINGZ(1 3 1, 3 5 2, 4 7 3, 7 3 4, 1 3 1))"
  << "CURVEPOLYGONZ(CIRCULARSTRINGZ(1 3 1, 3 5 2, 4 7 3, 7 3 4, 1 3 1))"
  << 5 << 1 << 0
  << false
  << "Point (4.40851485241232677 3.56861296120545823)" << 1.0 << 0.75 << 7.125 << 7.0
  << 24.9520887446 << 0.0 << 18.926681924;
  QTest::newRow( "geometry 14" ) << "CURVEPOLYGONM(CIRCULARSTRINGM(1 3 1, 3 5 2, 4 7 3, 7 3 4, 1 3 1))"
  << "CURVEPOLYGONM(CIRCULARSTRINGM(1 3 1, 3 5 2, 4 7 3, 7 3 4, 1 3 1))"
  << 5 << 1 << 0
  << false
  << "Point (4.40851485241232677 3.56861296120545823)" << 1.0 << 0.75 << 7.125 << 7.0
  << 24.9520887446 << 0.0 << 18.926681924;
  QTest::newRow( "geometry 15" ) << "CURVEPOLYGONZM(CIRCULARSTRINGZM(1 3 1 11, 3 5 2 12, 4 7 3 13, 7 3 4 14, 1 3 1 11))"
  << "CURVEPOLYGONZM(CIRCULARSTRINGZM(1 3 1 11, 3 5 2 12, 4 7 3 13, 7 3 4 14, 1 3 1 11))"
  << 5 << 1 << 0
  << false
  << "Point (4.40851485241232677 3.56861296120545823)" << 1.0 << 0.75 << 7.125 << 7.0
  << 24.9520887446 << 0.0 << 18.926681924;
  QTest::newRow( "geometry 16" ) << "CURVEPOLYGON(CIRCULARSTRING(1 3 1, 3 5 2, 4 7 3, 7 3 4, 1 3 1))"
  << "CURVEPOLYGONZ(CIRCULARSTRINGZ(1 3 1, 3 5 2, 4 7 3, 7 3 4, 1 3 1))"
  << 5 << 1 << 0
  << false
  << "Point (4.40851485241232677 3.56861296120545823)" << 1.0 << 0.75 << 7.125 << 7.0
  << 24.9520887446 << 0.0 << 18.926681924;
  QTest::newRow( "geometry 17" ) << "CURVEPOLYGON(CIRCULARSTRING(1 3 1 11, 3 5 2 12, 4 7 3 13, 7 3 4 14, 1 3 1 11))"
  << "CURVEPOLYGONZM(CIRCULARSTRINGZM(1 3 1 11, 3 5 2 12, 4 7 3 13, 7 3 4 14, 1 3 1 11))"
  << 5 << 1 << 0
  << false
  << "Point (4.40851485241232677 3.56861296120545823)" << 1.0 << 0.75 << 7.125 << 7.0
  << 24.9520887446 << 0.0 << 18.926681924;
  QTest::newRow( "geometry 18" ) << "GeometryCollection (GeometryCollection (Point (1 1)))"
  << "GEOMETRYCOLLECTION(GEOMETRYCOLLECTION(POINT(1 1)))"
  << 1 << 1 << 0
  << false
  << "POINT(1 1)" << 1.0 << 1.0 << 1.0 << 1.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 19" ) << "GeometryCollection (LineString (0 0, 1 1, 2 2, 3 3 , 4 4))"
  << "GEOMETRYCOLLECTION(LINESTRING(0 0,1 1,2 2,3 3,4 4))"
  << 5 << 1 << 0
  << false
  << "POINT(2 2)" << 0.0 << 0.0 << 4.0 << 4.0
  << 0.0 << 5.6568542495 << 0.0;
  QTest::newRow( "geometry 20" ) << "GeometryCollection (LineStringZ (0 0 0, 1 1 0, 2 2 0, 3 3 0, 4 4 0),PointZ (1 2 3))"
  << "GEOMETRYCOLLECTION Z (LINESTRING Z (0 0 0,1 1 0,2 2 0,3 3 0,4 4 0),POINT Z (1 2 3))"
  << 6 << 2 << 0
  << false
  << "POINT(2 2)" << 0.0 << 0.0 << 4.0 << 4.0
  << 0.0 << 5.6568542495 << 0.0;
  QTest::newRow( "geometry 21" ) << "GeometryCollection (LineStringZ (1 2 3 , 4 5 6 , 7 8 9 , 10 11 12, 13 14 15))"
  << "GEOMETRYCOLLECTION Z (LINESTRING Z (1 2 3,4 5 6,7 8 9,10 11 12,13 14 15))"
  << 5 << 1 << 0
  << false
  << "POINT(7 8)" << 1.0 << 2.0 << 13.0 << 14.0
  << 0.0 << 16.9705627485 << 0.0;
  QTest::newRow( "geometry 22" ) << "GeometryCollection (MultiLineString ((0 0, 1 1, 2 2, 3 3 , 4 4)))"
  << "GEOMETRYCOLLECTION(MULTILINESTRING((0 0,1 1,2 2,3 3,4 4)))"
  << 5 << 1 << 0
  << false
  << "POINT(2 2)" << 0.0 << 0.0 << 4.0 << 4.0
  << 0.0 << 5.6568542495 << 0.0;
  QTest::newRow( "geometry 23" ) << "GeometryCollection (MultiLineStringM ((0 0 0, 1 1 0, 2 2 0, 3 3 0 , 4 4 0)),PointM (1 2 3))"
  << "GEOMETRYCOLLECTION M (MULTILINESTRING M ((0 0 0,1 1 0,2 2 0,3 3 0,4 4 0)),POINT M (1 2 3))"
  << 6 << 2 << 0
  << false
  << "POINT(2 2)" << 0.0 << 0.0 << 4.0 << 4.0
  << 0.0 << 5.6568542495 << 0.0;
  QTest::newRow( "geometry 24" ) << "GeometryCollection (MultiLineStringZ ((1 2 3 , 4 5 6 , 7 8 9 , 10 11 12, 13 14 15),(0 0 0, 1 1 0, 2 2 0, 3 3 0, 4 4 0),(0 0 0, 1 1 0, 2 2 0, 3 3 0, 4 4 0)))"
  << "GEOMETRYCOLLECTION Z (MULTILINESTRING Z ((1 2 3,4 5 6,7 8 9,10 11 12,13 14 15),(0 0 0,1 1 0,2 2 0,3 3 0,4 4 0),(0 0 0,1 1 0,2 2 0,3 3 0,4 4 0)))"
  << 15 << 1 << 0
  << false
  << "POINT(5 5.6)" << 0.0 << 0.0 << 13.0 << 14.0
  << 0.0 << 28.2842712475 << 0.0;
  QTest::newRow( "geometry 25" ) << "GeometryCollection (MultiPoint ((1 2)))"
  << "GEOMETRYCOLLECTION(MULTIPOINT((1 2)))"
  << 1 << 1 << 0
  << false
  << "POINT(1 2)" << 1.0 << 2.0 << 1.0 << 2.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 26" ) << "GeometryCollection (MultiPoint (1 2))"
  << "GEOMETRYCOLLECTION(MULTIPOINT((1 2)))"
  << 1 << 1 << 0
  << false
  << "POINT(1 2)" << 1.0 << 2.0 << 1.0 << 2.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 27" ) << "GeometryCollection (MultiPointM ((1 2 3),(5 6 7),(8 9 10),(11 12 13)))"
  << "GEOMETRYCOLLECTION M (MULTIPOINT M ((1 2 3),(5 6 7),(8 9 10),(11 12 13)))"
  << 4 << 1 << 0
  << false
  << "POINT(6.25 7.25)" << 1.0 << 2.0 << 11.0 << 12.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 28" ) << "GeometryCollection (MultiPointZ ((1 2 0),(3 4 0),(5 6 0)),PointZ (1 2 3))"
  << "GEOMETRYCOLLECTION Z (MULTIPOINT Z ((1 2 0),(3 4 0),(5 6 0)),POINT Z (1 2 3))"
  << 4 << 2 << 0
  << false
  << "POINT(2.5 3.5)" << 1.0 << 2.0 << 5.0 << 6.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 29" ) << "GeometryCollection (MultiPointZ ((1 2 3)))"
  << "GEOMETRYCOLLECTION Z (MULTIPOINT Z ((1 2 3)))"
  << 1 << 1 << 0
  << false
  << "POINT(1 2)" << 1.0 << 2.0 << 1.0 << 2.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 30" ) << "GeometryCollection (MultiPolygonZ (((0 0 0, 10 0 0, 10 10 0, 0 10 0, 0 0 0)),((0 0 0, 10 0 0, 10 10 0, 0 10 0, 0 0 0),(5 5 0, 7 5 0, 7 7 0, 5 7 0, 5 5 0)) ,((0 0 1, 10 0 1, 10 10 1, 0 10 1, 0 0 1),(5 5 1, 7 5 1, 7 7 1, 5 7 1, 5 5 1),(1 1 1,2 1 1, 2 2 1, 1 2 1, 1 1 1))),MultiLineStringZ ((0 0 0, 1 1 0, 2 2 0, 3 3 0, 4 4 0),(0 0 0, 1 1 0, 2 2 0, 3 3 0, 4 4 0),(1 2 3 , 4 5 6 , 7 8 9 , 10 11 12, 13 14 15)),MultiPointZ ((1 2 3),(5 6 7),(8 9 10),(11 12 13)))"
  << "GEOMETRYCOLLECTION Z (MULTIPOLYGON Z (((0 0 0,10 0 0,10 10 0,0 10 0,0 0 0)),((0 0 0,10 0 0,10 10 0,0 10 0,0 0 0),(5 5 0,7 5 0,7 7 0,5 7 0,5 5 0)),((0 0 1,10 0 1,10 10 1,0 10 1,0 0 1),(5 5 1,7 5 1,7 7 1,5 7 1,5 5 1),(1 1 1,2 1 1,2 2 1,1 2 1,1 1 1))),MULTILINESTRING Z ((0 0 0,1 1 0,2 2 0,3 3 0,4 4 0),(0 0 0,1 1 0,2 2 0,3 3 0,4 4 0),(1 2 3,4 5 6,7 8 9,10 11 12,13 14 15)),MULTIPOINT Z ((1 2 3),(5 6 7),(8 9 10),(11 12 13)))"
  << 49 << 3 << 0
  << false
  << "POINT(4.98453608247423 4.98453608247423)" << 0.0 << 0.0 << 13.0 << 14.0
  << 291.0 << 28.2842712475 << 140.0;
  QTest::newRow( "geometry 31" ) << "GeometryCollection (MultiPolygonZ (((0 0 0, 10 0 0, 10 10 0, 0 10 0, 0 0 0)),((0 0 0, 10 0 0, 10 10 0, 0 10 0, 0 0 0),(5 5 0, 7 5 0, 7 7 0, 5 7 0, 5 5 0)) ,((0 0 1, 10 0 1, 10 10 1, 0 10 1, 0 0 1),(5 5 1, 7 5 1, 7 7 1, 5 7 1, 5 5 1),(1 1 1,2 1 1, 2 2 1, 1 2 1, 1 1 1))))"
  << "GEOMETRYCOLLECTION Z (MULTIPOLYGON Z (((0 0 0,10 0 0,10 10 0,0 10 0,0 0 0)),((0 0 0,10 0 0,10 10 0,0 10 0,0 0 0),(5 5 0,7 5 0,7 7 0,5 7 0,5 5 0)),((0 0 1,10 0 1,10 10 1,0 10 1,0 0 1),(5 5 1,7 5 1,7 7 1,5 7 1,5 5 1),(1 1 1,2 1 1,2 2 1,1 2 1,1 1 1))))"
  << 30 << 1 << 0
  << false
  << "POINT(4.98453608247423 4.98453608247423)" << 0.0 << 0.0 << 10.0 << 10.0
  << 291.0 << 0.0 << 140.0;
  QTest::newRow( "geometry 32" ) << "GeometryCollection (Point (1 2),LineString (0 0, 1 1, 2 2, 3 3 , 4 4))"
  << "GEOMETRYCOLLECTION(POINT(1 2),LINESTRING(0 0,1 1,2 2,3 3,4 4))"
  << 6 << 2 << 0
  << false
  << "POINT(2 2)" << 0.0 << 0.0 << 4.0 << 4.0
  << 0.0 << 5.6568542495 << 0.0;
  QTest::newRow( "geometry 33" ) << "GeometryCollection (Point (1 2))"
  << "GEOMETRYCOLLECTION(POINT(1 2))"
  << 1 << 1 << 0
  << false
  << "POINT(1 2)" << 1.0 << 2.0 << 1.0 << 2.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 34" ) << "GeometryCollection (Point (10 10),Point (30 30),LineString (15 15, 20 20))"
  << "GEOMETRYCOLLECTION(POINT(10 10),POINT(30 30),LINESTRING(15 15,20 20))"
  << 4 << 3 << 0
  << false
  << "POINT(17.5 17.5)" << 10.0 << 10.0 << 30.0 << 30.0
  << 0.0 << 7.0710678119 << 0.0;
  QTest::newRow( "geometry 35" ) << "GeometryCollection (PointM (1 2 0),PointM (1 2 3),PolygonM ((0 0 1, 10 0 1, 10 10 1, 0 10 1, 0 0 1),(5 5 1, 7 5 1, 7 7 1, 5 7 1, 5 5 1),(1 1 1,2 1 1, 2 2 1, 1 2 1, 1 1 1)))"
  << "GEOMETRYCOLLECTION M (POINT M (1 2 0),POINT M (1 2 3),POLYGON M ((0 0 1,10 0 1,10 10 1,0 10 1,0 0 1),(5 5 1,7 5 1,7 7 1,5 7 1,5 5 1),(1 1 1,2 1 1,2 2 1,1 2 1,1 1 1)))"
  << 17 << 3 << 0
  << false
  << "POINT(4.99473684210526 4.99473684210526)" << 0.0 << 0.0 << 10.0 << 10.0
  << 95.0 << 0.0 << 52.0;
  QTest::newRow( "geometry 36" ) << "GeometryCollection (PointM (1 2 3), MultiPolygonM (((0 0 0, 10 0 0, 10 10 0, 0 10 0, 0 0 0)),((0 0 0, 10 0 0, 10 10 0, 0 10 0, 0 0 0),(5 5 0, 7 5 0, 7 7 0, 5 7 0, 5 5 0)) ,((0 0 1, 10 0 1, 10 10 1, 0 10 1, 0 0 1),(5 5 1, 7 5 1, 7 7 1, 5 7 1, 5 5 1),(1 1 1,2 1 1, 2 2 1, 1 2 1, 1 1 1))))"
  << "GEOMETRYCOLLECTION M (POINT M (1 2 3),MULTIPOLYGON M (((0 0 0,10 0 0,10 10 0,0 10 0,0 0 0)),((0 0 0,10 0 0,10 10 0,0 10 0,0 0 0),(5 5 0,7 5 0,7 7 0,5 7 0,5 5 0)),((0 0 1,10 0 1,10 10 1,0 10 1,0 0 1),(5 5 1,7 5 1,7 7 1,5 7 1,5 5 1),(1 1 1,2 1 1,2 2 1,1 2 1,1 1 1))))"
  << 31 << 2 << 0
  << false
  << "POINT(4.98453608247423 4.98453608247423)" << 0.0 << 0.0 << 10.0 << 10.0
  << 291.0 << 0.0 << 140.0;
  QTest::newRow( "geometry 37" ) << "GeometryCollection (PointZ (1 2 0),MultiPointZ (1 2 3))"
  << "GEOMETRYCOLLECTION Z (POINT Z (1 2 0),MULTIPOINT Z ((1 2 3)))"
  << 2 << 2 << 0
  << false
  << "POINT(1 2)" << 1.0 << 2.0 << 1.0 << 2.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 38" ) << "GeometryCollection (PointZ (1 2 0),PointZ (1 2 3),LineStringZ (1 2 3 , 4 5 6 , 7 8 9 , 10 11 12, 13 14 15),PolygonZ ((0 0 0, 10 0 0, 10 10 0, 0 10 0, 0 0 0)))"
  << "GEOMETRYCOLLECTIONZ (POINT Z (1 2 0),POINT Z (1 2 3),LINESTRING Z (1 2 3,4 5 6,7 8 9,10 11 12,13 14 15),POLYGON Z ((0 0 0,10 0 0,10 10 0,0 10 0,0 0 0)))"
  << 12 << 4 << 0
  << false
  << "POINT(5 5)" << 0.0 << 0.0 << 13.0 << 14.0
  << 100.0 << 16.9705627485 << 40.0;
  QTest::newRow( "geometry 39" ) << "GeometryCollection (PointZ (1 2 0),PointZ (1 2 3),LineStringZ (1 2 3 , 4 5 6 , 7 8 9 , 10 11 12, 13 14 15))"
  << "GEOMETRYCOLLECTION Z (POINT Z (1 2 0),POINT Z (1 2 3),LINESTRING Z (1 2 3,4 5 6,7 8 9,10 11 12,13 14 15))"
  << 7 << 3 << 0
  << false
  << "POINT(7 8)" << 1.0 << 2.0 << 13.0 << 14.0
  << 0.0 << 16.9705627485 << 0.0;
  QTest::newRow( "geometry 40" ) << "GeometryCollection (PointZ (1 2 0),PointZ (1 2 3))"
  << "GEOMETRYCOLLECTION Z (POINT Z (1 2 0),POINT Z (1 2 3))"
  << 2 << 2 << 0
  << false
  << "POINT(1 2)" << 1.0 << 2.0 << 1.0 << 2.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 41" ) << "GeometryCollection (PointZ (1 2 3),MultiLineStringZ ((0 0 0, 1 1 0, 2 2 0, 3 3 0 , 4 4 0)))"
  << "GEOMETRYCOLLECTION Z (POINT Z (1 2 3),MULTILINESTRING Z ((0 0 0,1 1 0,2 2 0,3 3 0,4 4 0)))"
  << 6 << 2 << 0
  << false
  << "POINT(2 2)" << 0.0 << 0.0 << 4.0 << 4.0
  << 0.0 << 5.6568542495 << 0.0;
  QTest::newRow( "geometry 42" ) << "GeometryCollection (PointZ (1 2 3))"
  << "GEOMETRYCOLLECTION Z (POINT Z (1 2 3))"
  << 1 << 1 << 0
  << false
  << "POINT(1 2)" << 1.0 << 2.0 << 1.0 << 2.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 43" ) << "GeometryCollection (PolygonZ ((0 0 1, 10 0 1, 10 10 1, 0 10 1, 0 0 1),(5 5 1, 7 5 1, 7 7 1 , 5 7 1, 5 5 1)))"
  << "GEOMETRYCOLLECTION Z (POLYGON Z ((0 0 1,10 0 1,10 10 1,0 10 1,0 0 1),(5 5 1,7 5 1,7 7 1,5 7 1,5 5 1)))"
  << 10 << 1 << 0
  << false
  << "POINT(4.95833333333333 4.95833333333333)" << 0.0 << 0.0 << 10.0 << 10.0
  << 96.0 << 0.0 << 48.0;
  QTest::newRow( "geometry 44" ) << "LineString (0 0, 1 1, 2 2, 3 3 , 4 4)"
  << "LINESTRING(0 0,1 1,2 2,3 3,4 4)"
  << 5 << 1 << 0
  << false
  << "POINT(2 2)" << 0.0 << 0.0 << 4.0 << 4.0
  << 0.0 << 5.6568542495 << 0.0;
  QTest::newRow( "geometry 45" ) << "LineString (0 0, 10 10, 20 0)"
  << "LINESTRING(0 0,10 10,20 0)"
  << 3 << 1 << 0
  << false
  << "POINT(10 5)" << 0.0 << 0.0 << 20.0 << 10.0
  << 0.0 << 28.2842712475 << 0.0;
  QTest::newRow( "geometry 46" ) << "LineString (0 0,3 4,4 3)"
  << "LINESTRING(0 0,3 4,4 3)"
  << 3 << 1 << 0
  << false
  << "POINT(1.94096241842308 2.33072181381731)" << 0.0 << 0.0 << 4.0 << 4.0
  << 0.0 << 6.4142135624 << 0.0;
  QTest::newRow( "geometry 47" ) << "LineString (0 0,3 4)"
  << "LINESTRING(0 0,3 4)"
  << 2 << 1 << 0
  << false
  << "POINT(1.5 2)" << 0.0 << 0.0 << 3.0 << 4.0
  << 0.0 << 5.0 << 0.0;
  QTest::newRow( "geometry 48" ) << "LineString M (1 2 3 , 4 5 6 , 7 8 9 , 10 11 12, 13 14 15)"
  << "LINESTRING M (1 2 3,4 5 6,7 8 9,10 11 12,13 14 15)"
  << 5 << 1 << 0
  << false
  << "POINT(7 8)" << 1.0 << 2.0 << 13.0 << 14.0
  << 0.0 << 16.9705627485 << 0.0;
  QTest::newRow( "geometry 49" ) << "LineString( 1 2 3, 4 5 6 )"
  << "LineStringZ( 1 2 3, 4 5 6 )"
  << 2 << 1 << 0
  << false
  << "Point (2.5 3.5)" << 1.0 << 2.0 << 4.0 << 5.0
  << 0.0 << 4.2426406871 << 0.0;
  QTest::newRow( "geometry 50" ) << "LineString( 1 2 3 4, 5 6 7 8 )"
  << "LineStringZM( 1 2 3 4, 5 6 7 8)"
  << 2 << 0 << 0
  << false
  << "POINT( 3 4 )" << 1.0 << 2.0 << 5.0 << 6.0
  << 0.0 << 5.6568542495 << 0.0;
  QTest::newRow( "geometry 51" ) << "LineString(1 2,2 3,3 4)"
  << "LINESTRING(1 2,2 3,3 4)"
  << 3 << 1 << 0
  << false
  << "POINT(2 3)" << 1.0 << 2.0 << 3.0 << 4.0
  << 0.0 << 2.8284271247 << 0.0;
  QTest::newRow( "geometry 52" ) << "LineStringZ (0 0 0 , 1 1 1 , 2 2 2 , 3 3 3, 4 4 4)"
  << "LINESTRING Z (0 0 0,1 1 1,2 2 2,3 3 3,4 4 4)"
  << 5 << 1 << 0
  << false
  << "POINT(2 2)" << 0.0 << 0.0 << 4.0 << 4.0
  << 0.0 << 5.6568542495 << 0.0;
  QTest::newRow( "geometry 53" ) << "LineStringZM (0 0 0 2 , 1 1 1 3 , 2 2 2  4, 3 3 3 5, 4 4 4 6)"
  << "LineStringZM (0 0 0 2, 1 1 1 3, 2 2 2 4, 3 3 3 5, 4 4 4 6)"
  << 5 << 1 << 0
  << false
  << "POINT(2 2)" << 0.0 << 0.0 << 4.0 << 4.0
  << 0.0 << 5.6568542495 << 0.0;
  QTest::newRow( "geometry 54" ) << "LineString( 0 2, 3 2, 3 4, 0 4, 0 2 )"
  << "LineString( 0 2, 3 2, 3 4, 0 4, 0 2 )"
  << 5 << 1 << 0
  << true
  << "POINT(1.5 3)" << 0.0 << 2.0 << 3.0 << 4.0
  << 0.0 << 10.0 << 0.0;
  QTest::newRow( "geometry 55" ) << "LineStringZ( 0 2 1, 3 2 2, 3 4 5, 0 4 6, 0 2 1 )"
  << "LineStringZ( 0 2 1, 3 2 2, 3 4 5, 0 4 6, 0 2 1 )"
  << 5 << 1 << 0
  << true
  << "POINT(1.5 3)" << 0.0 << 2.0 << 3.0 << 4.0
  << 0.0 << 10.0 << 0.0;
  QTest::newRow( "geometry 56" ) << "LineStringZ( 0 2 1, 3 2 2, 3 4 5, 0 4 6, 0 2 2 )"
  << "LineStringZ( 0 2 1, 3 2 2, 3 4 5, 0 4 6, 0 2 2 )"
  << 5 << 1 << 0
  << false
  << "POINT(1.5 3)" << 0.0 << 2.0 << 3.0 << 4.0
  << 0.0 << 10.0 << 0.0;
  QTest::newRow( "geometry 57" ) << "LineStringM( 0 2 1, 3 2 2, 3 4 5, 0 4 6, 0 2 20 )"
  << "LineStringM( 0 2 1, 3 2 2, 3 4 5, 0 4 6, 0 2 20 )"
  << 5 << 1 << 0
  << true
  << "POINT(1.5 3)" << 0.0 << 2.0 << 3.0 << 4.0
  << 0.0 << 10.0 << 0.0;
  QTest::newRow( "geometry 58" ) << "MultiLineString ((0 0, 1 0, 1 1, 2 1, 2 0), (3 1, 5 1, 5 0, 6 0))"
  << "MULTILINESTRING((0 0,1 0,1 1,2 1,2 0),(3 1,5 1,5 0,6 0))"
  << 9 << 2 << 0
  << false
  << "POINT(2.9375 0.5625)" << 0.0 << 0.0 << 6.0 << 1.0
  << 0.0 << 8.0 << 0.0;
  QTest::newRow( "geometry 59" ) << "MultiLineString ((0 0, 1 1, 2 2, 3 3 , 4 4),(0 0, 1 1, 2 2, 3 3 , 4 4))"
  << "MULTILINESTRING((0 0,1 1,2 2,3 3,4 4),(0 0,1 1,2 2,3 3,4 4))"
  << 10 << 2 << 0
  << false
  << "POINT(2 2)" << 0.0 << 0.0 << 4.0 << 4.0
  << 0.0 << 11.313708499 << 0.0;
  QTest::newRow( "geometry 60" ) << "MultiLineString ((0 0, 1 1, 2 2, 3 3 , 4 4))"
  << "MULTILINESTRING((0 0,1 1,2 2,3 3,4 4))"
  << 5 << 1 << 0
  << false
  << "POINT(2 2)" << 0.0 << 0.0 << 4.0 << 4.0
  << 0.0 << 5.6568542495 << 0.0;
  QTest::newRow( "geometry 61" ) << "MultiLineString((0 0, 1 1),(0 0, 1 1, 2 2) )"
  << "MULTILINESTRING((0 0,1 1),(0 0,1 1,2 2))"
  << 5 << 2 << 0
  << false
  << "POINT(0.833333333333333 0.833333333333333)" << 0.0 << 0.0 << 2.0 << 2.0
  << 0.0 << 4.2426406871 << 0.0;
  QTest::newRow( "geometry 62" ) << "MultiLineStringM ((1 2 3 , 4 5 6 , 7 8 9 , 10 11 12, 13 14 15),(0 0 0, 1 1 0, 2 2 0, 3 3 0, 4 4 0),(0 0 0, 1 1 0, 2 2 0, 3 3 0 , 4 4 0))"
  << "MULTILINESTRING M ((1 2 3,4 5 6,7 8 9,10 11 12,13 14 15),(0 0 0,1 1 0,2 2 0,3 3 0,4 4 0),(0 0 0,1 1 0,2 2 0,3 3 0,4 4 0))"
  << 15 << 3 << 0
  << false
  << "POINT(5 5.6)" << 0.0 << 0.0 << 13.0 << 14.0
  << 0.0 << 28.2842712475 << 0.0;
  QTest::newRow( "geometry 63" ) << "MultiLineStringZ ((0 0 0, 1 1 0, 2 2 0, 3 3 0, 4 4 0),(0 0 0, 1 1 0, 2 2 0, 3 3 0, 4 4 0),(1 2 3 , 4 5 6 , 7 8 9 , 10 11 12, 13 14 15))"
  << "MULTILINESTRING Z ((0 0 0,1 1 0,2 2 0,3 3 0,4 4 0),(0 0 0,1 1 0,2 2 0,3 3 0,4 4 0),(1 2 3,4 5 6,7 8 9,10 11 12,13 14 15))"
  << 15 << 3 << 0
  << false
  << "POINT(5 5.6)" << 0.0 << 0.0 << 13.0 << 14.0
  << 0.0 << 28.2842712475 << 0.0;
  QTest::newRow( "geometry 64" ) << "MultiLineStringZM ((0 0 0 1, 1 1 0 2, 2 2 0 3, 3 3 0 4, 4 4 0 5),(0 0 0 6, 1 1 0 7, 2 2 0 8, 3 3 0 9, 4 4 0 10),(1 2 3 11, 4 5 6 12, 7 8 9 13, 10 11 12 14, 13 14 15 15))"
  << "MultiLineStringZM ((0 0 0 1, 1 1 0 2, 2 2 0 3, 3 3 0 4, 4 4 0 5),(0 0 0 6, 1 1 0 7, 2 2 0 8, 3 3 0 9, 4 4 0 10),(1 2 3 11, 4 5 6 12, 7 8 9 13, 10 11 12 14, 13 14 15 15))"
  << 15 << 3 << 0
  << false
  << "POINT(5 5.6)" << 0.0 << 0.0 << 13.0 << 14.0
  << 0.0 << 28.2842712475 << 0.0;
  QTest::newRow( "geometry 65" ) << "MultiLineString ((0 0 0, 1 1 0, 2 2 0, 3 3 0, 4 4 0),(0 0 0, 1 1 0, 2 2 0, 3 3 0, 4 4 0),(1 2 3 , 4 5 6 , 7 8 9 , 10 11 12, 13 14 15))"
  << "MULTILINESTRING Z ((0 0 0,1 1 0,2 2 0,3 3 0,4 4 0),(0 0 0,1 1 0,2 2 0,3 3 0,4 4 0),(1 2 3,4 5 6,7 8 9,10 11 12,13 14 15))"
  << 15 << 3 << 0
  << false
  << "POINT(5 5.6)" << 0.0 << 0.0 << 13.0 << 14.0
  << 0.0 << 28.2842712475 << 0.0;
  QTest::newRow( "geometry 66" ) << "MultiLineString ((0 0 0 1, 1 1 0 2, 2 2 0 3, 3 3 0 4, 4 4 0 5),(0 0 0 6, 1 1 0 7, 2 2 0 8, 3 3 0 9, 4 4 0 10),(1 2 3 11, 4 5 6 12, 7 8 9 13, 10 11 12 14, 13 14 15 15))"
  << "MultiLineStringZM ((0 0 0 1, 1 1 0 2, 2 2 0 3, 3 3 0 4, 4 4 0 5),(0 0 0 6, 1 1 0 7, 2 2 0 8, 3 3 0 9, 4 4 0 10),(1 2 3 11, 4 5 6 12, 7 8 9 13, 10 11 12 14, 13 14 15 15))"
  << 15 << 3 << 0
  << false
  << "POINT(5 5.6)" << 0.0 << 0.0 << 13.0 << 14.0
  << 0.0 << 28.2842712475 << 0.0;
  QTest::newRow( "geometry 67" ) << "MultiPoint ((1 2),(2 3))"
  << "MULTIPOINT((1 2),(2 3))"
  << 2 << 2 << 0
  << false
  << "POINT(1.5 2.5)" << 1.0 << 2.0 << 2.0 << 3.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 68" ) << "MultiPoint ((10 30),(40 20),(30 10),(20 10))"
  << "MULTIPOINT((10 30),(40 20),(30 10),(20 10))"
  << 4 << 4 << 0
  << false
  << "POINT(25 17.5)" << 10.0 << 10.0 << 40.0 << 30.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 69" ) << "MultiPointZ(( 1 2 3 ),(4 5 6))"
  << "MultiPointZ(( 1 2 3 ),(4 5 6))"
  << 2 << 2 << 0
  << false
  << "POINT(2.5 3.5)" << 1.0 << 2.0 << 4.0 << 5.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 70" ) << "MultiPointM(( 1 2 3 ),(4 5 6))"
  << "MultiPointM(( 1 2 3 ),(4 5 6))"
  << 2 << 2 << 0
  << false
  << "POINT(2.5 3.5)" << 1.0 << 2.0 << 4.0 << 5.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 71" ) << "MultiPointZM(( 1 2 3 11),(4 5 6 12))"
  << "MultiPointZM(( 1 2 3 11),(4 5 6 12))"
  << 2 << 2 << 0
  << false
  << "POINT(2.5 3.5)" << 1.0 << 2.0 << 4.0 << 5.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 72" ) << "MultiPoint(( 1 2 3 ),(4 5 6))"
  << "MultiPointZ(( 1 2 3 ),(4 5 6))"
  << 2 << 2 << 0
  << false
  << "POINT(2.5 3.5)" << 1.0 << 2.0 << 4.0 << 5.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 73" ) << "MultiPoint(( 1 2 3 11),(4 5 6 12))"
  << "MultiPointZM(( 1 2 3 11),(4 5 6 12))"
  << 2 << 2 << 0
  << false
  << "POINT(2.5 3.5)" << 1.0 << 2.0 << 4.0 << 5.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 74" ) << "MultiPoint (1 2, 3 4, 5 6)"
  << "MULTIPOINT((1 2),(3 4),(5 6))"
  << 3 << 3 << 0
  << false
  << "POINT(3 4)" << 1.0 << 2.0 << 5.0 << 6.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 75" ) << "MultiPoint (1 2)"
  << "MULTIPOINT((1 2))"
  << 1 << 1 << 0
  << false
  << "POINT(1 2)" << 1.0 << 2.0 << 1.0 << 2.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 76" ) << "MultiPoint Z (1 2 3)"
  << "MULTIPOINT Z ((1 2 3))"
  << 1 << 1 << 0
  << false
  << "POINT(1 2)" << 1.0 << 2.0 << 1.0 << 2.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 77" ) << "MultiPointM (1 2 3,4 5 0)"
  << "MULTIPOINT M ((1 2 3),(4 5 0))"
  << 2 << 2 << 0
  << false
  << "POINT(2.5 3.5)" << 1.0 << 2.0 << 4.0 << 5.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 78" ) << "MultiPointZ ((-1 -2 -3),(5.4 6.6 7.77),(-5.4 -6.6 -7.77),(1000000 0.000001 -1000000),(-0.0000013 -0.000014 0))"
  << "MULTIPOINT Z ((-1 -2 -3),(5.4 6.6 7.77),(-5.4 -6.6 -7.77),(1000000 0.000001 -1000000),(-0.0000013 -0.000014 0))"
  << 5 << 5 << 0
  << false
  << "POINT(199999.79999974 -0.4000026)" << -5.4 << -6.6 << 1000000.0 << 6.6
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 79" ) << "MultiPointZ (1 2 0, 1 2 3, 4 5 0, 6 7 8)"
  << "MULTIPOINT Z ((1 2 0),(1 2 3),(4 5 0),(6 7 8))"
  << 4 << 4 << 0
  << false
  << "POINT(3 4)" << 1.0 << 2.0 << 6.0 << 7.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 80" ) << "MultiPointZ (1 2 3, 5 6 7, 8 9 10, 11 12 13)"
  << "MULTIPOINT Z ((1 2 3),(5 6 7),(8 9 10),(11 12 13))"
  << 4 << 4 << 0
  << false
  << "POINT(6.25 7.25)" << 1.0 << 2.0 << 11.0 << 12.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 81" ) << "MultiPointZ (1 2 3)"
  << "MULTIPOINT Z ((1 2 3))"
  << 1 << 1 << 0
  << false
  << "POINT(1 2)" << 1.0 << 2.0 << 1.0 << 2.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 82" ) << "MultiPolygon (((0 0, 1 0, 1 1, 2 1, 2 2, 0 2, 0 0)),((4 0, 5 0, 5 2, 3 2, 3 1, 4 1, 4 0)))"
  << "MULTIPOLYGON(((0 0,1 0,1 1,2 1,2 2,0 2,0 0)),((4 0,5 0,5 2,3 2,3 1,4 1,4 0)))"
  << 14 << 2 << 0
  << false
  << "POINT(2.5 1.16666666666667)" << 0.0 << 0.0 << 5.0 << 2.0
  << 6.0 << 0.0 << 16.0;
  QTest::newRow( "geometry 83" ) << "MultiPolygon (((0 0, 10 0, 10 10, 0 10, 0 0)),((0 0, 10 0, 10 10, 0 10, 0 0),(5 5, 7 5, 7 7 , 5 7, 5 5)))"
  << "MULTIPOLYGON(((0 0,10 0,10 10,0 10,0 0)),((0 0,10 0,10 10,0 10,0 0),(5 5,7 5,7 7,5 7,5 5)))"
  << 15 << 2 << 0
  << false
  << "POINT(4.97959183673469 4.97959183673469)" << 0.0 << 0.0 << 10.0 << 10.0
  << 196.0 << 0.0 << 88.0;
  QTest::newRow( "geometry 84" ) << "MultiPolygon (((0 0, 10 0, 10 10, 0 10, 0 0)))"
  << "MULTIPOLYGON(((0 0,10 0,10 10,0 10,0 0)))"
  << 5 << 1 << 0
  << false
  << "POINT(5 5)" << 0.0 << 0.0 << 10.0 << 10.0
  << 100.0 << 0.0 << 40.0;
  QTest::newRow( "geometry 85" ) << "MultiPolygon( ((0 0, 10 0, 10 10, 0 10, 0 0)),( (0 0, 10 0, 10 10, 0 10, 0 0),(5 5, 7 5, 7 7 , 5 7, 5 5) ) ,( (0 0, 10 0, 10 10, 0 10, 0 0),(5 5, 7 5, 7 7, 5 7, 5 5),(1 1,2 1, 2 2, 1 2, 1 1) ) )"
  << "MULTIPOLYGON(((0 0,10 0,10 10,0 10,0 0)),((0 0,10 0,10 10,0 10,0 0),(5 5,7 5,7 7,5 7,5 5)),((0 0,10 0,10 10,0 10,0 0),(5 5,7 5,7 7,5 7,5 5),(1 1,2 1,2 2,1 2,1 1)))"
  << 30 << 3 << 0
  << false
  << "POINT(4.98453608247423 4.98453608247423)" << 0.0 << 0.0 << 10.0 << 10.0
  << 291.0 << 0.0 << 140.0;
  QTest::newRow( "geometry 86" ) << "MultiPolygonZ (((0 0 0, 10 0 0, 10 10 0, 0 10 0, 0 0 0)),((0 0 0, 10 0 0, 10 10 0, 0 10 0, 0 0 0),(5 5 0, 7 5 0, 7 7 0, 5 7 0, 5 5 0)) ,((0 0 1, 10 0 1, 10 10 1, 0 10 1, 0 0 1),(5 5 1, 7 5 1, 7 7 1, 5 7 1, 5 5 1),(1 1 1,2 1 1, 2 2 1, 1 2 1, 1 1 1)))"
  << "MULTIPOLYGON Z (((0 0 0,10 0 0,10 10 0,0 10 0,0 0 0)),((0 0 0,10 0 0,10 10 0,0 10 0,0 0 0),(5 5 0,7 5 0,7 7 0,5 7 0,5 5 0)),((0 0 1,10 0 1,10 10 1,0 10 1,0 0 1),(5 5 1,7 5 1,7 7 1,5 7 1,5 5 1),(1 1 1,2 1 1,2 2 1,1 2 1,1 1 1)))"
  << 30 << 3 << 0
  << false
  << "POINT(4.98453608247423 4.98453608247423)" << 0.0 << 0.0 << 10.0 << 10.0
  << 291.0 << 0.0 << 140.0;
  QTest::newRow( "geometry 87" ) << "MultiPolygonM (((0 0 1, 10 0 2, 10 10 3, 0 10 4, 0 0 1)),((0 0 5, 10 0 6, 10 10 7, 0 10 8, 0 0 5),(5 5 0, 7 5 0, 7 7 0, 5 7 0, 5 5 0)) ,((0 0 1, 10 0 1, 10 10 1, 0 10 1, 0 0 1),(5 5 1, 7 5 1, 7 7 1, 5 7 1, 5 5 1),(1 1 1,2 1 1, 2 2 1, 1 2 1, 1 1 1)))"
  << "MultiPolygonM (((0 0 1, 10 0 2, 10 10 3, 0 10 4, 0 0 1)),((0 0 5, 10 0 6, 10 10 7, 0 10 8, 0 0 5),(5 5 0, 7 5 0, 7 7 0, 5 7 0, 5 5 0)) ,((0 0 1, 10 0 1, 10 10 1, 0 10 1, 0 0 1),(5 5 1, 7 5 1, 7 7 1, 5 7 1, 5 5 1),(1 1 1,2 1 1, 2 2 1, 1 2 1, 1 1 1)))"
  << 30 << 3 << 0
  << false
  << "POINT(4.98453608247423 4.98453608247423)" << 0.0 << 0.0 << 10.0 << 10.0
  << 291.0 << 0.0 << 140.0;
  QTest::newRow( "geometry 88" ) << "MultiPolygonZM (((0 0 1 2, 10 0 2 3, 10 10 3 4, 0 10 4 5, 0 0 1 2)),((0 0 5 7, 10 0 6 8, 10 10 7 9, 0 10 8 10, 0 0 5 7),(5 5 0 1, 7 5 0 2, 7 7 0 3, 5 7 0 4, 5 5 0 1)) ,((0 0 1 1, 10 0 1 3, 10 10 1 4, 0 10 1 3, 0 0 1 1),(5 5 1 1, 7 5 1 2, 7 7 1 3, 5 7 1 5, 5 5 1 1),(1 1 1 0,2 1 1 0, 2 2 1 0, 1 2 1 0, 1 1 1 0)))"
  << "MultiPolygonZM (((0 0 1 2, 10 0 2 3, 10 10 3 4, 0 10 4 5, 0 0 1 2)),((0 0 5 7, 10 0 6 8, 10 10 7 9, 0 10 8 10, 0 0 5 7),(5 5 0 1, 7 5 0 2, 7 7 0 3, 5 7 0 4, 5 5 0 1)) ,((0 0 1 1, 10 0 1 3, 10 10 1 4, 0 10 1 3, 0 0 1 1),(5 5 1 1, 7 5 1 2, 7 7 1 3, 5 7 1 5, 5 5 1 1),(1 1 1 0,2 1 1 0, 2 2 1 0, 1 2 1 0, 1 1 1 0)))"
  << 30 << 3 << 0
  << false
  << "POINT(4.98453608247423 4.98453608247423)" << 0.0 << 0.0 << 10.0 << 10.0
  << 291.0 << 0.0 << 140.0;
  QTest::newRow( "geometry 89" ) << "MultiPolygon (((0 0 0, 10 0 0, 10 10 0, 0 10 0, 0 0 0)),((0 0 0, 10 0 0, 10 10 0, 0 10 0, 0 0 0),(5 5 0, 7 5 0, 7 7 0, 5 7 0, 5 5 0)) ,((0 0 1, 10 0 1, 10 10 1, 0 10 1, 0 0 1),(5 5 1, 7 5 1, 7 7 1, 5 7 1, 5 5 1),(1 1 1,2 1 1, 2 2 1, 1 2 1, 1 1 1)))"
  << "MULTIPOLYGON Z (((0 0 0,10 0 0,10 10 0,0 10 0,0 0 0)),((0 0 0,10 0 0,10 10 0,0 10 0,0 0 0),(5 5 0,7 5 0,7 7 0,5 7 0,5 5 0)),((0 0 1,10 0 1,10 10 1,0 10 1,0 0 1),(5 5 1,7 5 1,7 7 1,5 7 1,5 5 1),(1 1 1,2 1 1,2 2 1,1 2 1,1 1 1)))"
  << 30 << 3 << 0
  << false
  << "POINT(4.98453608247423 4.98453608247423)" << 0.0 << 0.0 << 10.0 << 10.0
  << 291.0 << 0.0 << 140.0;
  QTest::newRow( "geometry 90" ) << "MultiPolygon (((0 0 1 2, 10 0 2 3, 10 10 3 4, 0 10 4 5, 0 0 1 2)),((0 0 5 7, 10 0 6 8, 10 10 7 9, 0 10 8 10, 0 0 5 7),(5 5 0 1, 7 5 0 2, 7 7 0 3, 5 7 0 4, 5 5 0 1)) ,((0 0 1 1, 10 0 1 3, 10 10 1 4, 0 10 1 3, 0 0 1 1),(5 5 1 1, 7 5 1 2, 7 7 1 3, 5 7 1 5, 5 5 1 1),(1 1 1 0,2 1 1 0, 2 2 1 0, 1 2 1 0, 1 1 1 0)))"
  << "MultiPolygonZM (((0 0 1 2, 10 0 2 3, 10 10 3 4, 0 10 4 5, 0 0 1 2)),((0 0 5 7, 10 0 6 8, 10 10 7 9, 0 10 8 10, 0 0 5 7),(5 5 0 1, 7 5 0 2, 7 7 0 3, 5 7 0 4, 5 5 0 1)) ,((0 0 1 1, 10 0 1 3, 10 10 1 4, 0 10 1 3, 0 0 1 1),(5 5 1 1, 7 5 1 2, 7 7 1 3, 5 7 1 5, 5 5 1 1),(1 1 1 0,2 1 1 0, 2 2 1 0, 1 2 1 0, 1 1 1 0)))"
  << 30 << 3 << 0
  << false
  << "POINT(4.98453608247423 4.98453608247423)" << 0.0 << 0.0 << 10.0 << 10.0
  << 291.0 << 0.0 << 140.0;
  QTest::newRow( "geometry 91" ) << "Point (1 2)"
  << "POINT(1 2)"
  << 1 << 1 << 0
  << false
  << "POINT(1 2)" << 1.0 << 2.0 << 1.0 << 2.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 92" ) << "Point( 1 2 3 )"
  << "PointZ( 1 2 3)"
  << 1 << 1 << 0
  << false
  << "POINT(1 2)" << 1.0 << 2.0 << 1.0 << 2.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 93" ) << "Point( 1 2 3 4 )"
  << "PointZM( 1 2 3 4 )"
  << 1 << 1 << 0
  << false
  << "Point( 1 2 )" << 1.0 << 2.0 << 1.0 << 2.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 94" ) << "PointM( 4 5 6 )"
  << "PointM ( 4 5 6 )"
  << 1 << 1 << 0
  << false
  << "POINT(4 5)" << 4.0 << 5.0 << 4.0 << 5.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 95" ) << "PointZ (1 2 3)"
  << "POINT Z (1 2 3)"
  << 1 << 1 << 0
  << false
  << "POINT(1 2)" << 1.0 << 2.0 << 1.0 << 2.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 96" ) << "PointZM( 7 8 9 10 )"
  << "PointZM( 7 8 9 10 )"
  << 1 << 1 << 0
  << false
  << "POINT( 7 8 )" << 7.0 << 8.0 << 7.0 << 8.0
  << 0.0 << 0.0 << 0.0;
  QTest::newRow( "geometry 97" ) << "Polygon ((0 0, 10 0, 10 10, 0 10, 0 0),(5 5, 7 5, 7 7 , 5 7, 5 5),(1 1,2 1, 2 2, 1 2, 1 1))"
  << "POLYGON((0 0,10 0,10 10,0 10,0 0),(5 5,7 5,7 7,5 7,5 5),(1 1,2 1,2 2,1 2,1 1))"
  << 15 << 1 << 2
  << false
  << "POINT(4.99473684210526 4.99473684210526)" << 0.0 << 0.0 << 10.0 << 10.0
  << 95.0 << 0.0 << 52.0;
  QTest::newRow( "geometry 98" ) << "Polygon ((0 0, 10 0, 10 10, 0 10, 0 0),(5 5, 7 5, 7 7 , 5 7, 5 5))"
  << "POLYGON((0 0,10 0,10 10,0 10,0 0),(5 5,7 5,7 7,5 7,5 5))"
  << 10 << 1 << 1
  << false
  << "POINT(4.95833333333333 4.95833333333333)" << 0.0 << 0.0 << 10.0 << 10.0
  << 96.0 << 0.0 << 48.0;
  QTest::newRow( "geometry 99" ) << "Polygon ((0 0, 10 0, 10 10, 0 10, 0 0))"
  << "POLYGON((0 0,10 0,10 10,0 10,0 0))"
  << 5 << 1 << 0
  << false
  << "POINT(5 5)" << 0.0 << 0.0 << 10.0 << 10.0
  << 100.0 << 0.0 << 40.0;
  QTest::newRow( "geometry 100" ) << "Polygon ((0 0,0 7,4 2,2 0,0 0), (1 1,2 1,2 2,1 2,1 1))"
  << "POLYGON((0 0,0 7,4 2,2 0,0 0),(1 1,2 1,2 2,1 2,1 1))"
  << 10 << 1 << 1
  << false
  << "POINT(1.41111111111111 2.78888888888889)" << 0.0 << 0.0 << 4.0 << 7.0
  << 15.0 << 0.0 << 22.2315513622;
  QTest::newRow( "geometry 101" ) << "Polygon ((0 0,0 7,4 2,2 0,0 0))"
  << "POLYGON((0 0,0 7,4 2,2 0,0 0))"
  << 5 << 1 << 0
  << false
  << "POINT(1.41666666666667 2.70833333333333)" << 0.0 << 0.0 << 4.0 << 7.0
  << 16.0 << 0.0 << 18.2315513622;
  QTest::newRow( "geometry 102" ) << "Polygon ((1 1,2 2,3 1,2 0,1 1))"
  << "POLYGON((1 1,2 2,3 1,2 0,1 1))"
  << 5 << 1 << 0
  << false
  << "POINT(2 1)" << 1.0 << 0.0 << 3.0 << 2.0
  << 2.0 << 0.0 << 5.6568542495;
  QTest::newRow( "geometry 103" ) << "Polygon ((1872000 528000,1872000 192000,1536119 192000,1536000 528000,1200000 528000,1200000 863880,1536000 863880,1872000 863880,1872000 528000))"
  << "POLYGON((1872000 528000,1872000 192000,1536119 192000,1536000 528000,1200000 528000,1200000 863880,1536000 863880,1872000 863880,1872000 528000))"
  << 9 << 1 << 0
  << false
  << "POINT(1592016.64149202 583949.860499226)" << 1200000.0 << 192000.0 << 1872000.0 << 863880.0
  << 3.38587368e+11 << 0.0 << 2687641.02107;
  QTest::newRow( "geometry 104" ) << "Polygon ((60 180, 140 120, 100 180, 140 240, 140 240, 60 180))"
  << "POLYGON((60 180,140 120,100 180,140 240,140 240,60 180))"
  << 6 << 1 << 0
  << false
  << "POINT(100 180)" << 60.0 << 120.0 << 140.0 << 240.0
  << 2400.0 << 0.0 << 344.222051019;
  QTest::newRow( "geometry 105" ) << "Polygon ((60 180, 140 120, 100 180, 140 240, 60 180))"
  << "POLYGON((60 180,140 120,100 180,140 240,60 180))"
  << 5 << 1 << 0
  << false
  << "POINT(100 180)" << 60.0 << 120.0 << 140.0 << 240.0
  << 2400.0 << 0.0 << 344.222051019;
  QTest::newRow( "geometry 106" ) << "Polygon ((60 180, 140 240, 140 240, 140 240, 200 180, 120 120, 60 180))"
  << "POLYGON((60 180,140 240,140 240,140 240,200 180,120 120,60 180))"
  << 7 << 1 << 0
  << false
  << "POINT(130 180)" << 60.0 << 120.0 << 200.0 << 240.0
  << 8400.0 << 0.0 << 369.705627485;
  QTest::newRow( "geometry 107" ) << "Polygon(( 0 2 1, 3 2 2, 3 4 5, 0 4 6, 0 2 1),(0.5 2.5 8, 0.6 2.7 9, 0.5 2.7 10, 0.5 2.5 8))"
  << "PolygonZ(( 0 2 1, 3 2 2, 3 4 5, 0 4 6, 0 2 1),(0.5 2.5 8, 0.6 2.7 9, 0.5 2.7 10, 0.5 2.5 8))"
  << 9 << 1 << 1
  << false
  << "POINT(1.50161380077908 3.00061213132999)" << 0.0 << 2.0 << 3.0 << 4.0
  << 5.99 << 0.0 << 10.5236067978;
  QTest::newRow( "geometry 108" ) << "Polygon(( 0 2 1, 3 2 2, 3 4 5, 0 4 6, 0 2 1))"
  << "PolygonZ(( 0 2 1, 3 2 2, 3 4 5, 0 4 6, 0 2 1))"
  << 5 << 1 << 0
  << false
  << "Point (1.5 3)" << 0.0 << 2.0 << 3.0 << 4.0
  << 6.0 << 0.0 << 10.0;
  QTest::newRow( "geometry 109" ) << "Polygon((0 2 1 5, 3 2 2 6, 3 4 5 7, 0 4 6 8, 0 2 1 5),(0.5 2.5 8 10, 1.0 2.5 9 11, 0.5 2.5 10 12, 0.5 2.5 8 10))"
  << "PolygonZM((0 2 1 5, 3 2 2 6, 3 4 5 7, 0 4 6 8, 0 2 1 5),(0.5 2.5 8 10, 1.0 2.5 9 11, 0.5 2.5 10 12, 0.5 2.5 8 10))"
  << 9 << 1 << 1
  << false
  << "Point (1.5 3)" << 0.0 << 2.0 << 3.0 << 4.0
  << 6.0 << 0.0 << 11.0;
  QTest::newRow( "geometry 110" ) << "Polygon(( 0 2 1, 3 2 2, 3 4 5, 0 4 6, 0 2 1))"
  << "PolygonZ((0 2 1, 3 2 2, 3 4 5, 0 4 6, 0 2 1))"
  << 5 << 1 << 0
  << false
  << "Point (1.5 3)" << 0.0 << 2.0 << 3.0 << 4.0
  << 6.0 << 0.0 << 10.0;
  QTest::newRow( "geometry 111" ) << "PolygonM ((0 0 1, 10 0 1, 10 10 1, 0 10 1, 0 0 1),(5 5 1, 7 5 1, 7 7 1, 5 7 1, 5 5 1),(1 1 1,2 1 1, 2 2 1, 1 2 1, 1 1 1))"
  << "POLYGON M ((0 0 1,10 0 1,10 10 1,0 10 1,0 0 1),(5 5 1,7 5 1,7 7 1,5 7 1,5 5 1),(1 1 1,2 1 1,2 2 1,1 2 1,1 1 1))"
  << 15 << 1 << 2
  << false
  << "POINT(4.99473684210526 4.99473684210526)" << 0.0 << 0.0 << 10.0 << 10.0
  << 95.0 << 0.0 << 52.0;
  QTest::newRow( "geometry 112" ) << "PolygonZ ((0 0 1 , 10 0 1, 10 10 1, 0 10 1, 0 0 1))"
  << "POLYGON Z ((0 0 1,10 0 1,10 10 1,0 10 1,0 0 1))"
  << 5 << 1 << 0
  << false
  << "POINT(5 5)" << 0.0 << 0.0 << 10.0 << 10.0
  << 100.0 << 0.0 << 40.0;
  QTest::newRow( "geometry 113" ) << "PolygonZ ((0 0 1, 10 0 1, 10 10 1, 0 10 1, 0 0 1),(5 5 1, 7 5 1, 7 7 1 , 5 7 1, 5 5 1))"
  << "POLYGON Z ((0 0 1,10 0 1,10 10 1,0 10 1,0 0 1),(5 5 1,7 5 1,7 7 1,5 7 1,5 5 1))"
  << 10 << 1 << 1
  << false
  << "POINT(4.95833333333333 4.95833333333333)" << 0.0 << 0.0 << 10.0 << 10.0
  << 96.0 << 0.0 << 48.0;

}

void TestQgsGeometry::checkGeometries()
{
  QFETCH( QString, wkt );
  QFETCH( QString, validWkt );
  QFETCH( int, numPoints );
  QFETCH( int, numGeometries );
  QFETCH( int, numRings );
  QFETCH( bool, isClosed );
  QFETCH( QString, centroid );
  QFETCH( double, xMin );
  QFETCH( double, yMin );
  QFETCH( double, xMax );
  QFETCH( double, yMax );
  QFETCH( double, area );
  QFETCH( double, length );
  QFETCH( double, perimeter );

  QScopedPointer<QgsGeometry> geom( QgsGeometry::fromWkt( wkt ) );
  if ( !geom->geometry() )
    qDebug() << "Could not create geometry: " << wkt;
  QVERIFY( geom->geometry() );

  QCOMPARE( geom->geometry()->nCoordinates(), numPoints );
  if ( numGeometries > 1 )
  {
    QgsGeometryCollectionV2* collection = dynamic_cast< QgsGeometryCollectionV2* >( geom->geometry() );
    QVERIFY( collection );
    QCOMPARE( collection->numGeometries(), numGeometries );
  }

  if ( numRings > 1 )
  {
    QgsCurvePolygonV2* surface = dynamic_cast< QgsCurvePolygonV2* >( geom->geometry() );
    QVERIFY( surface );
    QCOMPARE( surface->numInteriorRings(), numRings );
  }

  if ( isClosed )
  {
    QgsCurveV2* curve = dynamic_cast< QgsCurveV2* >( geom->geometry() );
    QVERIFY( curve );
    QCOMPARE( curve->isClosed(), isClosed );
  }

  //bounds
  QgsRectangle bbox = geom->geometry()->boundingBox();
  QVERIFY( qgsDoubleNear( bbox.xMinimum(), xMin, 0.000000001 ) );
  QVERIFY( qgsDoubleNear( bbox.yMinimum(), yMin, 0.000000001 ) );
  QVERIFY( qgsDoubleNear( bbox.xMaximum(), xMax, 0.000000001 ) );
  QVERIFY( qgsDoubleNear( bbox.yMaximum(), yMax, 0.000000001 ) );

  //area/length/perimeter
  QVERIFY( qgsDoubleNear( geom->geometry()->area(), area, 0.00001 ) );
  QVERIFY( qgsDoubleNear( geom->geometry()->length(), length, 0.00001 ) );
  QVERIFY( qgsDoubleNear( geom->geometry()->perimeter(), perimeter, 0.00001 ) );
}

void TestQgsGeometry::initTestCase()
{
  //
  // Runs once before any tests are run
  //
  // init QGIS's paths - true means that all path will be inited from prefix
  QgsApplication::init();
  QgsApplication::initQgis();
  QgsApplication::showSettings();
  mReport += "<h1>Geometry Tests</h1>\n";
  mReport += "<p><font color=\"green\">Green = polygonA</font></p>\n";
  mReport += "<p><font color=\"red\">Red = polygonB</font></p>\n";
  mReport += "<p><font color=\"blue\">Blue = polygonC</font></p>\n";
}


void TestQgsGeometry::cleanupTestCase()
{


  //
  // Runs once after all tests are run
  //
  QString myReportFile = QDir::tempPath() + "/qgistest.html";
  QFile myFile( myReportFile );
  if ( myFile.open( QIODevice::WriteOnly | QIODevice::Append ) )
  {
    QTextStream myQTextStream( &myFile );
    myQTextStream << mReport;
    myFile.close();
    //QDesktopServices::openUrl( "file:///" + myReportFile );
  }

  QgsApplication::exitQgis();
}

// MK, Disabled 14.11.2014
// Too unclear what exactly should be tested and which variations are allowed for the line
#if 0
void TestQgsGeometry::simplifyCheck1()
{
  QVERIFY( mpPolylineGeometryD->simplify( 0.5 ) );
  // should be a single polygon as A intersect B
  QgsGeometry * mypSimplifyGeometry  =  mpPolylineGeometryD->simplify( 0.5 );
  qDebug( "Geometry Type: %s", QGis::featureType( mypSimplifyGeometry->wkbType() ) );
  QVERIFY( mypSimplifyGeometry->wkbType() == QGis::WKBLineString );
  QgsPolyline myLine = mypSimplifyGeometry->asPolyline();
  QVERIFY( myLine.size() > 0 ); //check that the union created a feature
  dumpPolyline( myLine );
  delete mypSimplifyGeometry;
  QVERIFY( renderCheck( "geometry_simplifyCheck1", "Checking simplify of line" ) );
}
#endif

void TestQgsGeometry::intersectionCheck1()
{
  QVERIFY( mpPolygonGeometryA->intersects( mpPolygonGeometryB ) );
  // should be a single polygon as A intersect B
  QgsGeometry * mypIntersectionGeometry  =  mpPolygonGeometryA->intersection( mpPolygonGeometryB );
  qDebug( "Geometry Type: %s", QGis::featureType( mypIntersectionGeometry->wkbType() ) );
  QVERIFY( mypIntersectionGeometry->wkbType() == QGis::WKBPolygon );
  QgsPolygon myPolygon = mypIntersectionGeometry->asPolygon();
  QVERIFY( myPolygon.size() > 0 ); //check that the union created a feature
  dumpPolygon( myPolygon );
  delete mypIntersectionGeometry;
  QVERIFY( renderCheck( "geometry_intersectionCheck1", "Checking if A intersects B" ) );
}
void TestQgsGeometry::intersectionCheck2()
{
  QVERIFY( !mpPolygonGeometryA->intersects( mpPolygonGeometryC ) );
}

void TestQgsGeometry::translateCheck1()
{
  QString wkt = "LineString (0 0, 10 0, 10 10)";
  QScopedPointer<QgsGeometry> geom( QgsGeometry::fromWkt( wkt ) );
  geom->translate( 10, -5 );
  QString obtained = geom->exportToWkt();
  QString expected = "LineString (10 -5, 20 -5, 20 5)";
  QCOMPARE( obtained, expected );
  geom->translate( -10, 5 );
  obtained = geom->exportToWkt();
  QCOMPARE( obtained, wkt );

  wkt = "Polygon ((-2 4, -2 -10, 2 3, -2 4),(1 1, -1 1, -1 -1, 1 1))";
  geom.reset( QgsGeometry::fromWkt( wkt ) );
  geom->translate( -2, 10 );
  obtained = geom->exportToWkt();
  expected = "Polygon ((-4 14, -4 0, 0 13, -4 14),(-1 11, -3 11, -3 9, -1 11))";
  QCOMPARE( obtained, expected );
  geom->translate( 2, -10 );
  obtained = geom->exportToWkt();
  QCOMPARE( obtained, wkt );

  wkt = "Point (40 50)";
  geom.reset( QgsGeometry::fromWkt( wkt ) );
  geom->translate( -2, 10 );
  obtained = geom->exportToWkt();
  expected = "Point (38 60)";
  QCOMPARE( obtained, expected );
  geom->translate( 2, -10 );
  obtained = geom->exportToWkt();
  QCOMPARE( obtained, wkt );

}

void TestQgsGeometry::rotateCheck1()
{
  QString wkt = "LineString (0 0, 10 0, 10 10)";
  QScopedPointer<QgsGeometry> geom( QgsGeometry::fromWkt( wkt ) );
  geom->rotate( 90, QgsPoint( 0, 0 ) );
  QString obtained = geom->exportToWkt();
  QString expected = "LineString (0 0, 0 -10, 10 -10)";
  QCOMPARE( obtained, expected );
  geom->rotate( -90, QgsPoint( 0, 0 ) );
  obtained = geom->exportToWkt();
  QCOMPARE( obtained, wkt );

  wkt = "Polygon ((-2 4, -2 -10, 2 3, -2 4),(1 1, -1 1, -1 -1, 1 1))";
  geom.reset( QgsGeometry::fromWkt( wkt ) );
  geom->rotate( 90, QgsPoint( 0, 0 ) );
  obtained = geom->exportToWkt();
  expected = "Polygon ((4 2, -10 2, 3 -2, 4 2),(1 -1, 1 1, -1 1, 1 -1))";
  QCOMPARE( obtained, expected );
  geom->rotate( -90, QgsPoint( 0, 0 ) );
  obtained = geom->exportToWkt();
  QCOMPARE( obtained, wkt );

  wkt = "Point (40 50)";
  geom.reset( QgsGeometry::fromWkt( wkt ) );
  geom->rotate( 90, QgsPoint( 0, 0 ) );
  obtained = geom->exportToWkt();
  expected = "Point (50 -40)";
  QCOMPARE( obtained, expected );
  geom->rotate( -90, QgsPoint( 0, 0 ) );
  obtained = geom->exportToWkt();
  QCOMPARE( obtained, wkt );
  geom->rotate( 180, QgsPoint( 40, 0 ) );
  expected = "Point (40 -50)";
  obtained = geom->exportToWkt();
  QCOMPARE( obtained, expected );
  geom->rotate( 180, QgsPoint( 40, 0 ) ); // round-trip
  obtained = geom->exportToWkt();
  QCOMPARE( obtained, wkt );

}

void TestQgsGeometry::unionCheck1()
{
  // should be a multipolygon with 2 parts as A does not intersect C
  QgsGeometry * mypUnionGeometry  =  mpPolygonGeometryA->combine( mpPolygonGeometryC );
  qDebug( "Geometry Type: %s", QGis::featureType( mypUnionGeometry->wkbType() ) );
  QVERIFY( mypUnionGeometry->wkbType() == QGis::WKBMultiPolygon );
  QgsMultiPolygon myMultiPolygon = mypUnionGeometry->asMultiPolygon();
  QVERIFY( myMultiPolygon.size() > 0 ); //check that the union did not fail
  dumpMultiPolygon( myMultiPolygon );
  delete mypUnionGeometry;
  QVERIFY( renderCheck( "geometry_unionCheck1", "Checking A union C produces 2 polys" ) );
}

void TestQgsGeometry::unionCheck2()
{
  // should be a single polygon as A intersect B
  QgsGeometry * mypUnionGeometry  =  mpPolygonGeometryA->combine( mpPolygonGeometryB );
  qDebug( "Geometry Type: %s", QGis::featureType( mypUnionGeometry->wkbType() ) );
  QVERIFY( mypUnionGeometry->wkbType() == QGis::WKBPolygon );
  QgsPolygon myPolygon = mypUnionGeometry->asPolygon();
  QVERIFY( myPolygon.size() > 0 ); //check that the union created a feature
  dumpPolygon( myPolygon );
  delete mypUnionGeometry;
  QVERIFY( renderCheck( "geometry_unionCheck2", "Checking A union B produces single union poly" ) );
}

void TestQgsGeometry::differenceCheck1()
{
  // should be same as A since A does not intersect C so diff is 100% of A
  QSharedPointer<QgsGeometry> mypDifferenceGeometry( mpPolygonGeometryA->difference( mpPolygonGeometryC ) );
  qDebug( "Geometry Type: %s", QGis::featureType( mypDifferenceGeometry->wkbType() ) );
  QVERIFY( mypDifferenceGeometry->wkbType() == QGis::WKBPolygon );
  QgsPolygon myPolygon = mypDifferenceGeometry->asPolygon();
  QVERIFY( myPolygon.size() > 0 ); //check that the union did not fail
  dumpPolygon( myPolygon );
  QVERIFY( renderCheck( "geometry_differenceCheck1", "Checking (A - C) = A" ) );
}

void TestQgsGeometry::differenceCheck2()
{
  // should be a single polygon as (A - B) = subset of A
  QSharedPointer<QgsGeometry> mypDifferenceGeometry( mpPolygonGeometryA->difference( mpPolygonGeometryB ) );
  qDebug( "Geometry Type: %s", QGis::featureType( mypDifferenceGeometry->wkbType() ) );
  QVERIFY( mypDifferenceGeometry->wkbType() == QGis::WKBPolygon );
  QgsPolygon myPolygon = mypDifferenceGeometry->asPolygon();
  QVERIFY( myPolygon.size() > 0 ); //check that the union created a feature
  dumpPolygon( myPolygon );
  QVERIFY( renderCheck( "geometry_differenceCheck2", "Checking (A - B) = subset of A" ) );
}
void TestQgsGeometry::bufferCheck()
{
  // should be a single polygon
  QSharedPointer<QgsGeometry> mypBufferGeometry( mpPolygonGeometryB->buffer( 10, 10 ) );
  qDebug( "Geometry Type: %s", QGis::featureType( mypBufferGeometry->wkbType() ) );
  QVERIFY( mypBufferGeometry->wkbType() == QGis::WKBPolygon );
  QgsPolygon myPolygon = mypBufferGeometry->asPolygon();
  QVERIFY( myPolygon.size() > 0 ); //check that the buffer created a feature
  dumpPolygon( myPolygon );
  QVERIFY( renderCheck( "geometry_bufferCheck", "Checking buffer(10,10) of B", 10 ) );
}

void TestQgsGeometry::smoothCheck()
{
  //can't smooth a point
  QString wkt = "Point (40 50)";
  QScopedPointer<QgsGeometry> geom( QgsGeometry::fromWkt( wkt ) );
  QgsGeometry* result = geom->smooth( 1, 0.25 );
  QString obtained = result->exportToWkt();
  delete result;
  QCOMPARE( obtained, wkt );

  //linestring
  wkt = "LineString(0 0, 10 0, 10 10, 20 10)";
  geom.reset( QgsGeometry::fromWkt( wkt ) );
  result = geom->smooth( 1, 0.25 );
  QgsPolyline line = result->asPolyline();
  delete result;
  QgsPolyline expectedLine;
  expectedLine << QgsPoint( 0, 0 ) << QgsPoint( 7.5, 0 ) << QgsPoint( 10.0, 2.5 )
  << QgsPoint( 10.0, 7.5 ) << QgsPoint( 12.5, 10.0 ) << QgsPoint( 20.0, 10.0 );
  QVERIFY( QgsGeometry::compare( line, expectedLine ) );

  wkt = "MultiLineString ((0 0, 10 0, 10 10, 20 10),(30 30, 40 30, 40 40, 50 40))";
  geom.reset( QgsGeometry::fromWkt( wkt ) );
  result = geom->smooth( 1, 0.25 );
  QgsMultiPolyline multiLine = result->asMultiPolyline();
  delete result;
  QgsMultiPolyline expectedMultiline;
  expectedMultiline << ( QgsPolyline() << QgsPoint( 0, 0 ) << QgsPoint( 7.5, 0 ) << QgsPoint( 10.0, 2.5 )
                         <<  QgsPoint( 10.0, 7.5 ) << QgsPoint( 12.5, 10.0 ) << QgsPoint( 20.0, 10.0 ) )
  << ( QgsPolyline() << QgsPoint( 30.0, 30.0 ) << QgsPoint( 37.5, 30.0 ) << QgsPoint( 40.0, 32.5 )
       << QgsPoint( 40.0, 37.5 ) << QgsPoint( 42.5, 40.0 ) << QgsPoint( 50.0, 40.0 ) );
  QVERIFY( QgsGeometry::compare( multiLine, expectedMultiline ) );

  //polygon
  wkt = "Polygon ((0 0, 10 0, 10 10, 0 10, 0 0 ),(2 2, 4 2, 4 4, 2 4, 2 2))";
  geom.reset( QgsGeometry::fromWkt( wkt ) );
  result = geom->smooth( 1, 0.25 );
  QgsPolygon poly = result->asPolygon();
  delete result;
  QgsPolygon expectedPolygon;
  expectedPolygon << ( QgsPolyline() << QgsPoint( 2.5, 0 ) << QgsPoint( 7.5, 0 ) << QgsPoint( 10.0, 2.5 )
                       <<  QgsPoint( 10.0, 7.5 ) << QgsPoint( 7.5, 10.0 ) << QgsPoint( 2.5, 10.0 ) << QgsPoint( 0, 7.5 )
                       << QgsPoint( 0, 2.5 ) << QgsPoint( 2.5, 0 ) )
  << ( QgsPolyline() << QgsPoint( 2.5, 2.0 ) << QgsPoint( 3.5, 2.0 ) << QgsPoint( 4.0, 2.5 )
       << QgsPoint( 4.0, 3.5 ) << QgsPoint( 3.5, 4.0 ) << QgsPoint( 2.5, 4.0 )
       << QgsPoint( 2.0, 3.5 ) << QgsPoint( 2.0, 2.5 ) << QgsPoint( 2.5, 2.0 ) );
  QVERIFY( QgsGeometry::compare( poly, expectedPolygon ) );

  //multipolygon
  wkt = "MultiPolygon (((0 0, 10 0, 10 10, 0 10, 0 0 )),((2 2, 4 2, 4 4, 2 4, 2 2)))";
  geom.reset( QgsGeometry::fromWkt( wkt ) );
  result = geom->smooth( 1, 0.1 );
  QgsMultiPolygon multipoly = result->asMultiPolygon();
  delete result;
  QgsMultiPolygon expectedMultiPoly;
  expectedMultiPoly << ( QgsPolygon() << ( QgsPolyline() << QgsPoint( 1.0, 0 ) << QgsPoint( 9, 0 ) << QgsPoint( 10.0, 1 )
                         <<  QgsPoint( 10.0, 9 ) << QgsPoint( 9, 10.0 ) << QgsPoint( 1, 10.0 ) << QgsPoint( 0, 9 )
                         << QgsPoint( 0, 1 ) << QgsPoint( 1, 0 ) ) )
  << ( QgsPolygon() << ( QgsPolyline() << QgsPoint( 2.2, 2.0 ) << QgsPoint( 3.8, 2.0 ) << QgsPoint( 4.0, 2.2 )
                         <<  QgsPoint( 4.0, 3.8 ) << QgsPoint( 3.8, 4.0 ) << QgsPoint( 2.2, 4.0 ) << QgsPoint( 2.0, 3.8 )
                         << QgsPoint( 2, 2.2 ) << QgsPoint( 2.2, 2 ) ) );
  QVERIFY( QgsGeometry::compare( multipoly, expectedMultiPoly ) );
}

void TestQgsGeometry::dataStream()
{
  QString wkt = "Point (40 50)";
  QScopedPointer<QgsGeometry> geom( QgsGeometry::fromWkt( wkt ) );

  QByteArray ba;
  QDataStream ds( &ba, QIODevice::ReadWrite );
  ds << *geom;

  QgsGeometry resultGeometry;
  ds.device()->seek( 0 );
  ds >> resultGeometry;

  QCOMPARE( geom->geometry()->asWkt(), resultGeometry.geometry()->asWkt( ) );

  //also test with geometry without data
  QScopedPointer<QgsGeometry> emptyGeom( new QgsGeometry() );

  QByteArray ba2;
  QDataStream ds2( &ba2, QIODevice::ReadWrite );
  ds2 << emptyGeom;

  ds2.device()->seek( 0 );
  ds2 >> resultGeometry;

  QVERIFY( resultGeometry.isEmpty() );
}

bool TestQgsGeometry::renderCheck( const QString& theTestName, const QString& theComment, int mismatchCount )
{
  mReport += "<h2>" + theTestName + "</h2>\n";
  mReport += "<h3>" + theComment + "</h3>\n";
  QString myTmpDir = QDir::tempPath() + "/";
  QString myFileName = myTmpDir + theTestName + ".png";
  mImage.save( myFileName, "PNG" );
  QgsRenderChecker myChecker;
  myChecker.setControlName( "expected_" + theTestName );
  myChecker.setRenderedImage( myFileName );
  bool myResultFlag = myChecker.compareImages( theTestName, mismatchCount );
  mReport += myChecker.report();
  return myResultFlag;
}

void TestQgsGeometry::dumpMultiPolygon( QgsMultiPolygon &theMultiPolygon )
{
  for ( int i = 0; i < theMultiPolygon.size(); i++ )
  {
    QgsPolygon myPolygon = theMultiPolygon.at( i );
    dumpPolygon( myPolygon );
  }
}

void TestQgsGeometry::dumpPolygon( QgsPolygon &thePolygon )
{
  QVector<QPointF> myPoints;
  for ( int j = 0; j < thePolygon.size(); j++ )
  {
    QgsPolyline myPolyline = thePolygon.at( j ); //rings of polygon

    for ( int k = 0; k < myPolyline.size(); k++ )
    {
      QgsPoint myPoint = myPolyline.at( k );
      myPoints << QPointF( myPoint.x(), myPoint.y() );
    }
  }
  mpPainter->drawPolygon( myPoints );
}

void TestQgsGeometry::dumpPolyline( QgsPolyline &thePolyline )
{
  QVector<QPointF> myPoints;
//  QgsPolyline myPolyline = thePolyline.at( j ); //rings of polygon
  for ( int j = 0; j < thePolyline.size(); j++ )
  {
    QgsPoint myPoint = thePolyline.at( j );
//    QgsPolyline myPolyline = thePolygon.at( j ); //rings of polygon
    myPoints << QPointF( myPoint.x(), myPoint.y() );

//    for ( int k = 0; k < myPolyline.size(); k++ )
//    {
//      QgsPoint myPoint = myPolyline.at( k );
//      qDebug( "\t\t\tPoint in ring %d : %s", k, myPoint.toString().toLocal8Bit().constData() );
//      myPoints << QPointF( myPoint.x(), myPoint.y() );
//    }
  }
  mpPainter->drawPolyline( myPoints );
}

QTEST_MAIN( TestQgsGeometry )
#include "testqgsgeometry.moc"
