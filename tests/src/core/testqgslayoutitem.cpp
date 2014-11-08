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
