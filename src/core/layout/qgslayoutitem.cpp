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
#include "qgslayoutcontext.h"
#include "qgslayoututils.h"
#include "qgis.h"
#include <QPainter>
#include <QUuid>

QgsLayoutItem::QgsLayoutItem( QgsLayout *layout )
    : QgsLayoutObject( layout )
    , QGraphicsRectItem( 0 )
    , mUuid( QUuid::createUuid().toString() )
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

void QgsLayoutItem::setId( const QString &id )
{
  if ( id == mId )
  {
    return;
  }

  mId = id;
  setToolTip( id );

  //TODO
  /*
  //inform model that id data has changed
  if ( mComposition )
  {
    mComposition->itemsModel()->updateItemDisplayName( this );
  }

  emit itemChanged();*/
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
  updateStoredItemPosition();
  refreshItemPosition();
}

void QgsLayoutItem::attemptResize( const QgsLayoutSize &targetSize )
{
  if ( !mLayout )
  {
    mItemSize = targetSize;
    setRect( 0, 0, targetSize.width(), targetSize.height() );
    return;
  }

  QgsLayoutSize evaluatedSize = applyDataDefinedSize( targetSize );
  QSizeF targetSizeLayoutUnits = mLayout->convertToLayoutUnits( evaluatedSize );
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

  QgsLayoutPoint evaluatedPoint = applyDataDefinedPosition( targetPoint );
  QPointF evaluatedPointLayoutUnits = mLayout->convertToLayoutUnits( evaluatedPoint );
  QPointF topLeftPointLayoutUnits = adjustPointForReferencePosition( evaluatedPointLayoutUnits, rect().size(), mReferencePoint );

  if ( topLeftPointLayoutUnits == scenePos() && targetPoint.units() == mItemPosition.units() )
  {
    //TODO - add test for second condition
    return;
  }

  QgsLayoutPoint referencePointTargetUnits = mLayout->convertFromLayoutUnits( evaluatedPointLayoutUnits, targetPoint.units() );
  mItemPosition = referencePointTargetUnits;
  setScenePos( topLeftPointLayoutUnits );
}

void QgsLayoutItem::setScenePos( const QPointF &destinationPos )
{
  //since setPos does not account for item rotation, use difference between
  //current scenePos (which DOES account for rotation) and destination pos
  //to calculate how much the item needs to move
  setPos( pos() + ( destinationPos - scenePos() ) );
}

double QgsLayoutItem::itemRotation() const
{
  return rotation();
}

QgsLayoutPoint QgsLayoutItem::applyDataDefinedPosition( const QgsLayoutPoint& position )
{
  if ( !mLayout )
  {
    return position;
  }

  double evaluatedX = applyDataDefinedProperty( position.x(), QgsLayoutObject::PositionX );
  double evaluatedY = applyDataDefinedProperty( position.y(), QgsLayoutObject::PositionY );
  return QgsLayoutPoint( evaluatedX, evaluatedY, position.units() );
}

QgsLayoutSize QgsLayoutItem::applyDataDefinedSize( const QgsLayoutSize &size )
{
  if ( !mLayout )
  {
    return size;
  }

  double evaluatedWidth = applyDataDefinedProperty( size.width(), QgsLayoutObject::ItemWidth );
  double evaluatedHeight = applyDataDefinedProperty( size.height(), QgsLayoutObject::ItemHeight );
  return QgsLayoutSize( evaluatedWidth, evaluatedHeight, size.units() );
}

double QgsLayoutItem::applyDataDefinedRotation( const double rotation )
{
  if ( !mLayout )
  {
    return rotation;
  }

  double evaluatedRotation = applyDataDefinedProperty( rotation, QgsLayoutObject::ItemRotation, 0.0 );
  return evaluatedRotation;
}

void QgsLayoutItem::refreshDataDefinedProperty( const QgsLayoutObject::DataDefinedProperty property )
{
  //update data defined properties and update item to match

  //evaulate width and height first, since they may affect position if non-top-left reference point set
  if ( property == QgsLayoutObject::ItemWidth || property == QgsLayoutObject::ItemHeight ||
       property == QgsLayoutObject::AllProperties )
  {
    refreshItemSize();
  }
  if ( property == QgsLayoutObject::PositionX || property == QgsLayoutObject::PositionY ||
       property == QgsLayoutObject::AllProperties )
  {
    refreshItemPosition();
  }
  if ( property == QgsLayoutObject::ItemRotation || property == QgsLayoutObject::AllProperties )
  {
    refreshItemRotation();
  }
}

void QgsLayoutItem::setItemRotation( const double angle )
{
  QPointF itemCenter = positionAtReferencePoint( QgsLayoutItem::Middle );
  double rotationRequired = angle - itemRotation();
  rotateItem( rotationRequired, itemCenter );
}

void QgsLayoutItem::updateStoredItemPosition()
{
  QPointF layoutPosReferencePoint = positionAtReferencePoint( mReferencePoint );
  mItemPosition = mLayout->convertFromLayoutUnits( layoutPosReferencePoint, mItemPosition.units() );
}

void QgsLayoutItem::rotateItem( const double angle, const QPointF &transformOrigin )
{
  double evaluatedAngle = angle + rotation();
  evaluatedAngle = QgsLayoutUtils::normalizedAngle( evaluatedAngle, true );
  evaluatedAngle = applyDataDefinedRotation( evaluatedAngle );

  QPointF itemTransformOrigin = mapFromScene( transformOrigin );
  setTransformOriginPoint( itemTransformOrigin );
  setRotation( evaluatedAngle );

  //TODO - remember that double spin box for rotation should have limits of -100000, 100000
  //as this function will automatically adjust value to valid ranges

  //adjust stored position of item to match scene pos of reference point
  updateStoredItemPosition();


  //TODO
//  emit itemRotationChanged( rotation );

  //TODO
  //update bounds of scene, since rotation may affect this
  //mLayout->updateBounds();
}

