/***************************************************************************
                         testqgsimageoperations.cpp
                         --------------------------
    begin                : January 2015
    copyright            : (C) 2015 by Nyall Dawson
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

#include "qgsimageoperation.h"
#include <QObject>
#include <QtTest/QtTest>
#include "qgsrenderchecker.h"

class TestQgsImageOperation : public QObject
{
    Q_OBJECT

  private slots:
    void initTestCase();// will be called before the first testfunction is executed.
    void cleanupTestCase();// will be called after the last testfunction was executed.
    void init();// will be called before each testfunction is executed.
    void cleanup();// will be called after every testfunction.
    void smallImageOp(); //test operation on small image (single threaded op)

    //grayscale
    void grayscaleLightness();
    void grayscaleLuminosity();
    void grayscaleAverage();

    //brightness/contrast
    void brightnessContrastNoChange();
    void increaseBrightness();
    void decreaseBrightness();
    void increaseContrast();
    void decreaseContrast();

  private:

    QString mReport;
    QString mSampleImage;

    bool imageCheck( QString testName , QImage &image, int mismatchCount );

};

void TestQgsImageOperation::initTestCase()
{
  mReport += "<h1>Image Operation Tests</h1>\n";
  mSampleImage = QString( TEST_DATA_DIR ) + QDir::separator() +  "sample_image.png";
}

void TestQgsImageOperation::cleanupTestCase()
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

void TestQgsImageOperation::init()
{

}

void TestQgsImageOperation::cleanup()
{

}

void TestQgsImageOperation::smallImageOp()
{
  QImage image( QString( TEST_DATA_DIR ) + QDir::separator() +  "small_sample_image.png" );
  QgsImageOperation::convertToGrayscale( image, QgsImageOperation::GrayscaleLightness );

  bool result = imageCheck( QString( "imageop_smallimage" ), image, 0 );
  QVERIFY( result );
}

void TestQgsImageOperation::grayscaleLightness()
{
  QImage image( mSampleImage );
  QgsImageOperation::convertToGrayscale( image, QgsImageOperation::GrayscaleLightness );

  bool result = imageCheck( QString( "imageop_graylightness" ), image, 0 );
  QVERIFY( result );
}

void TestQgsImageOperation::grayscaleLuminosity()
{
  QImage image( mSampleImage );
  QgsImageOperation::convertToGrayscale( image, QgsImageOperation::GrayscaleLuminosity );

  bool result = imageCheck( QString( "imageop_grayluminosity" ), image, 0 );
  QVERIFY( result );
}

void TestQgsImageOperation::grayscaleAverage()
{
  QImage image( mSampleImage );
  QgsImageOperation::convertToGrayscale( image, QgsImageOperation::GrayscaleAverage );

  bool result = imageCheck( QString( "imageop_grayaverage" ), image, 0 );
  QVERIFY( result );
}

void TestQgsImageOperation::brightnessContrastNoChange()
{
  QImage image( mSampleImage );
  QgsImageOperation::adjustBrightnessContrast( image, 0, 1.0 );

  bool result = imageCheck( QString( "imageop_bcnochange" ), image, 0 );
  QVERIFY( result );
}

void TestQgsImageOperation::increaseBrightness()
{
  QImage image( mSampleImage );
  QgsImageOperation::adjustBrightnessContrast( image, 50, 1.0 );

  bool result = imageCheck( QString( "imageop_increasebright" ), image, 0 );
  QVERIFY( result );
}

void TestQgsImageOperation::decreaseBrightness()
{
  QImage image( mSampleImage );
  QgsImageOperation::adjustBrightnessContrast( image, -50, 1.0 );

  bool result = imageCheck( QString( "imageop_decreasebright" ), image, 0 );
  QVERIFY( result );
}

void TestQgsImageOperation::increaseContrast()
{
  QImage image( mSampleImage );
  QgsImageOperation::adjustBrightnessContrast( image, 0, 30.0 );

  bool result = imageCheck( QString( "imageop_increasecontrast" ), image, 0 );
  QVERIFY( result );
}

void TestQgsImageOperation::decreaseContrast()
{
  QImage image( mSampleImage );
  QgsImageOperation::adjustBrightnessContrast( image, 0, 0.1 );

  bool result = imageCheck( QString( "imageop_decreasecontrast" ), image, 0 );
  QVERIFY( result );
}

//
// Private helper functions not called directly by CTest
//

bool TestQgsImageOperation::imageCheck( QString testName, QImage &image, int mismatchCount )
{
  //draw background
  QImage imageWithBackground( image.width(), image.height(), QImage::Format_RGB32 );
  QgsRenderChecker::drawBackground( &imageWithBackground );
  QPainter painter( &imageWithBackground );
  painter.drawImage( 0, 0, image );
  painter.end();

  mReport += "<h2>" + testName + "</h2>\n";
  QString tempDir = QDir::tempPath() + QDir::separator();
  QString fileName = tempDir + testName + ".png";
  imageWithBackground.save( fileName, "PNG" );
  QgsRenderChecker checker;
  checker.setControlName( "expected_" + testName );
  checker.setRenderedImage( fileName );
  checker.setColorTolerance( 1 );
  bool resultFlag = checker.compareImages( testName, mismatchCount );
  mReport += checker.report();
  return resultFlag;
}

QTEST_MAIN( TestQgsImageOperation )
#include "testqgsimageoperation.moc"
