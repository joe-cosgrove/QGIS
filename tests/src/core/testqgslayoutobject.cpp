/***************************************************************************
                         testqgslayoutobject.cpp
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

#include "qgslayoutobject.h"
#include "qgslayout.h"
#include "qgslayoutcontext.h"
#include "qgsdatadefined.h"
#include "qgsapplication.h"
#include <QObject>
#include <QtTest/QtTest>

class TestQgsLayoutObject: public QObject
{
    Q_OBJECT;
  private slots:
    void initTestCase();// will be called before the first testfunction is executed.
    void cleanupTestCase();// will be called after the last testfunction was executed.
    void init();// will be called before each testfunction is executed.
    void cleanup();// will be called after every testfunction.
    void creation(); //test creation of QgsLayoutObject
    void layout(); //test fetching layout from QgsLayoutObject
    void writeReadXml(); //test writing object to xml and reading back from it
    void setRetrieveDDProperty(); //test setting and retreiving a data defined property
    void evaluateDDProperty(); //test evaluating data defined properties
    void applyDoubleDD(); //test applyDataDefinedProperty for doubles
    void readDDProperty(); //test reading data defined property from xml
    void writeRetrieveDDProperties(); //test writing and retrieving dd properties from xml

  private:
    QgsLayout* mLayout;
    QString mReport;

};

void TestQgsLayoutObject::initTestCase()
{
  QgsApplication::init();
  QgsApplication::initQgis();

  mLayout = new QgsLayout();
  mReport = "<h1>Layout Object Tests</h1>\n";
}

void TestQgsLayoutObject::cleanupTestCase()
{
  delete mLayout;

  QString myReportFile = QDir::tempPath() + QDir::separator() + "qgistest.html";
  QFile myFile( myReportFile );
  if ( myFile.open( QIODevice::WriteOnly | QIODevice::Append ) )
  {
    QTextStream myQTextStream( &myFile );
    myQTextStream << mReport;
    myFile.close();
  }

  QgsApplication::exitQgis();
}

void TestQgsLayoutObject::init()
{

}

void TestQgsLayoutObject::cleanup()
{

}

void TestQgsLayoutObject::creation()
{
  QgsLayoutObject* object = new QgsLayoutObject( mLayout );
  QVERIFY( object );
  delete object;
}

void TestQgsLayoutObject::layout()
{
  QgsLayoutObject* object = new QgsLayoutObject( mLayout );
  QCOMPARE( object->layout(), mLayout );
  delete object;
}

void TestQgsLayoutObject::writeReadXml()
{
  QgsLayoutObject* object = new QgsLayoutObject( mLayout );
  QDomImplementation DomImplementation;
  QDomDocumentType documentType =
    DomImplementation.createDocumentType(
      "qgis", "http://mrcc.com/qgis.dtd", "SYSTEM" );
  QDomDocument doc( documentType );

  //test writing with no parent node
  QDomElement rootNode = doc.createElement( "qgis" );
  QDomElement noNode;
  QCOMPARE( object->writeObjectXML( noNode, doc ), false );

  //test writing with node
  QDomElement layoutObjectElem = doc.createElement( "item" );
  rootNode.appendChild( layoutObjectElem );
  QVERIFY( object->writeObjectXML( layoutObjectElem, doc ) );

  //check if object node was written
  QDomNodeList evalNodeList = rootNode.elementsByTagName( "LayoutObject" );
  QCOMPARE( evalNodeList.count(), 1 );

  //test reading node
  QgsLayoutObject* readObject = new QgsLayoutObject( mLayout );

  //test reading with no node
  QCOMPARE( readObject->readObjectXML( noNode, doc ), false );

  //test node with no layout object child
  QDomElement badLayoutObjectElem = doc.createElement( "item" );
  rootNode.appendChild( badLayoutObjectElem );
  QCOMPARE( readObject->readObjectXML( badLayoutObjectElem, doc ), false );

  //test reading node
  QVERIFY( readObject->readObjectXML( layoutObjectElem, doc ) );

  delete object;
  delete readObject;
}

void TestQgsLayoutObject::setRetrieveDDProperty()
{
  QgsLayoutObject* object = new QgsLayoutObject( mLayout );
  QgsDataDefined* dd = new QgsDataDefined( true, true, QString( "10 + 40" ), QString() );

  //test setting bad properties
  object->setDataDefinedProperty( QgsLayoutObject::NoProperty, dd );
  object->setDataDefinedProperty( QgsLayoutObject::AllProperties, dd );

  //test setting good property
  object->setDataDefinedProperty( QgsLayoutObject::Transparency, dd );

  //test retrieving bad properties
  QgsDataDefined* result = 0;
  result = object->dataDefinedProperty( QgsLayoutObject::NoProperty );
  QVERIFY( !result );
  result = object->dataDefinedProperty( QgsLayoutObject::AllProperties );
  QVERIFY( !result );
  //property not set
  result = object->dataDefinedProperty( QgsLayoutObject::BlendMode );
  QVERIFY( !result );

  //test retrieving good property
  result = object->dataDefinedProperty( QgsLayoutObject::Transparency );
  QVERIFY( result );
  QCOMPARE( result, dd );
  QVERIFY( result->isActive() );
  QVERIFY( result->useExpression() );
  QCOMPARE( result->expressionString(), QString( "10 + 40" ) );

  //test overwriting property
  QgsDataDefined* dd2 = new QgsDataDefined( true, true, QString( "20 + 80" ), QString() );
  object->setDataDefinedProperty( QgsLayoutObject::Transparency, dd2 );
  QCOMPARE( object->dataDefinedProperty( QgsLayoutObject::Transparency ), dd2 );

  //test setting property to default
  QgsDataDefined* defaultDD = new QgsDataDefined();
  object->setDataDefinedProperty( QgsLayoutObject::Transparency, defaultDD );
  //should be nothing
  QVERIFY( !object->dataDefinedProperty( QgsLayoutObject::Transparency ) );

  //test setting property to null should clear property
  object->setDataDefinedProperty( QgsLayoutObject::Transparency, new QgsDataDefined( true, true, QString( "20 + 80" ), QString() ) );
  QVERIFY( object->dataDefinedProperty( QgsLayoutObject::Transparency ) );
  object->setDataDefinedProperty( QgsLayoutObject::Transparency, NULL );
  QVERIFY( !object->dataDefinedProperty( QgsLayoutObject::Transparency ) );

  delete object;
}


void TestQgsLayoutObject::evaluateDDProperty()
{
  QgsLayoutObject* object = new QgsLayoutObject( mLayout );
  QgsDataDefined* dd = new QgsDataDefined( true, true, QString( "10 + 40" ), QString() );
  object->setDataDefinedProperty( QgsLayoutObject::Transparency, dd );
  object->prepareDataDefinedExpressions();

  QVariant result;
  //test evaluating bad properties
  QCOMPARE( object->evaluateDataDefinedProperty( QgsLayoutObject::NoProperty, result ), false );
  QCOMPARE( object->evaluateDataDefinedProperty( QgsLayoutObject::AllProperties, result ), false );
  //not set property
  QCOMPARE( object->evaluateDataDefinedProperty( QgsLayoutObject::BlendMode, result ), false );

  //test retrieving good property
  QVERIFY( object->evaluateDataDefinedProperty( QgsLayoutObject::Transparency, result ) );
  QCOMPARE( result.toInt(), 50 );

  delete object;
  object = new QgsLayoutObject( mLayout );
  dd = new QgsDataDefined( true, true, QString( "10 + 40" ), QString() );
  result.clear();
  //test evaluating with unprepared expression
  object->setDataDefinedProperty( QgsLayoutObject::Transparency, dd );
  QVERIFY( object->evaluateDataDefinedProperty( QgsLayoutObject::Transparency, result ) );
  QCOMPARE( result.toInt(), 50 );

  //test with data defined property which should not be evaluated
  //result should be invalid
  dd = new QgsDataDefined();
  result.clear();
  object->setDataDefinedProperty( QgsLayoutObject::Transparency, dd );
  QVERIFY( !object->evaluateDataDefinedProperty( QgsLayoutObject::Transparency, result ) );
  QVERIFY( !result.isValid() );
  //also test with inactive property
  dd = new QgsDataDefined( false, true, QString( "10 + 40" ), QString() );
  result.clear();
  object->setDataDefinedProperty( QgsLayoutObject::Transparency, dd );
  QVERIFY( !object->evaluateDataDefinedProperty( QgsLayoutObject::Transparency, result ) );
  QVERIFY( !result.isValid() );

  //test with bad expression
  dd = new QgsDataDefined( true, true, QString( "a' + ." ), QString() );
  result.clear();
  object->setDataDefinedProperty( QgsLayoutObject::Transparency, dd );
  QVERIFY( !object->evaluateDataDefinedProperty( QgsLayoutObject::Transparency, result ) );
  QVERIFY( !result.isValid() );

  //also test with layer and feature
  QgsVectorLayer* layer = new QgsVectorLayer( "Point?field=col1:integer&field=col2:integer", "A", "memory", false );
  mLayout->context()->setLayer( layer );
  QgsFeature testFeature;
  testFeature.initAttributes( 2 );
  testFeature.setAttribute( 0, 5 );
  testFeature.setAttribute( 1, 7 );
  mLayout->context()->setFeature( &testFeature );

  //test with expression using feature
  dd = new QgsDataDefined( true, true, QString( "\"col1\" + \"col2\"" ), QString() );
  result.clear();
  object->setDataDefinedProperty( QgsLayoutObject::Transparency, dd );
  QVERIFY( object->evaluateDataDefinedProperty( QgsLayoutObject::Transparency, result ) );
  QCOMPARE( result.toInt(), 12 );

  //test with field from feature
  dd = new QgsDataDefined( true, false, QString(), QString( "col2" ) );
  result.clear();
  object->setDataDefinedProperty( QgsLayoutObject::Transparency, dd );
  QVERIFY( object->evaluateDataDefinedProperty( QgsLayoutObject::Transparency, result ) );
  QCOMPARE( result.toInt(), 7 );

  //test with bad field name from feature
  dd = new QgsDataDefined( true, false, QString(), QString( "col3" ) );
  result.clear();
  object->setDataDefinedProperty( QgsLayoutObject::Transparency, dd );
  QVERIFY( !object->evaluateDataDefinedProperty( QgsLayoutObject::Transparency, result ) );
  QVERIFY( !result.isValid() );

  //test that variant is overwritten
  dd = new QgsDataDefined( true, true, QString( "10 + 40" ), QString() );
  QVariant filledResult( 5 );
  object->setDataDefinedProperty( QgsLayoutObject::Transparency, dd );
  QVERIFY( object->evaluateDataDefinedProperty( QgsLayoutObject::Transparency, filledResult ) );
  QCOMPARE( filledResult.toInt(), 50 );

  //test that variant isn't overwritten for bad properties
  QVariant filledResult2( 5 );
  QVERIFY( !object->evaluateDataDefinedProperty( QgsLayoutObject::BlendMode, filledResult2 ) );
  QCOMPARE( filledResult2.toInt(), 5 );

  delete object;
}

void TestQgsLayoutObject::applyDoubleDD()
{
  QgsLayoutObject* object = new QgsLayoutObject( mLayout );
  QgsDataDefined* dd = new QgsDataDefined( true, true, QString( "10 + 40" ), QString() );
  object->setDataDefinedProperty( QgsLayoutObject::Transparency, dd );
  QCOMPARE( object->applyDataDefinedProperty( 20.0, QgsLayoutObject::Transparency ), 50.0 );

  //for the following tests applyDataDefinedProperty should not modify the passed value
  //bad properties
  QCOMPARE( object->applyDataDefinedProperty( 20.0, QgsLayoutObject::NoProperty ), 20.0 );
  QCOMPARE( object->applyDataDefinedProperty( 20.0, QgsLayoutObject::AllProperties ), 20.0 );
  QCOMPARE( object->applyDataDefinedProperty( 20.0, QgsLayoutObject::AllProperties, -99.0 ), 20.0 );
  //not set property
  QCOMPARE( object->applyDataDefinedProperty( 20.0, QgsLayoutObject::BlendMode ), 20.0 );
  QCOMPARE( object->applyDataDefinedProperty( 20.0, QgsLayoutObject::BlendMode, -99.0 ), 20.0 );
  //not double
  object->setDataDefinedProperty( QgsLayoutObject::Transparency, new QgsDataDefined( true, true, QString( "'a'" ) ) );
  QCOMPARE( object->applyDataDefinedProperty( 20.0, QgsLayoutObject::Transparency ), 20.0 );
  QCOMPARE( object->applyDataDefinedProperty( 20.0, QgsLayoutObject::Transparency, -99.0 ), -99.0 );

  //null (have to create a feature to test)
  QgsVectorLayer* layer = new QgsVectorLayer( "Point?field=col1:integer", "A", "memory", false );
  mLayout->context()->setLayer( layer );
  QgsFeature testFeature;
  testFeature.initAttributes( 1 );
  QVariant null( QVariant::Double );
  testFeature.setAttribute( 0, null );
  mLayout->context()->setFeature( &testFeature );

  //test with expression using feature
  dd = new QgsDataDefined( true, false, QString(), QString( "col1" ) );
  object->setDataDefinedProperty( QgsLayoutObject::Transparency, dd );
  QCOMPARE( object->applyDataDefinedProperty( 20.0, QgsLayoutObject::Transparency ), 20.0 );
  QCOMPARE( object->applyDataDefinedProperty( 20.0, QgsLayoutObject::Transparency, -99.0 ), -99.0 );
}

void TestQgsLayoutObject::readDDProperty()
{
  QgsLayoutObject* object = new QgsLayoutObject( mLayout );

  //create a test dom element
  QDomImplementation DomImplementation;
  QDomDocumentType documentType =
    DomImplementation.createDocumentType(
      "qgis", "http://mrcc.com/qgis.dtd", "SYSTEM" );
  QDomDocument doc( documentType );

  //dd element
  QgsDataDefined dd;
  dd.setActive( true );
  dd.setUseExpression( true );
  dd.setExpressionString( QString( "test expression" ) );
  dd.setField( QString( "test field" ) );
  QDomElement ddElem = dd.toXmlElement( doc, "ddelem" );

  //try reading dd elements

  //bad data defined properties - should not be read into dataDefinedProperties map
  QVERIFY( !object->readDataDefinedProperty( QgsLayoutObject::NoProperty, ddElem ) );
  QVERIFY( !object->readDataDefinedProperty( QgsLayoutObject::AllProperties, ddElem ) );
  //bad element
  QDomElement badElem;
  QVERIFY( !object->readDataDefinedProperty( QgsLayoutObject::Transparency, badElem ) );
  QVERIFY( !object->dataDefinedProperty( QgsLayoutObject::Transparency ) );

  //read into valid property, not already set
  QVERIFY( object->readDataDefinedProperty( QgsLayoutObject::Transparency, ddElem ) );
  //check result
  QCOMPARE( *( object->dataDefinedProperty( QgsLayoutObject::Transparency ) ), dd );

  //test reading into already set property
  QgsDataDefined dd2;
  dd2.setActive( false );
  dd2.setUseExpression( false );
  dd2.setExpressionString( QString( "test expression2" ) );
  dd2.setField( QString( "test field2" ) );
  QDomElement dd2Elem = dd2.toXmlElement( doc, "dd2elem" );

  QVERIFY( object->readDataDefinedProperty( QgsLayoutObject::Transparency, dd2Elem ) );
  //check result
  QCOMPARE( *( object->dataDefinedProperty( QgsLayoutObject::Transparency ) ), dd2 );

  delete object;
}

void TestQgsLayoutObject::writeRetrieveDDProperties()
{
  QgsLayoutObject* object = new QgsLayoutObject( mLayout );
  QgsDataDefined* ddBefore = new QgsDataDefined();
  ddBefore->setActive( true );
  ddBefore->setUseExpression( true );
  ddBefore->setExpressionString( QString( "10+40" ) );
  ddBefore->setField( QString( "field" ) );
  object->setDataDefinedProperty( QgsLayoutObject::TestProperty, ddBefore );

  //test writing object with dd settings
  QDomImplementation DomImplementation;
  QDomDocumentType documentType =
    DomImplementation.createDocumentType(
      "qgis", "http://mrcc.com/qgis.dtd", "SYSTEM" );
  QDomDocument doc( documentType );
  QDomElement rootNode = doc.createElement( "qgis" );
  QDomElement itemElem = doc.createElement( "item" );
  rootNode.appendChild( itemElem );
  QVERIFY( object->writeObjectXML( itemElem, doc ) );

  //check if object node was written
  QDomNodeList evalNodeList = itemElem.elementsByTagName( "LayoutObject" );
  QCOMPARE( evalNodeList.count(), 1 );

  //test reading node containing dd settings
  QgsLayoutObject* readObject = new QgsLayoutObject( mLayout );
  QVERIFY( readObject->readObjectXML( itemElem, doc ) );

  //test getting not set dd from restored object
  QVERIFY( !readObject->dataDefinedProperty( QgsLayoutObject::BlendMode ) );

  //test getting good property
  QgsDataDefined* ddAfter = readObject->dataDefinedProperty( QgsLayoutObject::TestProperty );
  QVERIFY( ddAfter );
  QCOMPARE( *ddAfter, *ddBefore );
  //evaluating restored dd property
  QVariant result;
  QVERIFY( readObject->evaluateDataDefinedProperty( QgsLayoutObject::TestProperty, result ) );
  QCOMPARE( result.toInt(), 50 );

  delete object;
  delete readObject;

  //HERE - haven't run test
}

QTEST_MAIN( TestQgsLayoutObject )
#include "testqgslayoutobject.moc"
