/***************************************************************************
                              qgslayoutitem.h
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
#ifndef QGSLAYOUTITEM_H
#define QGSLAYOUTITEM_H

#include "qgslayoutobject.h"
#include "qgslayoutitemregistry.h"
#include "qgslayoutpoint.h"
#include "qgslayoutsize.h"
#include <QGraphicsRectItem>

class QgsLayout;
class QPainter;

/**\ingroup Layout
 * \class QgsLayoutItem
 * \brief
 * \note added in QGIS 2.7
 */
class CORE_EXPORT QgsLayoutItem : public QgsLayoutObject, public QGraphicsRectItem
{

    Q_OBJECT

  public:

    enum ReferencePoint
    {
      UpperLeft,
      UpperMiddle,
      UpperRight,
      MiddleLeft,
      Middle,
      MiddleRight,
      LowerLeft,
      LowerMiddle,
      LowerRight
    };

    explicit QgsLayoutItem( QgsLayout* layout );

    virtual ~QgsLayoutItem();

    /**Return correct graphics item type*/
    virtual int type() const = 0;

    /**Draws item's contents on a specified painter
     */
    virtual void draw( QPainter* painter, const QStyleOptionGraphicsItem* itemStyle, QWidget* pWidget ) = 0;

    /**Handles preparing a paint surface for the layout item and painting the item's
     * content. Derived classes must not override this method, but instead implement
     * the pure virtual method QgsLayoutItem::draw
     */
    void paint( QPainter* painter, const QStyleOptionGraphicsItem* itemStyle, QWidget* pWidget );

    /**Sets the reference point for positioning of the layout item. This point is also
     * fixed during resizing of the item, and any size changes will be performed
     * so that the position of the reference point within the layout remains unchanged.
     * @param reference reference point
     * @see referencePoint
    */
    void setReferencePoint( const ReferencePoint& reference );

    /**Returns the reference point for positioning of the layout item. This point is also
     * fixed during resizing of the item, and any size changes will be performed
     * so that the position of the reference point within the layout remains unchanged.
     * @returns reference point
     * @see setReferencePoint
    */
    ReferencePoint referencePoint() const { return mReferencePoint; }

    /**Returns the fixed size of the item, if applicable.
     * @returns fixed size for layout item, or an empty size is item can be freely
     * resized
     * @see setFixedSize
     * @see minimumSize
    */
    QgsLayoutSize fixedSize() { return mFixedSize; }

    /**Returns the minimum allowed size of the item, if applicable.
     * @returns minimum allowed size for layout item, or an empty size is item can be freely
     * resized
     * @see setMinimumSize
     * @see fixedSize
    */
    virtual QgsLayoutSize minimumSize() { return mMinimumSize; }

    /**Attempts to resize the item to a specified size. Note that the final size of the
     * item may not match the specified target size, as items with a fixed or minimum
     * size will place restrictions on the allowed item size. Data defined item size
     * will also override the specified target size.
     * @param targetSize desired size for item
     * @see minimumSize
     * @see fixedSize
     * @see attemptMove
    */
    virtual void attemptResize( const QgsLayoutSize& targetSize );

    /**Attempts to move the item to a specified position. This method respects the item's
     * reference point, in that the item will be moved so that its current reference
     * point is placed at the specified target point.
     * Note that the final position of the item may not match the specified target position,
     * as data defined item position may override the specified value.
     * @param targetPoint desired position for reference point of item
     * @see attemptResize
     * @see referencePoint
    */
    virtual void attemptMove( const QgsLayoutPoint& targetPoint );

    /**Returns the item's current position, including units. The position returned
     * is the position of the item's reference point, which may not necessarily be the top
     * left corner of the item.
     * @returns position of item's reference point within the layout
     * @see attemptMove
     * @see referencePoint
     */
    QgsLayoutPoint positionWithUnits() const { return mItemPosition; }

  public slots:

    /**Refreshes a data defined property for the item by reevaluating the property's value
     * and redrawing the item with this new value.
     * @param property data defined property to refresh. If property is set to
     * QgsComposerItem::AllProperties then all data defined properties for the item will be
     * refreshed.
     * @note this method was added in version 2.5
    */
    virtual void refreshDataDefinedProperty( const QgsLayoutObject::DataDefinedProperty property = QgsLayoutObject::AllProperties );


  protected:

    /**Draws a debugging rectangle of the item's current bounds within the specified
     * painter.
     * @param painter destination QPainter
     */
    virtual void drawDebugRect( QPainter* painter );

    /**Sets a fixed size for the layout item, which prevents it from being freely
     * resized.
     * @param size fixed size for layout item, or an empty size is item can be freely
     * resized
     * @see fixedSize
     * @see setMinimumSize
    */
    virtual void setFixedSize( const QgsLayoutSize& size );

    /**Sets the minimum allowed size for the layout item.
     * @param size minimum size for layout item, or an empty size is item can be freely
     * resized
     * @see minimumSize
     * @see setFixedSize
    */
    virtual void setMinimumSize( const QgsLayoutSize& size );

    /**Refreshes an item's size by rechecking it against any possible item fixed
     * or minimum sizes.
     * @see setFixedSize
     * @see setMinimumSize
     * @see refreshItemPosition
     */
    void refreshItemSize();

    /**Refreshes an item's position by rechecking it against any possible overrides
     * such as data defined positioning.
     * @see refreshItemSize
     */
    void refreshItemPosition();

    /**Adjusts a point to account for the item's reference point.
     * @param position position of reference point.
     * @param size size of item
     * @returns top left coordinate of item
     * @see referencePoint
     */
    QPointF adjustPointForReferencePosition( const QPointF& position, const QSizeF& size );

  private slots:

    /**Updates an item's size after the layout's dpi changes. This is required
     * when the item uses a screen based measurement unit and the layout uses a paper
     * based unit, or vice versa.
     * @param dpi new dpi for the layout
    */
    void layoutDpiChanged( const double dpi );

  private:

    ReferencePoint mReferencePoint;
    QgsLayoutSize mFixedSize;
    QgsLayoutSize mMinimumSize;

    QgsLayoutSize mItemSize;
    QgsLayoutPoint mItemPosition;

    void initConnectionsToLayout();

    /**Prepares a painter by setting rendering flags*/
    void preparePainter( QPainter* painter );
    bool shouldDrawAntialiased() const;
    bool shouldDrawDebugRect() const;

    QSizeF applyMinimumSize( const QSizeF &targetSize );
    QSizeF applyFixedSize( const QSizeF &targetSize );
    QgsLayoutPoint applyDataDefinedPosition( const QgsLayoutPoint &position );

    friend class TestQgsLayoutItem;
};

#endif //QGSLAYOUTITEM_H



