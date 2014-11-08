/***************************************************************************
                            qgslayoutitemregistry.h
                            ------------------------
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
#ifndef QGSLAYOUTITEMREGISTRY_H
#define QGSLAYOUTITEMREGISTRY_H

#include <QGraphicsItem> //for QGraphicsItem::UserType

class QgsLayoutItem;

/**\ingroup Layout
 * \class QgsLayoutItemRegistry
 * \brief
 * \note added in QGIS 2.7
 */
class CORE_EXPORT QgsLayoutItemRegistry
{

  public:

    enum ItemType
    {
      // base class for the items
      LayoutItem = QGraphicsItem::UserType + 100,

      // known item types
      LayoutPage,

      // item types provided by plugins
      PluginItem

    };

    /**Returns the instance pointer, creating and populating the registry on the first call
     */
    static QgsLayoutItemRegistry* instance();

    virtual ~QgsLayoutItemRegistry();

  protected:

    /**Creates a registry and populates it with standard item types
    */
    QgsLayoutItemRegistry();

  private:

    static QgsLayoutItemRegistry *mInstance;

};

#endif //QGSLAYOUTITEMREGISTRY_H



