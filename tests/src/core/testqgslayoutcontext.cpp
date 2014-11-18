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
#include <QObject>
#include <QtTest>

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

QTEST_MAIN( TestQgsLayoutContext )
#include "testqgslayoutcontext.moc"
