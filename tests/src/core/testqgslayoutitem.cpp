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
#include "qgscompositionchecker.h"
#include <QObject>
#include <QtTest>
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
    void shouldDrawDebug();
    void shouldDrawAntialiased();
    void preparePainter();
    void debugRect();
    void draw();
    void resize();
    void referencePoint();
    void adjustPointForReference();
    void fixedSize();
    void minSize();
    void move();
    void positionWithUnits();

  private:

    //simple item for testing, since some methods in QgsLayoutItem are pure virtual
    class TestItem : public QgsLayoutItem
    {
      public:

        TestItem( QgsLayout* layout ) : QgsLayoutItem( layout ) {}
        ~TestItem() {}

        //implement pure virtual methods
        int type() const { return QgsLayoutItemRegistry::LayoutItem + 101; }
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

void TestQgsLayoutItem::shouldDrawDebug()
{
  TestItem* item = new TestItem( mLayout );
  mLayout->setFlag( QgsLayout::Debug, true );
  QVERIFY( item->shouldDrawDebugRect() );
  mLayout->setFlag( QgsLayout::Debug, false );
  QVERIFY( !item->shouldDrawDebugRect() );
  delete item;
}

void TestQgsLayoutItem::shouldDrawAntialiased()
{
  TestItem* item = new TestItem( mLayout );
  mLayout->setFlag( QgsLayout::Antialiasing, false );
  QVERIFY( !item->shouldDrawAntialiased() );
  mLayout->setFlag( QgsLayout::Antialiasing, true );
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
  mLayout->setFlag( QgsLayout::Antialiasing, false );
  item->preparePainter( &painter );
  QVERIFY( !( painter.renderHints() & QPainter::Antialiasing ) );
  mLayout->setFlag( QgsLayout::Antialiasing, true );
  item->preparePainter( &painter );
  QVERIFY( painter.renderHints() & QPainter::Antialiasing );
  delete item;
}

void TestQgsLayoutItem::debugRect()
{
  TestItem* item = new TestItem( mLayout );
  mLayout->addItem( item );
  item->setPos( 100, 100 );
  item->setRect( 0, 0, 200, 200 );
  mLayout->setSceneRect( 0, 0, 400, 400 );
  mLayout->setFlag( QgsLayout::Debug, true );
  QImage image( mLayout->sceneRect().size().toSize(), QImage::Format_ARGB32 );
  image.fill( 0 );
  QPainter painter( &image );
  mLayout->render( &painter );
  painter.end();

  bool result = renderCheck( "layoutitem_debugrect", image, 0 );
  mLayout->setFlag( QgsLayout::Debug, false );
  QVERIFY( result );
}

void TestQgsLayoutItem::draw()
{
  TestItem* item = new TestItem( mLayout );
  mLayout->addItem( item );
  item->setPos( 100, 100 );
  item->setRect( 0, 0, 200, 200 );
  mLayout->setSceneRect( 0, 0, 400, 400 );
  mLayout->setFlag( QgsLayout::Antialiasing, false ); //disable antialiasing to limit cross platform differences
  QImage image( mLayout->sceneRect().size().toSize(), QImage::Format_ARGB32 );
  image.fill( 0 );
  QPainter painter( &image );
  mLayout->render( &painter );
  painter.end();
  mLayout->setFlag( QgsLayout::Antialiasing, true );

  bool result = renderCheck( "layoutitem_draw", image, 0 );
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
  QCOMPARE( item->pos().x(), 27.0 ); //item should not move
  QCOMPARE( item->pos().y(), 29.0 );

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
  QCOMPARE( item->pos().x(), 1.0 );
  QCOMPARE( item->pos().y(), 2.0 );
  item->setReferencePoint( QgsLayoutItem::UpperMiddle );
  item->attemptMove( QgsLayoutPoint( 1, 2 ) );
  QCOMPARE( item->pos().x(), 0.0 );
  QCOMPARE( item->pos().y(), 2.0 );
  item->setReferencePoint( QgsLayoutItem::UpperRight );
  item->attemptMove( QgsLayoutPoint( 1, 2 ) );
  QCOMPARE( item->pos().x(), -1.0 );
  QCOMPARE( item->pos().y(), 2.0 );
  item->setReferencePoint( QgsLayoutItem::MiddleLeft );
  item->attemptMove( QgsLayoutPoint( 1, 2 ) );
  QCOMPARE( item->pos().x(), 1.0 );
  QCOMPARE( item->pos().y(), 0.0 );
  item->setReferencePoint( QgsLayoutItem::Middle );
  item->attemptMove( QgsLayoutPoint( 1, 2 ) );
  QCOMPARE( item->pos().x(), 0.0 );
  QCOMPARE( item->pos().y(), 0.0 );
  item->setReferencePoint( QgsLayoutItem::MiddleRight );
  item->attemptMove( QgsLayoutPoint( 1, 2 ) );
  QCOMPARE( item->pos().x(), -1.0 );
  QCOMPARE( item->pos().y(), 0.0 );
  item->setReferencePoint( QgsLayoutItem::LowerLeft );
  item->attemptMove( QgsLayoutPoint( 1, 2 ) );
  QCOMPARE( item->pos().x(), 1.0 );
  QCOMPARE( item->pos().y(), -2.0 );
  item->setReferencePoint( QgsLayoutItem::LowerMiddle );
  item->attemptMove( QgsLayoutPoint( 1, 2 ) );
  QCOMPARE( item->pos().x(), 0.0 );
  QCOMPARE( item->pos().y(), -2.0 );
  item->setReferencePoint( QgsLayoutItem::LowerRight );
  item->attemptMove( QgsLayoutPoint( 1, 2 ) );
  QCOMPARE( item->pos().x(), -1.0 );
  QCOMPARE( item->pos().y(), -2.0 );

  delete item;
  item = new TestItem( mLayout );

  //test that resizing is done relative to reference point
  item->attemptResize( QgsLayoutSize( 2, 4 ) );
  item->setReferencePoint( QgsLayoutItem::UpperLeft );
  item->attemptMove( QgsLayoutPoint( 1, 2 ) );
  item->attemptResize( QgsLayoutSize( 4, 6 ) );
  QCOMPARE( item->pos().x(), 1.0 );
  QCOMPARE( item->pos().y(), 2.0 );
  item->setReferencePoint( QgsLayoutItem::UpperMiddle );
  item->attemptResize( QgsLayoutSize( 6, 4 ) );
  QCOMPARE( item->pos().x(), 0.0 );
  QCOMPARE( item->pos().y(), 2.0 );
  item->setReferencePoint( QgsLayoutItem::UpperRight );
  item->attemptResize( QgsLayoutSize( 4, 6 ) );
  QCOMPARE( item->pos().x(), 2.0 );
  QCOMPARE( item->pos().y(), 2.0 );
  item->setReferencePoint( QgsLayoutItem::MiddleLeft );
  item->attemptResize( QgsLayoutSize( 6, 4 ) );
  QCOMPARE( item->pos().x(), 2.0 );
  QCOMPARE( item->pos().y(), 3.0 );
  item->setReferencePoint( QgsLayoutItem::Middle );
  item->attemptResize( QgsLayoutSize( 4, 6 ) );
  QCOMPARE( item->pos().x(), 3.0 );
  QCOMPARE( item->pos().y(), 2.0 );
  item->setReferencePoint( QgsLayoutItem::MiddleRight );
  item->attemptResize( QgsLayoutSize( 6, 4 ) );
  QCOMPARE( item->pos().x(), 1.0 );
  QCOMPARE( item->pos().y(), 3.0 );
  item->setReferencePoint( QgsLayoutItem::LowerLeft );
  item->attemptResize( QgsLayoutSize( 4, 6 ) );
  QCOMPARE( item->pos().x(), 1.0 );
  QCOMPARE( item->pos().y(), 1.0 );
  item->setReferencePoint( QgsLayoutItem::LowerMiddle );
  item->attemptResize( QgsLayoutSize( 6, 4 ) );
  QCOMPARE( item->pos().x(), 0.0 );
  QCOMPARE( item->pos().y(), 3.0 );
  item->setReferencePoint( QgsLayoutItem::LowerRight );
  item->attemptResize( QgsLayoutSize( 4, 6 ) );
  QCOMPARE( item->pos().x(), 2.0 );
  QCOMPARE( item->pos().y(), 1.0 );

}

void TestQgsLayoutItem::adjustPointForReference()
{
  TestItem* item = new TestItem( mLayout );
  item->setReferencePoint( QgsLayoutItem::UpperLeft );
  QPointF result = item->adjustPointForReferencePosition( QPointF( 5, 7 ), QSizeF( 2, 4 ) );
  QCOMPARE( result.x(), 5.0 );
  QCOMPARE( result.y(), 7.0 );
  item->setReferencePoint( QgsLayoutItem::UpperMiddle );
  result = item->adjustPointForReferencePosition( QPointF( 5, 7 ), QSizeF( 2, 4 ) );
  QCOMPARE( result.x(), 4.0 );
  QCOMPARE( result.y(), 7.0 );
  item->setReferencePoint( QgsLayoutItem::UpperRight );
  result = item->adjustPointForReferencePosition( QPointF( 5, 7 ), QSizeF( 2, 4 ) );
  QCOMPARE( result.x(), 3.0 );
  QCOMPARE( result.y(), 7.0 );
  item->setReferencePoint( QgsLayoutItem::MiddleLeft );
  result = item->adjustPointForReferencePosition( QPointF( 5, 7 ), QSizeF( 2, 4 ) );
  QCOMPARE( result.x(), 5.0 );
  QCOMPARE( result.y(), 5.0 );
  item->setReferencePoint( QgsLayoutItem::Middle );
  result = item->adjustPointForReferencePosition( QPointF( 5, 7 ), QSizeF( 2, 4 ) );
  QCOMPARE( result.x(), 4.0 );
  QCOMPARE( result.y(), 5.0 );
  item->setReferencePoint( QgsLayoutItem::MiddleRight );
  result = item->adjustPointForReferencePosition( QPointF( 5, 7 ), QSizeF( 2, 4 ) );
  QCOMPARE( result.x(), 3.0 );
  QCOMPARE( result.y(), 5.0 );
  item->setReferencePoint( QgsLayoutItem::LowerLeft );
  result = item->adjustPointForReferencePosition( QPointF( 5, 7 ), QSizeF( 2, 4 ) );
  QCOMPARE( result.x(), 5.0 );
  QCOMPARE( result.y(), 3.0 );
  item->setReferencePoint( QgsLayoutItem::LowerMiddle );
  result = item->adjustPointForReferencePosition( QPointF( 5, 7 ), QSizeF( 2, 4 ) );
  QCOMPARE( result.x(), 4.0 );
  QCOMPARE( result.y(), 3.0 );
  item->setReferencePoint( QgsLayoutItem::LowerRight );
  result = item->adjustPointForReferencePosition( QPointF( 5, 7 ), QSizeF( 2, 4 ) );
  QCOMPARE( result.x(), 3.0 );
  QCOMPARE( result.y(), 3.0 );
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
  QCOMPARE( item->pos().x(), 60.0 );
  QCOMPARE( item->pos().y(), 15.0 );

  //test conversion of units
  mLayout->setUnits( QgsLayoutUnits::Centimeters );
  item->setPos( 100, 200 );
  item->attemptMove( QgsLayoutPoint( 0.30, 0.45, QgsLayoutUnits::Meters ) );
  QCOMPARE( item->pos().x(), 30.0 );
  QCOMPARE( item->pos().y(), 45.0 );

  //test pixel -> page conversion
  mLayout->setUnits( QgsLayoutUnits::Inches );
  mLayout->setDpi( 100.0 );
  item->setPos( 1, 2 );
  item->attemptMove( QgsLayoutPoint( 140, 280, QgsLayoutUnits::Pixels ) );
  QCOMPARE( item->pos().x(), 1.4 );
  QCOMPARE( item->pos().y(), 2.8 );
  //changing the dpi should move the item
  mLayout->setDpi( 200.0 );
  QCOMPARE( item->pos().x(), 0.7 );
  QCOMPARE( item->pos().y(), 1.4 );

  //test page -> pixel conversion
  mLayout->setUnits( QgsLayoutUnits::Pixels );
  mLayout->setDpi( 100.0 );
  item->setPos( 2, 2 );
  item->attemptMove( QgsLayoutPoint( 1, 3, QgsLayoutUnits::Inches ) );
  QCOMPARE( item->pos().x(), 100.0 );
  QCOMPARE( item->pos().y(), 300.0 );
  //changing dpi results in item move
  mLayout->setDpi( 200.0 );
  QCOMPARE( item->pos().x(), 200.0 );
  QCOMPARE( item->pos().y(), 600.0 );

  mLayout->setUnits( QgsLayoutUnits::Millimeters );
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
