/***************************************************************************
                              qgslayoutpage.h
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
#ifndef QGSLAYOUTPAGE_H
#define QGSLAYOUTPAGE_H

#include "qgslayoutitem.h"

/**\ingroup Layout
 * \class QgsLayoutPage
 * \brief
 * \note added in QGIS 2.7
 */
class CORE_EXPORT QgsLayoutPage : public QgsLayoutItem
{

  public:

    enum Orientation
    {
      Portrait,
      Landscape
    };

    QgsLayoutPage( QgsLayout* layout );

    virtual ~QgsLayoutPage();

    /**Return correct graphics item type*/
    virtual int type() const { return QgsLayoutItemRegistry::LayoutPage; }

    void draw( QPainter* painter, const QStyleOptionGraphicsItem* itemStyle, QWidget* pWidget );

    /**Decodes a string representing a page orientation
     * @param orientationString string to decode
     * @param orientation will be set to decoded page orientation (if possible)
     * @returns true if string could be decoded
    */
    static bool decodePageOrientation( const QString& orientationString, QgsLayoutPage::Orientation& orientation );


};

#endif //QGSLAYOUTPAGE_H



