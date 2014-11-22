/***************************************************************************
                         testqgslayoutrectitem.cpp
                         -------------------------
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

#include "qgslayoutrectitem.h"
#include "qgslayout.h"
#include "qgscompositionchecker.h"
#include <QObject>
#include <QtTest/QtTest>
#include <QPainter>
#include <QImage>

class TestQgsLayoutRectItem: public QObject
{
    Q_OBJECT;
  private slots:
    void initTestCase();// will be called before the first testfunction is executed.
    void cleanupTestCase();// will be called after the last testfunction was executed.
    void init();// will be called before each testfunction is executed.
    void cleanup();// will be called after every testfunction.
    void creation(); //test creation of QgsLayoutItem

  private:

    //simple item for testing, since some methods in QgsLayoutItem are pure virtual
    class TestItem : public QgsLayoutRectItem
    {
      public:

        TestItem( QgsLayout* layout ) : QgsLayoutRectItem( layout ) {}
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

void TestQgsLayoutRectItem::initTestCase()
{
  mLayout = new QgsLayout();
  mReport = "<h1>Layout Rect Item Tests</h1>\n";
}

void TestQgsLayoutRectItem::cleanupTestCase()
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

void TestQgsLayoutRectItem::init()
{

}

void TestQgsLayoutRectItem::cleanup()
{

}

void TestQgsLayoutRectItem::creation()
{
  TestItem* item = new TestItem( mLayout );
  QVERIFY( item );
  delete item;
}

bool TestQgsLayoutRectItem::renderCheck( QString testName, QImage &image, int mismatchCount )
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

QTEST_MAIN( TestQgsLayoutRectItem )
#include "testqgslayoutrectitem.moc"
