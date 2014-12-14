/***************************************************************************
                         testqgspagesizeregistry.cpp
                         ----------------------------
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

#include "qgspagesizeregistry.h"
#include "qgis.h"
#include <QObject>
#include <QtTest>

class TestQgsPageSizeRegistry : public QObject
{
    Q_OBJECT;

  private slots:
    void initTestCase();// will be called before the first testfunction is executed.
    void cleanupTestCase();// will be called after the last testfunction was executed.
    void init();// will be called before each testfunction is executed.
    void cleanup();// will be called after every testfunction.
    void pageSizeEquality(); //test equality of QgsPageSize
    void pageCopyConstructor(); //test copy constructor of QgsPageSize
    void createInstance(); // create global instance of QgsPageSizeRegistry
    void instanceHasDefaultSizes(); // check that global instance is populated with default page sizes
    void addSize(); // check adding a size to the registry
    void findSize(); //find a size in the registry
    void decodePageSize(); //test decoding a page size string

  private:

};

void TestQgsPageSizeRegistry::initTestCase()
{

}

void TestQgsPageSizeRegistry::cleanupTestCase()
{

}

void TestQgsPageSizeRegistry::init()
{

}

void TestQgsPageSizeRegistry::cleanup()
{

}

void TestQgsPageSizeRegistry::pageSizeEquality()
{
  QgsPageSize size1( QString( "test" ), QgsLayoutSize( 1, 2 ) );
  QgsPageSize size2( QString( "test" ), QgsLayoutSize( 1, 2 ) );
  QCOMPARE( size1, size2 );
  QVERIFY( !( size1 != size2 ) );
  QgsPageSize size3( QString( "no match" ), QgsLayoutSize( 1, 2 ) );
  QgsPageSize size4( QString( "test" ), QgsLayoutSize( 3, 4 ) );
  QVERIFY( !( size1 == size3 ) );
  QVERIFY( size1 != size3 );
  QVERIFY( !( size1 == size4 ) );
  QVERIFY( size1 != size4 );
  QVERIFY( !( size3 == size4 ) );
  QVERIFY( size3 != size4 );
}

void TestQgsPageSizeRegistry::pageCopyConstructor()
{
  QgsPageSize size1( QString( "test" ), QgsLayoutSize( 1, 2 ) );
  QgsPageSize size2( size1 );
  QCOMPARE( size1.name, size2.name );
  QCOMPARE( size1.size, size2.size );
}

void TestQgsPageSizeRegistry::createInstance()
{
  QgsPageSizeRegistry* registry = QgsPageSizeRegistry::instance();
  QVERIFY( registry );
}

void TestQgsPageSizeRegistry::instanceHasDefaultSizes()
{
  //check that registry instance is initially populated with some known page sizes
  QgsPageSizeRegistry* registry = QgsPageSizeRegistry::instance();
  QVERIFY( registry->entries().length() > 0 );
}

void TestQgsPageSizeRegistry::addSize()
{
  QgsPageSizeRegistry* registry = QgsPageSizeRegistry::instance();
  int oldSize = registry->entries().length();
  QgsPageSize newSize( QString( "new" ), QgsLayoutSize( 1, 2 ) );
  registry->add( newSize );
  QCOMPARE( registry->entries().length(), oldSize + 1 );
  QVERIFY( registry->entries().indexOf( newSize ) >= 0 );
}

void TestQgsPageSizeRegistry::findSize()
{
  QgsPageSizeRegistry* registry = QgsPageSizeRegistry::instance();
  QgsPageSize newSize( QString( "test size" ), QgsLayoutSize( 1, 2 ) );
  registry->add( newSize );
  QList< QgsPageSize > results = registry->find( QString( "test size" ) );
  QVERIFY( results.length() > 0 );
  QCOMPARE( results.at( 0 ), newSize );
  //check that match is case insensitive
  QList< QgsPageSize > results2 = registry->find( QString( "tEsT Size" ) );
  QVERIFY( results2.length() > 0 );
  QCOMPARE( results2.at( 0 ), newSize );
}

void TestQgsPageSizeRegistry::decodePageSize()
{
  QgsPageSizeRegistry* registry = QgsPageSizeRegistry::instance();

  //test with good string
  QgsPageSize result;
  QVERIFY( registry->decodePageSize( QString( " a3 " ), result ) );
  QCOMPARE( result.name, QString( "A3" ) );
  QCOMPARE( result.size, QgsLayoutSize( 297.0, 420.0 ) );

  //test with bad string
  QgsPageSize result2( QString( "nomatch" ), QgsLayoutSize( 10.0, 20.0 ) );
  QgsPageSize expected( result2 ); //for a bad match, expect page size to be unchanged
  QVERIFY( !registry->decodePageSize( QString( "bad" ), result2 ) );
  QCOMPARE( result2, expected );
}

QTEST_MAIN( TestQgsPageSizeRegistry )
#include "testqgspagesizeregistry.moc"
