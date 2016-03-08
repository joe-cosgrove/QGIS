/***************************************************************************
     testqgssvgcache.cpp
     -------------------
    Date                 : 2016-03-08
    Copyright            : (C) 2016 Nyall Dawson
    Email                : nyall dot dawson at gmail.com
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
#include <QStringList>
#include <QApplication>
#include <QFileInfo>

//qgis includes...
#include "qgsmultirenderchecker.h"
#include "qgsapplication.h"
#include "qgssvgcache.h"

/** \ingroup UnitTests
 * This is a unit test for QgsSvgCache
 */
class TestQgsSvgCache : public QObject
{
    Q_OBJECT

  private:

    QString mReport;
    QString mTestDataDir;

    bool imageCheck( const QString& testName, QImage &image, int mismatchCount );

  private slots:

    // init / cleanup
    void initTestCase();// will be called before the first testfunction is executed.
    void cleanupTestCase();// will be called after the last testfunction was executed.
    void init() {}// will be called before each testfunction is executed.
    void cleanup() {}// will be called after every testfunction.
    void changeImage(); //check that cache is updated if svg source file changes

};

// slots
void TestQgsSvgCache::initTestCase()
{
  // initialize with test settings directory so we don't mess with user's stuff
  QgsApplication::init( QDir::tempPath() + "/dot-qgis" );
  QgsApplication::initQgis();
  QgsApplication::createDB();
  mTestDataDir = QString( TEST_DATA_DIR ) + '/'; //defined in CmakeLists.txt

  mReport += "<h1>QgsSvgCache Tests</h1>\n";
}

void TestQgsSvgCache::cleanupTestCase()
{
  QgsApplication::exitQgis();

  QString myReportFile = QDir::tempPath() + "/qgistest.html";
  QFile myFile( myReportFile );
  if ( myFile.open( QIODevice::WriteOnly | QIODevice::Append ) )
  {
    QTextStream myQTextStream( &myFile );
    myQTextStream << mReport;
    myFile.close();
    //QDesktopServices::openUrl( "file:///" + myReportFile );
  }
}

void TestQgsSvgCache::changeImage()
{
  bool inCache;

  //copy an image to the temp folder
  QString tempImagePath = QDir::tempPath() + "/svg_cache.svg";

  QString originalImage = mTestDataDir + "test_symbol_svg.svg";
  QFile::copy( originalImage, tempImagePath );

  //render it through the cache
  QImage img = QgsSvgCache::instance()->svgAsImage( tempImagePath, 200, QColor( 0, 0, 0 ), QColor( 0, 0, 0 ), 1.0,
               1.0, 1.0, inCache );
  //replace the image in the temp folder
  QString newImage = mTestDataDir + "test_symbol_svg2.svg";
  QFile::remove( tempImagePath );
  QFile::copy( newImage, tempImagePath );

  //re-render it
  img = QgsSvgCache::instance()->svgAsImage( tempImagePath, 200, QColor( 0, 0, 0 ), QColor( 0, 0, 0 ), 1.0,
        1.0, 1.0, inCache );
  QVERIFY( imageCheck( "svgcache_changed", img, 30 ) );
}

bool TestQgsSvgCache::imageCheck( const QString& testName, QImage &image, int mismatchCount )
{
  //draw background
  QImage imageWithBackground( image.width(), image.height(), QImage::Format_RGB32 );
  QgsRenderChecker::drawBackground( &imageWithBackground );
  QPainter painter( &imageWithBackground );
  painter.drawImage( 0, 0, image );
  painter.end();

  mReport += "<h2>" + testName + "</h2>\n";
  QString tempDir = QDir::tempPath() + '/';
  QString fileName = tempDir + testName + ".png";
  imageWithBackground.save( fileName, "PNG" );
  QgsRenderChecker checker;
  checker.setControlName( "expected_" + testName );
  checker.setRenderedImage( fileName );
  checker.setColorTolerance( 2 );
  bool resultFlag = checker.compareImages( testName, mismatchCount );
  mReport += checker.report();
  return resultFlag;
}

QTEST_MAIN( TestQgsSvgCache )
#include "testqgssvgcache.moc"
