/***************************************************************************
                         testqgslayoutitem.cpp
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

#include "qgslayoutitem.h"
#include "qgslayout.h"
#include "qgslayoutcontext.h"
#include "qgscompositionchecker.h"
#include "qgsdatadefined.h"
#include <QObject>
#include <QtTest/QtTest>
#include <QPainter>
#include <QImage>

class TestQgsLayoutItem: public QObject
{
    Q_OBJECT;
  private slots:
    void initTestCase();// will be called before the first testfunction is executed.
    void cleanupTestCase();// will be called after the last testfunction was executed.
    void init();// will be called before each testfunction is executed.
    void cleanup();// will be called after every testfunction.
    void creation(); //test creation of QgsLayoutItem
    void uuid();
    void id();
    void shouldDrawDebug();
    void shouldDrawAntialiased();
    void preparePainter();
    void debugRect();
    void draw();
    void resize();
    void referencePoint();
    void itemPositionReferencePoint();
    void adjustPointForReference();
    void positionAtReferencePoint();
    void fixedSize();
    void minSize();
    void move();
    void positionWithUnits();
    void sizeWithUnits();
    void dataDefinedPosition();
    void dataDefinedSize();
    void combinedDataDefinedPositionAndSize();
    void rotation();
    void writeXml();
    void readXml();
    void writeReadXmlProperties();

  private:

    //simple item for testing, since some methods in QgsLayoutItem are pure virtual
    class TestItem : public QgsLayoutItem
    {
      public:

        TestItem( QgsLayout* layout ) : QgsLayoutItem( layout ) {}
        ~TestItem() {}

        //implement pure virtual methods
        int type() const { return QgsLayoutItemRegistry::LayoutItem + 101; }
        QString stringType() const { return QString( "testItem" ); }
        void draw( QPainter* painter, const QStyleOptionGraphicsItem* itemStyle, QWidget* pWidget )
        {
          Q_UNUSED( itemStyle );
          Q_UNUSED( pWidget );
          painter->save();
          painter->setRenderHint( QPainter::Antialiasing, false );
          painter->setPen( Qt::NoPen );
          painter->setBrush( QColor( 255, 100, 100, 200 ) );
          painter->drawRect( rect() );
          painter->restore();
        }

        virtual bool writePropertiesToElement( QDomElement& element, QDomDocument& document ) const
        {
          element.setAttribute( QString( "testAttribute" ), QString( "testValue" ) );
          return QgsLayoutItem::writePropertiesToElement( element, document );;
        }
    };

    //item with minimum size
    class MinSizedItem : public TestItem
    {
      public:
        MinSizedItem( QgsLayout* layout ) : TestItem( layout )
        {
          setMinimumSize( QgsLayoutSize( 5.0, 10.0, QgsLayoutUnits::Centimeters ) );
        }

        void updateMinSize( QgsLayoutSize size )
        {
          setMinimumSize( size );
        }

        ~MinSizedItem() {}
    };

    //item with fixed size
    class FixedSizedItem : public TestItem
    {
      public:

        FixedSizedItem( QgsLayout* layout ) : TestItem( layout )
        {
          setFixedSize( QgsLayoutSize( 2.0, 4.0, QgsLayoutUnits::Inches ) );
        }

        void updateFixedSize( QgsLayoutSize size )
        {
          setFixedSize( size );
        }
        ~FixedSizedItem() {}
    };

    //item with both conflicting fixed and minimum size
    class FixedMinSizedItem : public TestItem
    {
      public:

        FixedMinSizedItem( QgsLayout* layout ) : TestItem( layout )
        {
          setFixedSize( QgsLayoutSize( 2.0, 4.0, QgsLayoutUnits::Centimeters ) );
          setMinimumSize( QgsLayoutSize( 5.0, 9.0, QgsLayoutUnits::Centimeters ) );
        }
        ~FixedMinSizedItem() {}
    };

    QgsLayout* mLayout;
    QString mReport;

    //creates a copy of a layout item by first saving to xml and then restoring
    QgsLayoutItem* createCopyViaXml( QgsLayoutItem* original );
    bool renderCheck( QString testName, QImage &image, int mismatchCount );

};

void TestQgsLayoutItem::initTestCase()
{
  mLayout = new QgsLayout();
  mReport = "<h1>Layout Item Tests</h1>\n";
}

void TestQgsLayoutItem::cleanupTestCase()
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
}

void TestQgsLayoutItem::init()
{

}

void TestQgsLayoutItem::cleanup()
{

}

void TestQgsLayoutItem::creation()
{
  TestItem* item = new TestItem( mLayout );
  QVERIFY( item );
  delete item;
}

void TestQgsLayoutItem::uuid()
{
  //basic test of uuid
  TestItem item( mLayout );
  TestItem item2( mLayout );
  QVERIFY( item.uuid() != item2.uuid() );
}

void TestQgsLayoutItem::id()
{
  TestItem item( mLayout );
  item.setId( QString( "test" ) );
  QCOMPARE( item.id(), QString( "test" ) );
}

void TestQgsLayoutItem::shouldDrawDebug()
{
  TestItem* item = new TestItem( mLayout );
  mLayout->context()->setFlag( QgsLayoutContext::Debug, true );
  QVERIFY( item->shouldDrawDebugRect() );
  mLayout->context()->setFlag( QgsLayoutContext::Debug, false );
  QVERIFY( !item->shouldDrawDebugRect() );
  delete item;
}

void TestQgsLayoutItem::shouldDrawAntialiased()
{
  TestItem* item = new TestItem( mLayout );
  mLayout->context()->setFlag( QgsLayoutContext::Antialiasing, false );
  QVERIFY( !item->shouldDrawAntialiased() );
  mLayout->context()->setFlag( QgsLayoutContext::Antialiasing, true );
  QVERIFY( item->shouldDrawAntialiased() );
  delete item;
}

void TestQgsLayoutItem::preparePainter()
{
  TestItem* item = new TestItem( mLayout );
  //test with no painter
  item->preparePainter( 0 );

  //test antialiasing correctly set for painter
  QImage image( QSize( 100, 100 ), QImage::Format_ARGB32 );
  QPainter painter;
  painter.begin( &image );
  mLayout->context()->setFlag( QgsLayoutContext::Antialiasing, false );
  item->preparePainter( &painter );
  QVERIFY( !( painter.renderHints() & QPainter::Antialiasing ) );
  mLayout->context()->setFlag( QgsLayoutContext::Antialiasing, true );
  item->preparePainter( &painter );
  QVERIFY( painter.renderHints() & QPainter::Antialiasing );
}

void TestQgsLayoutItem::debugRect()
{
  TestItem* item = new TestItem( mLayout );
  mLayout->addItem( item );
  item->setPos( 100, 100 );
  item->setRect( 0, 0, 200, 200 );
  mLayout->setSceneRect( 0, 0, 400, 400 );
  mLayout->context()->setFlag( QgsLayoutContext::Debug, true );
  QImage image( mLayout->sceneRect().size().toSize(), QImage::Format_ARGB32 );
  image.fill( 0 );
  QPainter painter( &image );
  mLayout->render( &painter );
  painter.end();

  bool result = renderCheck( "layoutitem_debugrect", image, 0 );
  mLayout->context()->setFlag( QgsLayoutContext::Debug, false );
  mLayout->removeItem( item );
  delete item;
  QVERIFY( result );
}

void TestQgsLayoutItem::draw()
{
  TestItem* item = new TestItem( mLayout );
  mLayout->addItem( item );
  item->setPos( 100, 100 );
  item->setRect( 0, 0, 200, 200 );
  mLayout->setSceneRect( 0, 0, 400, 400 );
  mLayout->context()->setFlag( QgsLayoutContext::Antialiasing, false ); //disable antialiasing to limit cross platform differences
  QImage image( mLayout->sceneRect().size().toSize(), QImage::Format_ARGB32 );
  image.fill( 0 );
  QPainter painter( &image );
  mLayout->render( &painter );
  painter.end();
  mLayout->context()->setFlag( QgsLayoutContext::Antialiasing, true );
  bool result = renderCheck( "layoutitem_draw", image, 0 );
  mLayout->removeItem( item );
  delete item;
  QVERIFY( result );
}

void TestQgsLayoutItem::resize()
{
  //resize test item (no restrictions), same units as layout
  mLayout->setUnits( QgsLayoutUnits::Millimeters );
  TestItem* item = new TestItem( mLayout );
  item->setRect( 0, 0, 55, 45 );
  item->attemptMove( QgsLayoutPoint( 27, 29 ) );
  item->attemptResize( QgsLayoutSize( 100.0, 200.0, QgsLayoutUnits::Millimeters ) );
  QCOMPARE( item->rect().width(), 100.0 );
  QCOMPARE( item->rect().height(), 200.0 );
  QCOMPARE( item->scenePos().x(), 27.0 ); //item should not move
  QCOMPARE( item->scenePos().y(), 29.0 );

  //test conversion of units
  mLayout->setUnits( QgsLayoutUnits::Centimeters );
  item->setRect( 0, 0, 100, 200 );
  item->attemptResize( QgsLayoutSize( 0.30, 0.45, QgsLayoutUnits::Meters ) );
  QCOMPARE( item->rect().width(), 30.0 );
  QCOMPARE( item->rect().height(), 45.0 );

  //test pixel -> page conversion
  mLayout->setUnits( QgsLayoutUnits::Inches );
  mLayout->setDpi( 100.0 );
  item->setRect( 0, 0, 1, 2 );
  item->attemptResize( QgsLayoutSize( 140, 280, QgsLayoutUnits::Pixels ) );
  QCOMPARE( item->rect().width(), 1.4 );
  QCOMPARE( item->rect().height(), 2.8 );
  //changing the dpi should resize the item
  mLayout->setDpi( 200.0 );
  QCOMPARE( item->rect().width(), 0.7 );
  QCOMPARE( item->rect().height(), 1.4 );

  //test page -> pixel conversion
  mLayout->setUnits( QgsLayoutUnits::Pixels );
  mLayout->setDpi( 100.0 );
  item->setRect( 0, 0, 2, 2 );
  item->attemptResize( QgsLayoutSize( 1, 3, QgsLayoutUnits::Inches ) );
  QCOMPARE( item->rect().width(), 100.0 );
  QCOMPARE( item->rect().height(), 300.0 );
  //changing dpi results in item resize
  mLayout->setDpi( 200.0 );
  QCOMPARE( item->rect().width(), 200.0 );
  QCOMPARE( item->rect().height(), 600.0 );

  mLayout->setUnits( QgsLayoutUnits::Millimeters );
  delete item;
}

void TestQgsLayoutItem::referencePoint()
{
  //test setting/getting reference point
  TestItem* item = new TestItem( mLayout );
  item->setReferencePoint( QgsLayoutItem::LowerMiddle );
  QCOMPARE( item->referencePoint(), QgsLayoutItem::LowerMiddle );

  //test that setting reference point results in positionWithUnits returning position at new reference
  //point
  item->setReferencePoint( QgsLayoutItem::UpperLeft );
  item->attemptResize( QgsLayoutSize( 2, 4 ) );
  item->attemptMove( QgsLayoutPoint( 1, 2 ) );
  QCOMPARE( item->positionWithUnits().x(), 1.0 );
  QCOMPARE( item->positionWithUnits().y(), 2.0 );
  item->setReferencePoint( QgsLayoutItem::UpperLeft );
  QCOMPARE( item->positionWithUnits().x(), 1.0 );
  QCOMPARE( item->positionWithUnits().y(), 2.0 );
  item->setReferencePoint( QgsLayoutItem::UpperMiddle );
  QCOMPARE( item->positionWithUnits().x(), 2.0 );
  QCOMPARE( item->positionWithUnits().y(), 2.0 );
  item->setReferencePoint( QgsLayoutItem::UpperRight );
  QCOMPARE( item->positionWithUnits().x(), 3.0 );
  QCOMPARE( item->positionWithUnits().y(), 2.0 );
  item->setReferencePoint( QgsLayoutItem::MiddleLeft );
  QCOMPARE( item->positionWithUnits().x(), 1.0 );
  QCOMPARE( item->positionWithUnits().y(), 4.0 );
  item->setReferencePoint( QgsLayoutItem::Middle );
  QCOMPARE( item->positionWithUnits().x(), 2.0 );
  QCOMPARE( item->positionWithUnits().y(), 4.0 );
  item->setReferencePoint( QgsLayoutItem::MiddleRight );
  QCOMPARE( item->positionWithUnits().x(), 3.0 );
  QCOMPARE( item->positionWithUnits().y(), 4.0 );
  item->setReferencePoint( QgsLayoutItem::LowerLeft );
  QCOMPARE( item->positionWithUnits().x(), 1.0 );
  QCOMPARE( item->positionWithUnits().y(), 6.0 );
  item->setReferencePoint( QgsLayoutItem::LowerMiddle );
  QCOMPARE( item->positionWithUnits().x(), 2.0 );
  QCOMPARE( item->positionWithUnits().y(), 6.0 );
  item->setReferencePoint( QgsLayoutItem::LowerRight );
  QCOMPARE( item->positionWithUnits().x(), 3.0 );
  QCOMPARE( item->positionWithUnits().y(), 6.0 );

  delete item;
  item = new TestItem( mLayout );

  //test that setting item position is done relative to reference point
  mLayout->setUnits( QgsLayoutUnits::Millimeters );
  item->attemptResize( QgsLayoutSize( 2, 4 ) );
  item->setReferencePoint( QgsLayoutItem::UpperLeft );
  item->attemptMove( QgsLayoutPoint( 1, 2 ) );
  QCOMPARE( item->scenePos().x(), 1.0 );
  QCOMPARE( item->scenePos().y(), 2.0 );
  item->setReferencePoint( QgsLayoutItem::UpperMiddle );
  item->attemptMove( QgsLayoutPoint( 1, 2 ) );
  QCOMPARE( item->scenePos().x(), 0.0 );
  QCOMPARE( item->scenePos().y(), 2.0 );
  item->setReferencePoint( QgsLayoutItem::UpperRight );
  item->attemptMove( QgsLayoutPoint( 1, 2 ) );
  QCOMPARE( item->scenePos().x(), -1.0 );
  QCOMPARE( item->scenePos().y(), 2.0 );
  item->setReferencePoint( QgsLayoutItem::MiddleLeft );
  item->attemptMove( QgsLayoutPoint( 1, 2 ) );
  QCOMPARE( item->scenePos().x(), 1.0 );
  QCOMPARE( item->scenePos().y(), 0.0 );
  item->setReferencePoint( QgsLayoutItem::Middle );
  item->attemptMove( QgsLayoutPoint( 1, 2 ) );
  QCOMPARE( item->scenePos().x(), 0.0 );
  QCOMPARE( item->scenePos().y(), 0.0 );
  item->setReferencePoint( QgsLayoutItem::MiddleRight );
  item->attemptMove( QgsLayoutPoint( 1, 2 ) );
  QCOMPARE( item->scenePos().x(), -1.0 );
  QCOMPARE( item->scenePos().y(), 0.0 );
  item->setReferencePoint( QgsLayoutItem::LowerLeft );
  item->attemptMove( QgsLayoutPoint( 1, 2 ) );
  QCOMPARE( item->scenePos().x(), 1.0 );
  QCOMPARE( item->scenePos().y(), -2.0 );
  item->setReferencePoint( QgsLayoutItem::LowerMiddle );
  item->attemptMove( QgsLayoutPoint( 1, 2 ) );
  QCOMPARE( item->scenePos().x(), 0.0 );
  QCOMPARE( item->scenePos().y(), -2.0 );
  item->setReferencePoint( QgsLayoutItem::LowerRight );
  item->attemptMove( QgsLayoutPoint( 1, 2 ) );
  QCOMPARE( item->scenePos().x(), -1.0 );
  QCOMPARE( item->scenePos().y(), -2.0 );

  delete item;
  item = new TestItem( mLayout );

  //test that resizing is done relative to reference point
  item->attemptResize( QgsLayoutSize( 2, 4 ) );
  item->setReferencePoint( QgsLayoutItem::UpperLeft );
  item->attemptMove( QgsLayoutPoint( 1, 2 ) );
  item->attemptResize( QgsLayoutSize( 4, 6 ) );
  QCOMPARE( item->scenePos().x(), 1.0 );
  QCOMPARE( item->scenePos().y(), 2.0 );
  item->setReferencePoint( QgsLayoutItem::UpperMiddle );
  item->attemptResize( QgsLayoutSize( 6, 4 ) );
  QCOMPARE( item->scenePos().x(), 0.0 );
  QCOMPARE( item->scenePos().y(), 2.0 );
  item->setReferencePoint( QgsLayoutItem::UpperRight );
  item->attemptResize( QgsLayoutSize( 4, 6 ) );
  QCOMPARE( item->scenePos().x(), 2.0 );
  QCOMPARE( item->scenePos().y(), 2.0 );
  item->setReferencePoint( QgsLayoutItem::MiddleLeft );
  item->attemptResize( QgsLayoutSize( 6, 4 ) );
  QCOMPARE( item->scenePos().x(), 2.0 );
  QCOMPARE( item->scenePos().y(), 3.0 );
  item->setReferencePoint( QgsLayoutItem::Middle );
  item->attemptResize( QgsLayoutSize( 4, 6 ) );
  QCOMPARE( item->scenePos().x(), 3.0 );
  QCOMPARE( item->scenePos().y(), 2.0 );
  item->setReferencePoint( QgsLayoutItem::MiddleRight );
  item->attemptResize( QgsLayoutSize( 6, 4 ) );
  QCOMPARE( item->scenePos().x(), 1.0 );
  QCOMPARE( item->scenePos().y(), 3.0 );
  item->setReferencePoint( QgsLayoutItem::LowerLeft );
  item->attemptResize( QgsLayoutSize( 4, 6 ) );
  QCOMPARE( item->scenePos().x(), 1.0 );
  QCOMPARE( item->scenePos().y(), 1.0 );
  item->setReferencePoint( QgsLayoutItem::LowerMiddle );
  item->attemptResize( QgsLayoutSize( 6, 4 ) );
  QCOMPARE( item->scenePos().x(), 0.0 );
  QCOMPARE( item->scenePos().y(), 3.0 );
  item->setReferencePoint( QgsLayoutItem::LowerRight );
  item->attemptResize( QgsLayoutSize( 4, 6 ) );
  QCOMPARE( item->scenePos().x(), 2.0 );
  QCOMPARE( item->scenePos().y(), 1.0 );

  delete item;
}

void TestQgsLayoutItem::itemPositionReferencePoint()
{
  TestItem* item = new TestItem( mLayout );
  QPointF result = item->itemPositionAtReferencePoint( QgsLayoutItem::UpperLeft, QSizeF( 2, 4 ) );
  QCOMPARE( result.x(), 0.0 );
  QCOMPARE( result.y(), 0.0 );
  result = item->itemPositionAtReferencePoint( QgsLayoutItem::UpperMiddle, QSizeF( 2, 4 ) );
  QCOMPARE( result.x(), 1.0 );
  QCOMPARE( result.y(), 0.0 );
  result = item->itemPositionAtReferencePoint( QgsLayoutItem::UpperRight, QSizeF( 2, 4 ) );
  QCOMPARE( result.x(), 2.0 );
  QCOMPARE( result.y(), 0.0 );
  result = item->itemPositionAtReferencePoint( QgsLayoutItem::MiddleLeft, QSizeF( 2, 4 ) );
  QCOMPARE( result.x(), 0.0 );
  QCOMPARE( result.y(), 2.0 );
  result = item->itemPositionAtReferencePoint( QgsLayoutItem::Middle, QSizeF( 2, 4 ) );
  QCOMPARE( result.x(), 1.0 );
  QCOMPARE( result.y(), 2.0 );
  result = item->itemPositionAtReferencePoint( QgsLayoutItem::MiddleRight, QSizeF( 2, 4 ) );
  QCOMPARE( result.x(), 2.0 );
  QCOMPARE( result.y(), 2.0 );
  result = item->itemPositionAtReferencePoint( QgsLayoutItem::LowerLeft, QSizeF( 2, 4 ) );
  QCOMPARE( result.x(), 0.0 );
  QCOMPARE( result.y(), 4.0 );
  result = item->itemPositionAtReferencePoint( QgsLayoutItem::LowerMiddle, QSizeF( 2, 4 ) );
  QCOMPARE( result.x(), 1.0 );
  QCOMPARE( result.y(), 4.0 );
  result = item->itemPositionAtReferencePoint( QgsLayoutItem::LowerRight, QSizeF( 2, 4 ) );
  QCOMPARE( result.x(), 2.0 );
  QCOMPARE( result.y(), 4.0 );

  delete item;
}

void TestQgsLayoutItem::adjustPointForReference()
{
  TestItem* item = new TestItem( mLayout );
  QPointF result = item->adjustPointForReferencePosition( QPointF( 5, 7 ), QSizeF( 2, 4 ), QgsLayoutItem::UpperLeft );
  QCOMPARE( result.x(), 5.0 );
  QCOMPARE( result.y(), 7.0 );
  result = item->adjustPointForReferencePosition( QPointF( 5, 7 ), QSizeF( 2, 4 ), QgsLayoutItem::UpperMiddle );
  QCOMPARE( result.x(), 4.0 );
  QCOMPARE( result.y(), 7.0 );
  result = item->adjustPointForReferencePosition( QPointF( 5, 7 ), QSizeF( 2, 4 ), QgsLayoutItem::UpperRight );
  QCOMPARE( result.x(), 3.0 );
  QCOMPARE( result.y(), 7.0 );
  result = item->adjustPointForReferencePosition( QPointF( 5, 7 ), QSizeF( 2, 4 ), QgsLayoutItem::MiddleLeft );
  QCOMPARE( result.x(), 5.0 );
  QCOMPARE( result.y(), 5.0 );
  result = item->adjustPointForReferencePosition( QPointF( 5, 7 ), QSizeF( 2, 4 ), QgsLayoutItem::Middle );
  QCOMPARE( result.x(), 4.0 );
  QCOMPARE( result.y(), 5.0 );
  result = item->adjustPointForReferencePosition( QPointF( 5, 7 ), QSizeF( 2, 4 ), QgsLayoutItem::MiddleRight );
  QCOMPARE( result.x(), 3.0 );
  QCOMPARE( result.y(), 5.0 );
  result = item->adjustPointForReferencePosition( QPointF( 5, 7 ), QSizeF( 2, 4 ), QgsLayoutItem::LowerLeft );
  QCOMPARE( result.x(), 5.0 );
  QCOMPARE( result.y(), 3.0 );
  result = item->adjustPointForReferencePosition( QPointF( 5, 7 ), QSizeF( 2, 4 ), QgsLayoutItem::LowerMiddle );
  QCOMPARE( result.x(), 4.0 );
  QCOMPARE( result.y(), 3.0 );
  result = item->adjustPointForReferencePosition( QPointF( 5, 7 ), QSizeF( 2, 4 ), QgsLayoutItem::LowerRight );
  QCOMPARE( result.x(), 3.0 );
  QCOMPARE( result.y(), 3.0 );

  delete item;
}

void TestQgsLayoutItem::positionAtReferencePoint()
{
  TestItem* item = new TestItem( mLayout );
  item->setPos( 8.0, 6.0 );
  item->setRect( 0.0, 0.0, 4.0, 6.0 );
  QPointF result = item->positionAtReferencePoint( QgsLayoutItem::UpperLeft );
  QCOMPARE( result.x(), 8.0 );
  QCOMPARE( result.y(), 6.0 );
  result = item->positionAtReferencePoint( QgsLayoutItem::UpperMiddle );
  QCOMPARE( result.x(), 10.0 );
  QCOMPARE( result.y(), 6.0 );
  result = item->positionAtReferencePoint( QgsLayoutItem::UpperRight );
  QCOMPARE( result.x(), 12.0 );
  QCOMPARE( result.y(), 6.0 );
  result = item->positionAtReferencePoint( QgsLayoutItem::MiddleLeft );
  QCOMPARE( result.x(), 8.0 );
  QCOMPARE( result.y(), 9.0 );
  result = item->positionAtReferencePoint( QgsLayoutItem::Middle );
  QCOMPARE( result.x(), 10.0 );
  QCOMPARE( result.y(), 9.0 );
  result = item->positionAtReferencePoint( QgsLayoutItem::MiddleRight );
  QCOMPARE( result.x(), 12.0 );
  QCOMPARE( result.y(), 9.0 );
  result = item->positionAtReferencePoint( QgsLayoutItem::LowerLeft );
  QCOMPARE( result.x(), 8.0 );
  QCOMPARE( result.y(), 12.0 );
  result = item->positionAtReferencePoint( QgsLayoutItem::LowerMiddle );
  QCOMPARE( result.x(), 10.0 );
  QCOMPARE( result.y(), 12.0 );
  result = item->positionAtReferencePoint( QgsLayoutItem::LowerRight );
  QCOMPARE( result.x(), 12.0 );
  QCOMPARE( result.y(), 12.0 );

  //test with a rotated item
  item->setItemRotation( 90 );
  result = item->positionAtReferencePoint( QgsLayoutItem::UpperLeft );
  QCOMPARE( result.x(), 13.0 );
  QCOMPARE( result.y(), 7.0 );
  result = item->positionAtReferencePoint( QgsLayoutItem::UpperMiddle );
  QCOMPARE( result.x(), 13.0 );
  QCOMPARE( result.y(), 9.0 );
  result = item->positionAtReferencePoint( QgsLayoutItem::UpperRight );
  QCOMPARE( result.x(), 13.0 );
  QCOMPARE( result.y(), 11.0 );
  result = item->positionAtReferencePoint( QgsLayoutItem::MiddleLeft );
  QCOMPARE( result.x(), 10.0 );
  QCOMPARE( result.y(), 7.0 );
  result = item->positionAtReferencePoint( QgsLayoutItem::Middle );
  QCOMPARE( result.x(), 10.0 );
  QCOMPARE( result.y(), 9.0 );
  result = item->positionAtReferencePoint( QgsLayoutItem::MiddleRight );
  QCOMPARE( result.x(), 10.0 );
  QCOMPARE( result.y(), 11.0 );
  result = item->positionAtReferencePoint( QgsLayoutItem::LowerLeft );
  QCOMPARE( result.x(), 7.0 );
  QCOMPARE( result.y(), 7.0 );
  result = item->positionAtReferencePoint( QgsLayoutItem::LowerMiddle );
  QCOMPARE( result.x(), 7.0 );
  QCOMPARE( result.y(), 9.0 );
  result = item->positionAtReferencePoint( QgsLayoutItem::LowerRight );
  QCOMPARE( result.x(), 7.0 );
  QCOMPARE( result.y(), 11.0 );

  delete item;
}

void TestQgsLayoutItem::fixedSize()
{
  mLayout->setUnits( QgsLayoutUnits::Millimeters );
  FixedSizedItem* item = new FixedSizedItem( mLayout );
  QCOMPARE( item->fixedSize().width(), 2.0 );
  QCOMPARE( item->fixedSize().height(), 4.0 );
  QCOMPARE( item->fixedSize().units(), QgsLayoutUnits::Inches );

  item->setRect( 0, 0, 5.0, 6.0 ); //temporarily set rect to random size
  item->attemptResize( QgsLayoutSize( 7.0, 8.0, QgsLayoutUnits::Points ) );
  //check size matches fixed item size converted to mm
  QVERIFY( qgsDoubleNear( item->rect().width(), 2.0 * 25.4 ) );
  QVERIFY( qgsDoubleNear( item->rect().height(), 4.0 * 25.4 ) );

  //check that setting a fixed size applies this size immediately
  item->updateFixedSize( QgsLayoutSize( 150, 250, QgsLayoutUnits::Millimeters ) );
  QVERIFY( qgsDoubleNear( item->rect().width(), 150.0 ) );
  QVERIFY( qgsDoubleNear( item->rect().height(), 250.0 ) );

  delete item;
}

void TestQgsLayoutItem::minSize()
{
  mLayout->setUnits( QgsLayoutUnits::Millimeters );
  MinSizedItem* item = new MinSizedItem( mLayout );
  QCOMPARE( item->minimumSize().width(), 5.0 );
  QCOMPARE( item->minimumSize().height(), 10.0 );
  QCOMPARE( item->minimumSize().units(), QgsLayoutUnits::Centimeters );

  item->setRect( 0, 0, 9.0, 6.0 ); //temporarily set rect to random size
  //try to resize to less than minimum size
  item->attemptResize( QgsLayoutSize( 1.0, 0.5, QgsLayoutUnits::Points ) );
  //check size matches min item size converted to mm
  QVERIFY( qgsDoubleNear( item->rect().width(), 50.0 ) );
  QVERIFY( qgsDoubleNear( item->rect().height(), 100.0 ) );

  //check that resize to larger than min size works
  item->attemptResize( QgsLayoutSize( 0.1, 0.2, QgsLayoutUnits::Meters ) );
  QVERIFY( qgsDoubleNear( item->rect().width(), 100.0 ) );
  QVERIFY( qgsDoubleNear( item->rect().height(), 200.0 ) );

  //check that setting a minimum size applies this size immediately
  item->updateMinSize( QgsLayoutSize( 150, 250, QgsLayoutUnits::Millimeters ) );
  QVERIFY( qgsDoubleNear( item->rect().width(), 150.0 ) );
  QVERIFY( qgsDoubleNear( item->rect().height(), 250.0 ) );

  //also need check that fixed size trumps min size
  FixedMinSizedItem* fixedMinItem = new FixedMinSizedItem( mLayout );
  QCOMPARE( fixedMinItem->minimumSize().width(), 5.0 );
  QCOMPARE( fixedMinItem->minimumSize().height(), 9.0 );
  QCOMPARE( fixedMinItem->minimumSize().units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( fixedMinItem->fixedSize().width(), 2.0 );
  QCOMPARE( fixedMinItem->fixedSize().height(), 4.0 );
  QCOMPARE( fixedMinItem->fixedSize().units(), QgsLayoutUnits::Centimeters );
  //try to resize to less than minimum size
  fixedMinItem->attemptResize( QgsLayoutSize( 1.0, 0.5, QgsLayoutUnits::Points ) );
  //check size matches fixed item size, not minimum size (converted to mm)
  QVERIFY( qgsDoubleNear( fixedMinItem->rect().width(), 50.0 ) );
  QVERIFY( qgsDoubleNear( fixedMinItem->rect().height(), 90.0 ) );

  delete fixedMinItem;
  delete item;
}

void TestQgsLayoutItem::move()
{
  //move test item, same units as layout
  mLayout->setUnits( QgsLayoutUnits::Millimeters );
  TestItem* item = new TestItem( mLayout );
  item->setRect( 0, 0, 55, 45 );
  item->setPos( 27, 29 );
  item->attemptMove( QgsLayoutPoint( 60.0, 15.0, QgsLayoutUnits::Millimeters ) );
  QCOMPARE( item->rect().width(), 55.0 ); //size should not change
  QCOMPARE( item->rect().height(), 45.0 );
  QCOMPARE( item->scenePos().x(), 60.0 );
  QCOMPARE( item->scenePos().y(), 15.0 );

  //test conversion of units
  mLayout->setUnits( QgsLayoutUnits::Centimeters );
  item->setPos( 100, 200 );
  item->attemptMove( QgsLayoutPoint( 0.30, 0.45, QgsLayoutUnits::Meters ) );
  QCOMPARE( item->scenePos().x(), 30.0 );
  QCOMPARE( item->scenePos().y(), 45.0 );

  //test pixel -> page conversion
  mLayout->setUnits( QgsLayoutUnits::Inches );
  mLayout->setDpi( 100.0 );
  item->setPos( 1, 2 );
  item->attemptMove( QgsLayoutPoint( 140, 280, QgsLayoutUnits::Pixels ) );
  QCOMPARE( item->scenePos().x(), 1.4 );
  QCOMPARE( item->scenePos().y(), 2.8 );
  //changing the dpi should move the item
  mLayout->setDpi( 200.0 );
  QCOMPARE( item->scenePos().x(), 0.7 );
  QCOMPARE( item->scenePos().y(), 1.4 );

  //test page -> pixel conversion
  mLayout->setUnits( QgsLayoutUnits::Pixels );
  mLayout->setDpi( 100.0 );
  item->setPos( 2, 2 );
  item->attemptMove( QgsLayoutPoint( 1, 3, QgsLayoutUnits::Inches ) );
  QCOMPARE( item->scenePos().x(), 100.0 );
  QCOMPARE( item->scenePos().y(), 300.0 );
  //changing dpi results in item move
  mLayout->setDpi( 200.0 );
  QCOMPARE( item->scenePos().x(), 200.0 );
  QCOMPARE( item->scenePos().y(), 600.0 );

  mLayout->setUnits( QgsLayoutUnits::Millimeters );
  delete item;
}

void TestQgsLayoutItem::positionWithUnits()
{
  TestItem* item = new TestItem( mLayout );
  item->attemptMove( QgsLayoutPoint( 60.0, 15.0, QgsLayoutUnits::Millimeters ) );
  QCOMPARE( item->positionWithUnits().x(), 60.0 );
  QCOMPARE( item->positionWithUnits().y(), 15.0 );
  QCOMPARE( item->positionWithUnits().units(), QgsLayoutUnits::Millimeters );
  item->attemptMove( QgsLayoutPoint( 50.0, 100.0, QgsLayoutUnits::Pixels ) );
  QCOMPARE( item->positionWithUnits().x(), 50.0 );
  QCOMPARE( item->positionWithUnits().y(), 100.0 );
  QCOMPARE( item->positionWithUnits().units(), QgsLayoutUnits::Pixels );

  delete item;
}

void TestQgsLayoutItem::sizeWithUnits()
{
  TestItem* item = new TestItem( mLayout );
  item->attemptResize( QgsLayoutSize( 60.0, 15.0, QgsLayoutUnits::Millimeters ) );
  QCOMPARE( item->sizeWithUnits().width(), 60.0 );
  QCOMPARE( item->sizeWithUnits().height(), 15.0 );
  QCOMPARE( item->sizeWithUnits().units(), QgsLayoutUnits::Millimeters );
  item->attemptResize( QgsLayoutSize( 50.0, 100.0, QgsLayoutUnits::Pixels ) );
  QCOMPARE( item->sizeWithUnits().width(), 50.0 );
  QCOMPARE( item->sizeWithUnits().height(), 100.0 );
  QCOMPARE( item->sizeWithUnits().units(), QgsLayoutUnits::Pixels );

  delete item;
}

void TestQgsLayoutItem::dataDefinedPosition()
{
  //test setting data defined position
  TestItem* item = new TestItem( mLayout );
  mLayout->setUnits( QgsLayoutUnits::Millimeters );
  item->attemptMove( QgsLayoutPoint( 6.0, 1.50, QgsLayoutUnits::Centimeters ) );
  item->attemptResize( QgsLayoutSize( 2.0, 4.0, QgsLayoutUnits::Centimeters ) );

  //position x
  item->setDataDefinedProperty( QgsLayoutObject::PositionX, new QgsDataDefined( true, true, QString( "4+7" ) ) );
  item->refreshDataDefinedProperty( QgsLayoutObject::PositionX );
  QCOMPARE( item->positionWithUnits().x(), 11.0 );
  QCOMPARE( item->positionWithUnits().units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( item->scenePos().x(), 110.0 ); //mm

  //position y
  item->setDataDefinedProperty( QgsLayoutObject::PositionY, new QgsDataDefined( true, true, QString( "2+3" ) ) );
  item->refreshDataDefinedProperty( QgsLayoutObject::PositionY );
  QCOMPARE( item->positionWithUnits().y(), 5.0 );
  QCOMPARE( item->positionWithUnits().units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( item->scenePos().y(), 50.0 ); //mm

  //refreshPosition should also respect data defined positioning
  item->setPos( 0, 0 );
  item->setDataDefinedProperty( QgsLayoutObject::PositionX, new QgsDataDefined( true, true, QString( "4+8" ) ) );
  item->setDataDefinedProperty( QgsLayoutObject::PositionY, new QgsDataDefined( true, true, QString( "2+4" ) ) );
  item->refreshItemPosition();
  QCOMPARE( item->positionWithUnits().x(), 12.0 );
  QCOMPARE( item->positionWithUnits().y(), 6.0 );
  QCOMPARE( item->positionWithUnits().units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( item->scenePos().x(), 120.0 ); //mm
  QCOMPARE( item->scenePos().y(), 60.0 ); //mm

  //also check that data defined position overrides when attempting to move
  item->attemptMove( QgsLayoutPoint( 6.0, 1.50, QgsLayoutUnits::Centimeters ) );
  QCOMPARE( item->positionWithUnits().x(), 12.0 );
  QCOMPARE( item->positionWithUnits().y(), 6.0 );
  QCOMPARE( item->positionWithUnits().units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( item->scenePos().x(), 120.0 ); //mm
  QCOMPARE( item->scenePos().y(), 60.0 ); //mm
  //restriction only for x position
  item->setDataDefinedProperty( QgsLayoutObject::PositionX, new QgsDataDefined( true, true, QString( "4+8" ) ) );
  item->setDataDefinedProperty( QgsLayoutObject::PositionY, NULL );
  item->attemptMove( QgsLayoutPoint( 6.0, 1.5, QgsLayoutUnits::Centimeters ) );
  QCOMPARE( item->positionWithUnits().x(), 12.0 );
  QCOMPARE( item->positionWithUnits().y(), 1.5 );
  QCOMPARE( item->positionWithUnits().units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( item->scenePos().x(), 120.0 ); //mm
  QCOMPARE( item->scenePos().y(), 15.0 ); //mm
  //restriction only for y position
  item->setDataDefinedProperty( QgsLayoutObject::PositionX, NULL );
  item->setDataDefinedProperty( QgsLayoutObject::PositionY, new QgsDataDefined( true, true, QString( "2+4" ) ) );
  item->attemptMove( QgsLayoutPoint( 7.0, 1.5, QgsLayoutUnits::Centimeters ) );
  QCOMPARE( item->positionWithUnits().x(), 7.0 );
  QCOMPARE( item->positionWithUnits().y(), 6.0 );
  QCOMPARE( item->positionWithUnits().units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( item->scenePos().x(), 70.0 ); //mm
  QCOMPARE( item->scenePos().y(), 60.0 ); //mm

  //check change of units should apply to data defined position
  item->setDataDefinedProperty( QgsLayoutObject::PositionX, new QgsDataDefined( true, true, QString( "4+8" ) ) );
  item->setDataDefinedProperty( QgsLayoutObject::PositionY, new QgsDataDefined( true, true, QString( "2+4" ) ) );
  //first set to same as existing position, but with different units
  item->attemptMove( QgsLayoutPoint( 120.0, 60.0, QgsLayoutUnits::Millimeters ) );
  //data defined position should utilise new units
  QCOMPARE( item->positionWithUnits().x(), 12.0 ); //mm
  QCOMPARE( item->positionWithUnits().y(), 6.0 ); //mm
  QCOMPARE( item->positionWithUnits().units(), QgsLayoutUnits::Millimeters );
  QCOMPARE( item->scenePos().x(), 12.0 ); //mm
  QCOMPARE( item->scenePos().y(), 6.0 ); //mm

  //test that data defined position applies to item's reference point
  item->attemptMove( QgsLayoutPoint( 12.0, 6.0, QgsLayoutUnits::Centimeters ) );
  item->setReferencePoint( QgsLayoutItem::LowerRight );
  QCOMPARE( item->positionWithUnits().x(), 12.0 ); //cm
  QCOMPARE( item->positionWithUnits().y(), 6.0 ); //cm
  QCOMPARE( item->positionWithUnits().units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( item->scenePos().x(), 100.0 ); //mm
  QCOMPARE( item->scenePos().y(), 20.0 ); //mm

  //also check setting data defined position AFTER setting reference point
  item->setPos( 0, 0 );
  item->setDataDefinedProperty( QgsLayoutObject::PositionX, new QgsDataDefined( true, true, QString( "6+10" ) ) );
  item->setDataDefinedProperty( QgsLayoutObject::PositionY, new QgsDataDefined( true, true, QString( "2+6" ) ) );
  item->refreshItemPosition();
  QCOMPARE( item->positionWithUnits().x(), 16.0 ); //cm
  QCOMPARE( item->positionWithUnits().y(), 8.0 ); //cm
  QCOMPARE( item->positionWithUnits().units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( item->scenePos().x(), 140.0 ); //mm
  QCOMPARE( item->scenePos().y(), 40.0 ); //mm

  delete item;
}

void TestQgsLayoutItem::dataDefinedSize()
{
  //test setting data defined size
  TestItem* item = new TestItem( mLayout );
  mLayout->setUnits( QgsLayoutUnits::Millimeters );
  item->attemptMove( QgsLayoutPoint( 6.0, 1.50, QgsLayoutUnits::Centimeters ) );
  item->attemptResize( QgsLayoutSize( 2.0, 4.0, QgsLayoutUnits::Centimeters ) );

  //width
  item->setDataDefinedProperty( QgsLayoutObject::ItemWidth, new QgsDataDefined( true, true, QString( "4+7" ) ) );
  item->refreshDataDefinedProperty( QgsLayoutObject::ItemWidth );
  QCOMPARE( item->sizeWithUnits().width(), 11.0 );
  QCOMPARE( item->sizeWithUnits().units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( item->rect().width(), 110.0 ); //mm

  //height
  item->setDataDefinedProperty( QgsLayoutObject::ItemHeight, new QgsDataDefined( true, true, QString( "2+3" ) ) );
  item->refreshDataDefinedProperty( QgsLayoutObject::ItemHeight );
  QCOMPARE( item->sizeWithUnits().height(), 5.0 );
  QCOMPARE( item->sizeWithUnits().units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( item->rect().height(), 50.0 ); //mm

  //refreshSize should also respect data defined size
  item->setRect( 0.0, 0.0, 9.0, 8.0 );
  item->setDataDefinedProperty( QgsLayoutObject::ItemWidth, new QgsDataDefined( true, true, QString( "4+8" ) ) );
  item->setDataDefinedProperty( QgsLayoutObject::ItemHeight, new QgsDataDefined( true, true, QString( "2+4" ) ) );
  item->refreshItemSize();
  QCOMPARE( item->sizeWithUnits().width(), 12.0 );
  QCOMPARE( item->sizeWithUnits().height(), 6.0 );
  QCOMPARE( item->sizeWithUnits().units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( item->rect().width(), 120.0 ); //mm
  QCOMPARE( item->rect().height(), 60.0 ); //mm

  //also check that data defined size overrides when attempting to resize
  item->attemptResize( QgsLayoutSize( 6.0, 1.50, QgsLayoutUnits::Centimeters ) );
  QCOMPARE( item->sizeWithUnits().width(), 12.0 );
  QCOMPARE( item->sizeWithUnits().height(), 6.0 );
  QCOMPARE( item->sizeWithUnits().units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( item->rect().width(), 120.0 ); //mm
  QCOMPARE( item->rect().height(), 60.0 ); //mm
  //restriction only for width
  item->setDataDefinedProperty( QgsLayoutObject::ItemWidth, new QgsDataDefined( true, true, QString( "4+8" ) ) );
  item->setDataDefinedProperty( QgsLayoutObject::ItemHeight, NULL );
  item->attemptResize( QgsLayoutSize( 6.0, 1.50, QgsLayoutUnits::Centimeters ) );
  QCOMPARE( item->sizeWithUnits().width(), 12.0 );
  QCOMPARE( item->sizeWithUnits().height(), 1.5 );
  QCOMPARE( item->sizeWithUnits().units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( item->rect().width(), 120.0 ); //mm
  QCOMPARE( item->rect().height(), 15.0 ); //mm
  //restriction only for y position
  item->setDataDefinedProperty( QgsLayoutObject::ItemWidth, NULL );
  item->setDataDefinedProperty( QgsLayoutObject::ItemHeight, new QgsDataDefined( true, true, QString( "2+4" ) ) );
  item->attemptResize( QgsLayoutSize( 7.0, 1.50, QgsLayoutUnits::Centimeters ) );
  QCOMPARE( item->sizeWithUnits().width(), 7.0 );
  QCOMPARE( item->sizeWithUnits().height(), 6.0 );
  QCOMPARE( item->sizeWithUnits().units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( item->rect().width(), 70.0 ); //mm
  QCOMPARE( item->rect().height(), 60.0 ); //mm

  //check change of units should apply to data defined size
  item->setDataDefinedProperty( QgsLayoutObject::ItemWidth, new QgsDataDefined( true, true, QString( "4+8" ) ) );
  item->setDataDefinedProperty( QgsLayoutObject::ItemHeight, new QgsDataDefined( true, true, QString( "2+4" ) ) );
  //first set to same as existing size, but with different units
  item->attemptResize( QgsLayoutSize( 120.0, 60.0, QgsLayoutUnits::Millimeters ) );
  //data defined size should utilise new units
  QCOMPARE( item->sizeWithUnits().width(), 12.0 ); //mm
  QCOMPARE( item->sizeWithUnits().height(), 6.0 ); //mm
  QCOMPARE( item->sizeWithUnits().units(), QgsLayoutUnits::Millimeters );
  QCOMPARE( item->rect().width(), 12.0 ); //mm
  QCOMPARE( item->rect().height(), 6.0 ); //mm

  //test that data defined size applies to item's reference point
  item->setDataDefinedProperty( QgsLayoutObject::ItemWidth, NULL );
  item->setDataDefinedProperty( QgsLayoutObject::ItemHeight, NULL );
  item->attemptResize( QgsLayoutSize( 10.0, 5.0, QgsLayoutUnits::Millimeters ) );
  item->attemptMove( QgsLayoutPoint( 20.0, 10.0, QgsLayoutUnits::Millimeters ) );
  item->setDataDefinedProperty( QgsLayoutObject::ItemWidth, new QgsDataDefined( true, true, QString( "5" ) ) );
  item->setDataDefinedProperty( QgsLayoutObject::ItemHeight, new QgsDataDefined( true, true, QString( "6" ) ) );
  item->setReferencePoint( QgsLayoutItem::LowerRight );
  item->refreshItemSize();
  QCOMPARE( item->scenePos().x(), 25.0 ); //mm
  QCOMPARE( item->scenePos().y(), 9.0 ); //mm

  //test that data defined size applied after setting item's reference point respects reference
  item->setDataDefinedProperty( QgsLayoutObject::ItemWidth, NULL );
  item->setDataDefinedProperty( QgsLayoutObject::ItemHeight, NULL );
  item->setReferencePoint( QgsLayoutItem::UpperLeft );
  item->attemptResize( QgsLayoutSize( 10.0, 5.0, QgsLayoutUnits::Millimeters ) );
  item->attemptMove( QgsLayoutPoint( 20.0, 10.0, QgsLayoutUnits::Millimeters ) );
  item->setReferencePoint( QgsLayoutItem::LowerRight );
  item->setDataDefinedProperty( QgsLayoutObject::ItemWidth, new QgsDataDefined( true, true, QString( "7" ) ) );
  item->setDataDefinedProperty( QgsLayoutObject::ItemHeight, new QgsDataDefined( true, true, QString( "9" ) ) );
  item->refreshItemSize();
  QCOMPARE( item->scenePos().x(), 23.0 ); //mm
  QCOMPARE( item->scenePos().y(), 6.0 ); //mm

  delete item;
}

void TestQgsLayoutItem::combinedDataDefinedPositionAndSize()
{
  //test setting data defined size
  TestItem* item = new TestItem( mLayout );
  mLayout->setUnits( QgsLayoutUnits::Millimeters );
  item->attemptMove( QgsLayoutPoint( 6.0, 1.50, QgsLayoutUnits::Centimeters ) );
  item->attemptResize( QgsLayoutSize( 2.0, 4.0, QgsLayoutUnits::Centimeters ) );

  //test item with all of data defined x, y, width, height set
  item->setDataDefinedProperty( QgsLayoutObject::PositionX, new QgsDataDefined( true, true, QString( "4+7" ) ) );
  item->setDataDefinedProperty( QgsLayoutObject::PositionY, new QgsDataDefined( true, true, QString( "2+3" ) ) );
  item->setDataDefinedProperty( QgsLayoutObject::ItemWidth, new QgsDataDefined( true, true, QString( "4+9" ) ) );
  item->setDataDefinedProperty( QgsLayoutObject::ItemHeight, new QgsDataDefined( true, true, QString( "2+4" ) ) );
  item->refreshDataDefinedProperty( QgsLayoutObject::AllProperties );
  QCOMPARE( item->positionWithUnits().x(), 11.0 );
  QCOMPARE( item->positionWithUnits().y(), 5.0 );
  QCOMPARE( item->positionWithUnits().units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( item->sizeWithUnits().width(), 13.0 );
  QCOMPARE( item->sizeWithUnits().height(), 6.0 );
  QCOMPARE( item->sizeWithUnits().units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( item->scenePos().x(), 110.0 ); //mm
  QCOMPARE( item->scenePos().y(), 50.0 ); //mm
  QCOMPARE( item->rect().width(), 130.0 ); //mm
  QCOMPARE( item->rect().height(), 60.0 ); //mm

  //also try with reference point set
  item->setReferencePoint( QgsLayoutItem::Middle );
  item->setDataDefinedProperty( QgsLayoutObject::PositionX, new QgsDataDefined( true, true, QString( "4+8" ) ) );
  item->setDataDefinedProperty( QgsLayoutObject::PositionY, new QgsDataDefined( true, true, QString( "2+4" ) ) );
  item->setDataDefinedProperty( QgsLayoutObject::ItemWidth, new QgsDataDefined( true, true, QString( "3+7" ) ) );
  item->setDataDefinedProperty( QgsLayoutObject::ItemHeight, new QgsDataDefined( true, true, QString( "1+3" ) ) );
  item->refreshDataDefinedProperty( QgsLayoutObject::AllProperties );
  QCOMPARE( item->positionWithUnits().x(), 12.0 );
  QCOMPARE( item->positionWithUnits().y(), 6.0 );
  QCOMPARE( item->positionWithUnits().units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( item->sizeWithUnits().width(), 10.0 );
  QCOMPARE( item->sizeWithUnits().height(), 4.0 );
  QCOMPARE( item->sizeWithUnits().units(), QgsLayoutUnits::Centimeters );
  QCOMPARE( item->scenePos().x(), 70.0 ); //mm
  QCOMPARE( item->scenePos().y(), 40.0 ); //mm
  QCOMPARE( item->rect().width(), 100.0 ); //mm
  QCOMPARE( item->rect().height(), 40.0 ); //mm

  delete item;
}

void TestQgsLayoutItem::rotation()
{
  TestItem* item = new TestItem( mLayout );
  mLayout->setUnits( QgsLayoutUnits::Millimeters );
  item->setPos( 6.0, 10.0 );
  item->setRect( 0.0, 0.0, 10.0, 8.0 );

  item->setItemRotation( 90.0 );
  QCOMPARE( item->itemRotation(), 90.0 );
  QCOMPARE( item->rotation(), 90.0 );
  QRectF bounds = item->sceneBoundingRect();
  QCOMPARE( bounds.left(), 7.0 );
  QCOMPARE( bounds.right(), 15.0 );
  QCOMPARE( bounds.top(), 9.0 );
  QCOMPARE( bounds.bottom(), 19.0 );

  //check that negative angles are preserved as negative
  item->setItemRotation( -90.0 );
  QCOMPARE( item->itemRotation(), -90.0 );
  QCOMPARE( item->rotation(), -90.0 );
  bounds = item->sceneBoundingRect();
  QCOMPARE( bounds.width(), 8.0 );
  QCOMPARE( bounds.height(), 10.0 );

  //check that rotating changes stored item position for reference point
  item->setItemRotation( 0.0 );
  item->attemptMove( QgsLayoutPoint( 5.0, 8.0 ) );
  item->attemptResize( QgsLayoutSize( 10.0, 6.0 ) );
  item->setItemRotation( 90.0 );
  QCOMPARE( item->positionWithUnits().x(), 13.0 );
  QCOMPARE( item->positionWithUnits().y(), 6.0 );

  //setting item position (for reference point) respects rotation
  item->attemptMove( QgsLayoutPoint( 10.0, 8.0 ) );
  QCOMPARE( item->scenePos().x(), 10.0 );
  QCOMPARE( item->scenePos().y(), 8.0 );
  QRectF p = item->sceneBoundingRect();
  qDebug() << p.left();
  QCOMPARE( item->sceneBoundingRect().left(), 4.0 );
  QCOMPARE( item->sceneBoundingRect().right(), 10.0 );
  QCOMPARE( item->sceneBoundingRect().top(), 8.0 );
  QCOMPARE( item->sceneBoundingRect().bottom(), 18.0 );

  //TODO also changing size?


  //data defined rotation
  item->setItemRotation( 0.0 );
  item->attemptMove( QgsLayoutPoint( 5.0, 8.0 ) );
  item->attemptResize( QgsLayoutSize( 10.0, 6.0 ) );
  item->setDataDefinedProperty( QgsLayoutObject::ItemRotation, new QgsDataDefined( true, true, QString( "90" ) ) );
  item->refreshDataDefinedProperty( QgsLayoutObject::ItemRotation );
  QCOMPARE( item->itemRotation(), 90.0 );
  //also check when refreshing all properties
  item->setDataDefinedProperty( QgsLayoutObject::ItemRotation, new QgsDataDefined( true, true, QString( "45" ) ) );
  item->refreshDataDefinedProperty( QgsLayoutObject::AllProperties );
  QCOMPARE( item->itemRotation(), 45.0 );

  delete item;

  //render check
  item = new TestItem( mLayout );
  item->setItemRotation( 0.0 );
  item->setPos( 100, 150 );
  item->setRect( 0, 0, 200, 100 );
  mLayout->addItem( item );
  item->setItemRotation( 45 );
  mLayout->setSceneRect( 0, 0, 400, 400 );
  mLayout->context()->setFlag( QgsLayoutContext::Debug, true );
  QImage image( mLayout->sceneRect().size().toSize(), QImage::Format_ARGB32 );
  image.fill( 0 );
  QPainter painter( &image );
  mLayout->render( &painter );
  painter.end();

  bool result = renderCheck( "layoutitem_rotation", image, 0 );
  mLayout->context()->setFlag( QgsLayoutContext::Debug, false );
  mLayout->removeItem( item );
  delete item;
  QVERIFY( result );
}

//TODO rotation tests:
//restoring item from xml respects rotation/position
//rotate item around layout point


void TestQgsLayoutItem::writeXml()
{
  QDomImplementation DomImplementation;
  QDomDocumentType documentType =
    DomImplementation.createDocumentType(
      "qgis", "http://mrcc.com/qgis.dtd", "SYSTEM" );
  QDomDocument doc( documentType );

  TestItem* item = new TestItem( mLayout );
  QDomElement element = item->toXmlElement( doc );

  QCOMPARE( element.nodeName(), QString( "LayoutItem" ) );
  QCOMPARE( element.attribute( "type", "" ), item->stringType() );
  QCOMPARE( element.attribute( "testAttribute", "" ), QString( "testValue" ) );

  //check that element has an object node
  QDomNodeList objectNodeList = element.elementsByTagName( "LayoutObject" );
  QCOMPARE( objectNodeList.count(), 1 );

  delete item;
}

void TestQgsLayoutItem::readXml()
{
  QDomImplementation DomImplementation;
  QDomDocumentType documentType =
    DomImplementation.createDocumentType(
      "qgis", "http://mrcc.com/qgis.dtd", "SYSTEM" );
  QDomDocument doc( documentType );

  TestItem* item = new TestItem( mLayout );

  //try reading bad elements
  QDomElement badElement = doc.createElement( "bad" );
  QDomElement noNode;
  QVERIFY( !item->readXMLElement( badElement, doc ) );
  QVERIFY( !item->readXMLElement( noNode, doc ) );

  //element with wrong type
  QDomElement wrongType = doc.createElement( "LayoutItem" );
  wrongType.setAttribute( QString( "type" ), "bad" );
  QVERIFY( !item->readXMLElement( wrongType, doc ) );

  //try good element
  QDomElement goodElement = doc.createElement( "LayoutItem" );
  goodElement.setAttribute( QString( "type" ), "testItem" );
  QVERIFY( item->readXMLElement( goodElement, doc ) );
  delete item;
}

void TestQgsLayoutItem::writeReadXmlProperties()
{
  TestItem* original = new TestItem( mLayout );

  QgsDataDefined* ddOrig = new QgsDataDefined( true, true, "50" );
  original->setDataDefinedProperty( QgsLayoutItem::TestProperty, ddOrig );
  original->setReferencePoint( QgsLayoutItem::Middle );
  original->attemptResize( QgsLayoutSize( 6, 8, QgsLayoutUnits::Centimeters ) );
  original->attemptMove( QgsLayoutPoint( 0.05, 0.09, QgsLayoutUnits::Meters ) );
  original->setItemRotation( 45.0 );
  original->setId( QString( "test" ) );

  QgsLayoutItem* copy = createCopyViaXml( original );

  QCOMPARE( copy->uuid(), original->uuid() );
  QCOMPARE( copy->id(), original->id() );
  QgsDataDefined* ddResult = copy->dataDefinedProperty( QgsLayoutItem::TestProperty );
  QCOMPARE( *ddResult, *ddOrig );
  QCOMPARE( copy->referencePoint(), original->referencePoint() );
  QCOMPARE( copy->sizeWithUnits(), original->sizeWithUnits() );
  QCOMPARE( copy->positionWithUnits(), original->positionWithUnits() );
  QCOMPARE( copy->itemRotation(), original->itemRotation() );

  delete copy;
  delete original;
}

QgsLayoutItem *TestQgsLayoutItem::createCopyViaXml( QgsLayoutItem *original )
{
  //save original item to xml
  QDomImplementation DomImplementation;
  QDomDocumentType documentType =
    DomImplementation.createDocumentType(
      "qgis", "http://mrcc.com/qgis.dtd", "SYSTEM" );
  QDomDocument doc( documentType );
  QDomElement element = original->toXmlElement( doc );

  //create new item and restore settings from xml
  TestItem* copy = new TestItem( mLayout );
  copy->readXMLElement( element, doc );

  return copy;
}

bool TestQgsLayoutItem::renderCheck( QString testName, QImage &image, int mismatchCount )
{
  mReport += "<h2>" + testName + "</h2>\n";
  QString myTmpDir = QDir::tempPath() + QDir::separator();
  QString myFileName = myTmpDir + testName + ".png";
  image.save( myFileName, "PNG" );
  QgsRenderChecker myChecker;
  myChecker.setControlName( "expected_" + testName );
  myChecker.setRenderedImage( myFileName );
  bool myResultFlag = myChecker.compareImages( testName, mismatchCount );
  mReport += myChecker.report();
  return myResultFlag;
}

QTEST_MAIN( TestQgsLayoutItem )
#include "testqgslayoutitem.moc"
