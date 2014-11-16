/***************************************************************************
                              qgslayoutitem.cpp
                             -------------------
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
#include <QPainter>

QgsLayoutItem::QgsLayoutItem( QgsLayout *layout )
    : QgsLayoutObject( layout )
    , QGraphicsRectItem( 0 )
    , mReferencePoint( UpperLeft )
{
  setCacheMode( QGraphicsItem::DeviceCoordinateCache );

  //record initial position
  QgsLayoutUnits::Units initialUnits = layout ? layout->units() : QgsLayoutUnits::Millimeters;
  mItemPosition = QgsLayoutPoint( scenePos().x(), scenePos().y(), initialUnits );
  mItemSize = QgsLayoutSize( rect().width(), rect().height(), initialUnits );

  initConnectionsToLayout();
}

QgsLayoutItem::~QgsLayoutItem()
{

}

void QgsLayoutItem::initConnectionsToLayout()
{
  if ( !mLayout )
  {
    return;
  }

  connect( mLayout, SIGNAL( dpiChanged( const double ) ), this, SLOT( layoutDpiChanged( const double ) ) );
}

void QgsLayoutItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *itemStyle, QWidget *pWidget )
{
  if ( !painter || !painter->device() )
  {
    return;
  }

  //TODO - remember to disable saving/restoring on graphics view!!
  painter->save();
  preparePainter( painter );

  if ( shouldDrawDebugRect() )
  {
    drawDebugRect( painter );
  }
  else
  {
    draw( painter, itemStyle, pWidget );
  }

  painter->restore();
}

void QgsLayoutItem::setReferencePoint( const QgsLayoutItem::ReferencePoint &reference )
{
  if ( reference == mReferencePoint )
  {
    return;
  }

  mReferencePoint = reference;

  //also need to adjust stored position
  QPointF positionReferencePointLayoutUnits = adjustPointForReferencePosition( pos(), QSizeF( -rect().width(), -rect().height() ) );
  mItemPosition = mLayout->convertFromLayoutUnits( positionReferencePointLayoutUnits, mItemPosition.units() );
}

void QgsLayoutItem::attemptResize( const QgsLayoutSize &targetSize )
{
  if ( !mLayout )
  {
    mItemSize = targetSize;
    setRect( 0, 0, targetSize.width(), targetSize.height() );
    return;
  }

  QSizeF targetSizeLayoutUnits = mLayout->convertToLayoutUnits( targetSize );
  QSizeF actualSizeLayoutUnits = applyMinimumSize( targetSizeLayoutUnits );
  actualSizeLayoutUnits = applyFixedSize( actualSizeLayoutUnits );

  if ( actualSizeLayoutUnits == rect().size() )
  {
    return;
  }

  QgsLayoutSize actualSizeTargetUnits = mLayout->convertFromLayoutUnits( actualSizeLayoutUnits, targetSize.units() );
  mItemSize = actualSizeTargetUnits;

  setRect( 0, 0, actualSizeLayoutUnits.width(), actualSizeLayoutUnits.height() );
  refreshItemPosition();
}

void QgsLayoutItem::attemptMove( const QgsLayoutPoint& targetPoint )
{
  if ( !mLayout )
  {
    mItemPosition = targetPoint;
    setPos( targetPoint.toQPointF() );
    return;
  }

  QPointF targetPointLayoutUnits = mLayout->convertToLayoutUnits( targetPoint );
  //TODO - apply data defined position here
  targetPointLayoutUnits = adjustPointForReferencePosition( targetPointLayoutUnits, rect().size() );
  QPointF actualPointLayoutUnits = targetPointLayoutUnits;

  if ( actualPointLayoutUnits == pos() )
  {
    return;
  }

  QgsLayoutPoint actualPointTargetUnits = mLayout->convertFromLayoutUnits( actualPointLayoutUnits, targetPoint.units() );
  mItemPosition = actualPointTargetUnits;

  setPos( targetPointLayoutUnits );
}

QPointF QgsLayoutItem::adjustPointForReferencePosition( const QPointF& position, const QSizeF& size )
{
  switch ( mReferencePoint )
  {
    case UpperMiddle:
      return QPointF( position.x() - size.width() / 2.0, position.y() );
    case UpperRight:
      return QPointF( position.x() - size.width(), position.y() );
    case MiddleLeft:
      return QPointF( position.x(), position.y() - size.height() / 2.0 );
    case Middle:
      return QPointF( position.x() - size.width() / 2.0, position.y() - size.height() / 2.0 );
    case MiddleRight:
      return QPointF( position.x() - size.width(), position.y() - size.height() / 2.0 );
    case LowerLeft:
      return QPointF( position.x(), position.y() - size.height() );
    case LowerMiddle:
      return QPointF( position.x() - size.width() / 2.0, position.y() - size.height() );
    case LowerRight:
      return QPointF( position.x() - size.width() , position.y() - size.height() );
    case UpperLeft:
    default:
      return position;
  }
}

QSizeF QgsLayoutItem::applyMinimumSize( const QSizeF& targetSize )
{
  if ( !mLayout || minimumSize().isEmpty() )
  {
    return targetSize;
  }
  QSizeF mininumSizeLayoutUnits = mLayout->convertToLayoutUnits( minimumSize() );
  return targetSize.expandedTo( mininumSizeLayoutUnits );
}

QSizeF QgsLayoutItem::applyFixedSize( const QSizeF& targetSize )
{
  if ( !mLayout || fixedSize().isEmpty() )
  {
    return targetSize;
  }
  QSizeF fixedSizeLayoutUnits = mLayout->convertToLayoutUnits( fixedSize() );
  return targetSize.expandedTo( fixedSizeLayoutUnits );
}

void QgsLayoutItem::drawDebugRect( QPainter *painter )
{
  if ( !painter )
  {
    return;
  }

  painter->save();
  painter->setRenderHint( QPainter::Antialiasing, false );
  painter->setPen( Qt::NoPen );
  painter->setBrush( QColor( 100, 255, 100, 200 ) );
  painter->drawRect( rect() );
  painter->restore();
}

void QgsLayoutItem::setFixedSize( const QgsLayoutSize &size )
{
  mFixedSize = size;
  refreshItemSize();
}

void QgsLayoutItem::setMinimumSize( const QgsLayoutSize &size )
{
  mMinimumSize = size;
  refreshItemSize();
}

void QgsLayoutItem::layoutDpiChanged( const double dpi )
{
  Q_UNUSED( dpi );

  if ( QgsLayoutUnits::unitType( mLayout->units() ) !=
       QgsLayoutUnits::unitType( mItemSize.units() ) )
  {
    //conversion required, item and layout units are in a mix of both paper and screen units
    refreshItemSize();
  }
  if ( QgsLayoutUnits::unitType( mLayout->units() ) !=
       QgsLayoutUnits::unitType( mItemPosition.units() ) )
  {
    //conversion required, item and layout units are in a mix of both paper and screen units
    refreshItemPosition();
  }
}

void QgsLayoutItem::refreshItemSize()
{
  attemptResize( mItemSize );
}

void QgsLayoutItem::refreshItemPosition()
{
  attemptMove( mItemPosition );
}

void QgsLayoutItem::preparePainter( QPainter *painter )
{
  if ( !painter || !painter->device() )
  {
    return;
  }
  painter->setRenderHint( QPainter::Antialiasing, shouldDrawAntialiased() );
}

bool QgsLayoutItem::shouldDrawAntialiased() const
{
  if ( !mLayout )
  {
    return true;
  }
  return mLayout->testFlag( QgsLayout::Antialiasing ) && !mLayout->testFlag( QgsLayout::Debug );
}

bool QgsLayoutItem::shouldDrawDebugRect() const
{
  return mLayout && mLayout->testFlag( QgsLayout::Debug );
}

