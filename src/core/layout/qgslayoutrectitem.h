/***************************************************************************
                             qgslayoutrectitem.h
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
#ifndef QGSLAYOUTRECTITEM_H
#define QGSLAYOUTRECTITEM_H

#include "qgslayoutitem.h"

/**\ingroup Layout
 * \class QgsLayoutRectItem
 * \brief
 * \note added in QGIS 2.7
 */
class CORE_EXPORT QgsLayoutRectItem : public QgsLayoutItem
{

    Q_OBJECT

  public:

    explicit QgsLayoutRectItem( QgsLayout* layout );

    virtual ~QgsLayoutRectItem();


  private:


    friend class TestQgsLayoutRectItem;
};

#endif //QGSLAYOUTRECTITEM_H



