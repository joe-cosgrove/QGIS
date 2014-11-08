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
#include <QObject>
#include <QtTest>

class TestQgsLayout: public QObject
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

void TestQgsLayout::flags()
{
  QgsLayout layout;
  //test getting and setting flags
  layout.setFlags( QgsLayout::Flags( QgsLayout::Antialiasing | QgsLayout::UseAdvancedEffects ) );
  QVERIFY( layout.flags() == ( QgsLayout::Antialiasing | QgsLayout::UseAdvancedEffects ) );
  QVERIFY( layout.testFlag( QgsLayout::Antialiasing ) );
  QVERIFY( layout.testFlag( QgsLayout::UseAdvancedEffects ) );
  QVERIFY( ! layout.testFlag( QgsLayout::Debug ) );
  layout.setFlag( QgsLayout::Debug );
  QVERIFY( layout.testFlag( QgsLayout::Debug ) );
  layout.setFlag( QgsLayout::Debug, false );
  QVERIFY( ! layout.testFlag( QgsLayout::Debug ) );
}

QTEST_MAIN( TestQgsLayout )
#include "testqgslayout.moc"
