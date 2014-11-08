/***************************************************************************
                            qgslayoutitemregistry.cpp
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

#include "qgslayoutitemregistry.h"

QgsLayoutItemRegistry *QgsLayoutItemRegistry::mInstance = 0;
QgsLayoutItemRegistry* QgsLayoutItemRegistry::instance()
{
  if ( mInstance == 0 )
  {
    mInstance = new QgsLayoutItemRegistry();
  }
  return mInstance;
}

QgsLayoutItemRegistry::~QgsLayoutItemRegistry()
{

}

QgsLayoutItemRegistry::QgsLayoutItemRegistry()
{

}
