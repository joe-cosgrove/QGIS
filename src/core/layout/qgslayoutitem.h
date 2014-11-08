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

  public:

    QgsLayoutItem( QgsLayout* layout );

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

  protected:

    /**Draws a debugging rectangle of the item's current bounds within the specified
     * painter.
     * @param painter destination QPainter
     */
    virtual void drawDebugRect( QPainter* painter );

  private:

    /**Prepares a painter by setting rendering flags*/
    void preparePainter( QPainter* painter );
    bool shouldDrawAntialiased() const;
    bool shouldDrawDebugRect() const;


    friend class TestQgsLayoutItem;
};

#endif //QGSLAYOUTITEM_H



