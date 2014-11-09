/***************************************************************************
                         testqgslayoutpage.cpp
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

#include "qgslayoutpage.h"
#include <QObject>
#include <QtTest>

class TestQgsLayoutPage : public QObject
{
    Q_OBJECT;

  private slots:
    void initTestCase();// will be called before the first testfunction is executed.
    void cleanupTestCase();// will be called after the last testfunction was executed.
    void init();// will be called before each testfunction is executed.
    void cleanup();// will be called after every testfunction.

    void creation();
    void itemType();
    void decodePageOrientation();

  private:

};

void TestQgsLayoutPage::initTestCase()
{

}

void TestQgsLayoutPage::cleanupTestCase()
{

}

void TestQgsLayoutPage::init()
{

}

void TestQgsLayoutPage::cleanup()
{

}

void TestQgsLayoutPage::creation()
{
  QgsLayoutPage* page = new QgsLayoutPage( 0 );
  QVERIFY( page );
  delete page;
}

void TestQgsLayoutPage::itemType()
{
  QgsLayoutPage* page = new QgsLayoutPage( 0 );
  QCOMPARE( page->type(), ( int )QgsLayoutItemRegistry::LayoutPage );
  delete page;
}

void TestQgsLayoutPage::decodePageOrientation()
{
  //test good string
  QgsLayoutPage::Orientation orientation = QgsLayoutPage::Landscape;
  QVERIFY( QgsLayoutPage::decodePageOrientation( QString( " porTrait " ), orientation ) );
  QCOMPARE( orientation, QgsLayoutPage::Portrait );

  //test bad string
  QgsLayoutPage::Orientation badOrientation = QgsLayoutPage::Landscape;
  QVERIFY( !QgsLayoutPage::decodePageOrientation( QString(), badOrientation ) );
  QCOMPARE( badOrientation, QgsLayoutPage::Landscape );

}

QTEST_MAIN( TestQgsLayoutPage )
#include "testqgslayoutpage.moc"
