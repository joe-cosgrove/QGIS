/***************************************************************************
                         testqgslayoutcontext.cpp
                         ------------------------
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

#include "qgslayoutcontext.h"
#include "qgis.h"
#include "qgsfeature.h"
#include "qgsvectorlayer.h"
#include <QObject>
#include <QtTest/QtTest>

class TestQgsLayoutContext: public QObject
{
    Q_OBJECT;

  private slots:
    void initTestCase();// will be called before the first testfunction is executed.
    void cleanupTestCase();// will be called after the last testfunction was executed.
    void init();// will be called before each testfunction is executed.
    void cleanup();// will be called after every testfunction.
    void creation(); //test creation of QgsLayout
    void flags(); //test QgsLayout flags
    void feature();
    void layer();

  private:
    QString mReport;

};

void TestQgsLayoutContext::initTestCase()
{
  mReport = "<h1>Layout Context Tests</h1>\n";
}

void TestQgsLayoutContext::cleanupTestCase()
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

void TestQgsLayoutContext::init()
{

}

void TestQgsLayoutContext::cleanup()
{

}

void TestQgsLayoutContext::creation()
{
  QgsLayoutContext* context = new QgsLayoutContext();
  QVERIFY( context );
  delete context;
}

void TestQgsLayoutContext::flags()
{
  QgsLayoutContext context;
  //test getting and setting flags
  context.setFlags( QgsLayoutContext::Flags( QgsLayoutContext::Antialiasing | QgsLayoutContext::UseAdvancedEffects ) );
  QVERIFY( context.flags() == ( QgsLayoutContext::Antialiasing | QgsLayoutContext::UseAdvancedEffects ) );
  QVERIFY( context.testFlag( QgsLayoutContext::Antialiasing ) );
  QVERIFY( context.testFlag( QgsLayoutContext::UseAdvancedEffects ) );
  QVERIFY( ! context.testFlag( QgsLayoutContext::Debug ) );
  context.setFlag( QgsLayoutContext::Debug );
  QVERIFY( context.testFlag( QgsLayoutContext::Debug ) );
  context.setFlag( QgsLayoutContext::Debug, false );
  QVERIFY( ! context.testFlag( QgsLayoutContext::Debug ) );
}

void TestQgsLayoutContext::feature()
{
  QgsLayoutContext context;

  //listen out for featureChanged signals
  qRegisterMetaType< QgsFeature >();
  QSignalSpy spyFeatureChanged( &context, SIGNAL( featureChanged( QgsFeature* ) ) );

  //test removing feature
  context.setFeature( NULL );
  QVERIFY( !context.feature() );
  //expect no feature changed signal, since going from no feature->no feature
  QCOMPARE( spyFeatureChanged.count(), 0 );

  //test setting/getting feature
  QgsFeature testFeature;
  testFeature.initAttributes( 1 );
  testFeature.setAttribute( 0, "Test" );
  context.setFeature( &testFeature );
  QCOMPARE( context.feature()->attribute( 0 ), testFeature.attribute( 0 ) );
  QCOMPARE( spyFeatureChanged.count(), 1 );

  //test overwriting feature
  QgsFeature* testFeature2 = new QgsFeature();
  testFeature2->initAttributes( 1 );
  testFeature2->setAttribute( 0, "Test2" );
  context.setFeature( testFeature2 );
  QCOMPARE( context.feature()->attribute( 0 ), testFeature2->attribute( 0 ) );
  QCOMPARE( spyFeatureChanged.count(), 2 );

  //test that feature was a copy
  QVERIFY( context.feature() != testFeature2 );
  delete testFeature2; //deleting original feature
  QCOMPARE( context.feature()->attribute( 0 ), QVariant( "Test2" ) );
}

void TestQgsLayoutContext::layer()
{
  QgsLayoutContext context;

  //listen out for layerChanged signals
  QSignalSpy spyLayerChanged( &context, SIGNAL( layerChanged( QgsVectorLayer* ) ) );

  //test clearing layer
  context.setLayer( NULL );
  QVERIFY( !context.layer() );
  //expect no layer changed signal, since going from no layer->no layer
  QCOMPARE( spyLayerChanged.count(), 0 );

  //test setting/getting layer
  QgsVectorLayer* layer = new QgsVectorLayer( "Point?field=id_a:integer", "A", "memory" );
  context.setLayer( layer );
  QCOMPARE( context.layer(), layer );
  QCOMPARE( spyLayerChanged.count(), 1 );
  //layer change signal should not be emitted if setting to same layer
  context.setLayer( layer );
  QCOMPARE( spyLayerChanged.count(), 1 );

  //test overwriting layer
  QgsVectorLayer* layer2 = new QgsVectorLayer( "Point?field=id_a:integer", "B", "memory" );
  context.setLayer( layer2 );
  QCOMPARE( context.layer(), layer2 );
  QCOMPARE( spyLayerChanged.count(), 2 );

  //clear layer
  context.setLayer( NULL );
  QVERIFY( !context.layer() );
  QCOMPARE( spyLayerChanged.count(), 3 );

  delete layer;
  delete layer2;
}

QTEST_MAIN( TestQgsLayoutContext )
#include "testqgslayoutcontext.moc"