QPointF QgsLayoutItem::itemPositionAtReferencePoint( const ReferencePoint reference, const QSizeF& size ) const
{
  switch ( reference )
  {
    case UpperMiddle:
      return QPointF( size.width() / 2.0, 0 );
    case UpperRight:
      return QPointF( size.width(), 0 );
    case MiddleLeft:
      return QPointF( 0, size.height() / 2.0 );
    case Middle:
      return QPointF( size.width() / 2.0, size.height() / 2.0 );
    case MiddleRight:
      return QPointF( size.width(), size.height() / 2.0 );
    case LowerLeft:
      return QPointF( 0, size.height() );
    case LowerMiddle:
      return QPointF( size.width() / 2.0, size.height() );
    case LowerRight:
      return QPointF( size.width(), size.height() );
    case UpperLeft:
    default:
      return QPointF( 0, 0 );
  }
}

QPointF QgsLayoutItem::adjustPointForReferencePosition( const QPointF& position, const QSizeF& size, const ReferencePoint &reference ) const
{
  QPointF itemPosition = mapFromScene( position ); //need to map from scene to handle item rotation
  QPointF adjustedPointInsideItem = itemPosition - itemPositionAtReferencePoint( reference, size );
  return mapToScene( adjustedPointInsideItem );
}

QPointF QgsLayoutItem::positionAtReferencePoint( const QgsLayoutItem::ReferencePoint &reference ) const
{
  QPointF pointWithinItem = itemPositionAtReferencePoint( reference, rect().size() );
  return mapToScene( pointWithinItem );
}

QDomElement QgsLayoutItem::toXmlElement( QDomDocument &document ) const
{
  QDomElement element = document.createElement( "LayoutItem" );
  element.setAttribute( "type", stringType() );

  writePropertiesToElement( element, document );

  return element;
}

bool QgsLayoutItem::readXMLElement( const QDomElement &element, const QDomDocument &document )
{
  if ( element.nodeName() != QString( "LayoutItem" ) || element.attribute( "type" ) != stringType() )
  {
    return false;
  }

  return readPropertiesFromElement( element, document );
}

bool QgsLayoutItem::writePropertiesToElement( QDomElement& element, QDomDocument& document ) const
{
  element.setAttribute( "uuid", mUuid );
  element.setAttribute( "id", mId );
  element.setAttribute( "refeferencePoint", QString::number(( int ) mReferencePoint ) );
  element.setAttribute( "position", mItemPosition.encodePoint() );
  element.setAttribute( "size", mItemSize.encodeSize() );
  element.setAttribute( "rotation", QString::number( rotation() ) );

  //TODO
  /*

  composerItemElem.setAttribute( "zValue", QString::number( zValue() ) );
  composerItemElem.setAttribute( "visibility", isVisible() );
  //position lock for mouse moves/resizes
  if ( mItemPositionLocked )
  {
    composerItemElem.setAttribute( "positionLock", "true" );
  }
  else
  {
    composerItemElem.setAttribute( "positionLock", "false" );
  }
  */

  //blend mode
//  composerItemElem.setAttribute( "blendMode", QgsMapRenderer::getBlendModeEnum( mBlendMode ) );

  //transparency
//  composerItemElem.setAttribute( "transparency", QString::number( mTransparency ) );

//  composerItemElem.setAttribute( "excludeFromExports", mExcludeFromExports );

  writeObjectPropertiesToElement( element, document );
  return true;
}

bool QgsLayoutItem::readPropertiesFromElement( const QDomElement &element, const QDomDocument &document )
{
  readObjectPropertiesFromElement( element, document );

  mUuid = element.attribute( "uuid", QUuid::createUuid().toString() );
  setId( element.attribute( "id" ) );
  mReferencePoint = ( ReferencePoint )element.attribute( "refeferencePoint" ).toInt();
  attemptMove( QgsLayoutPoint::decodePoint( element.attribute( "position" ) ) );
  attemptResize( QgsLayoutSize::decodeSize( element.attribute( "size" ) ) );
  setItemRotation( element.attribute( "rotation", "0" ).toDouble() );

  //TODO
  /*
  // temporary for groups imported from templates
  mTemplateUuid = itemElem.attribute( "templateUuid" );

  //position lock for mouse moves/resizes
  QString positionLock = itemElem.attribute( "positionLock" );
  if ( positionLock.compare( "true", Qt::CaseInsensitive ) == 0 )
  {
    setPositionLock( true );
  }
  else
  {
    setPositionLock( false );
  }

  //visibility
  setVisibility( itemElem.attribute( "visibility", "1" ) != "0" );

  setZValue( itemElem.attribute( "zValue" ).toDouble() );

  //blend mode
  setBlendMode( QgsMapRenderer::getCompositionMode(( QgsMapRenderer::BlendMode ) itemElem.attribute( "blendMode", "0" ).toUInt() ) );

  //transparency
  setTransparency( itemElem.attribute( "transparency", "0" ).toInt() );

  mExcludeFromExports = itemElem.attribute( "excludeFromExports", "0" ).toInt();
  mEvaluatedExcludeFromExports = mExcludeFromExports;
  */
  return true;
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

void QgsLayoutItem::refreshItemRotation()
{
  setItemRotation( itemRotation() );
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
  return mLayout->context()->testFlag( QgsLayoutContext::Antialiasing ) && !mLayout->context()->testFlag( QgsLayoutContext::Debug );
}

bool QgsLayoutItem::shouldDrawDebugRect() const
{
  return mLayout && mLayout->context()->testFlag( QgsLayoutContext::Debug );
}
