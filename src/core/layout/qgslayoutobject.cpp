/***************************************************************************
                            qgslayoutobject.cpp
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

#include <QPainter>

#include "qgslayout.h"
#include "qgslayoututils.h"
#include "qgslayoutobject.h"
#include "qgsdatadefined.h"

#define FONT_WORKAROUND_SCALE 10 //scale factor for upscaling fontsize and downscaling painter

#ifndef M_DEG2RAD
#define M_DEG2RAD 0.0174532925
#endif

QgsLayoutObject::QgsLayoutObject( QgsLayout* layout )
    : QObject( 0 )
    , mLayout( layout )
{
}

QgsLayoutObject::~QgsLayoutObject()
{

}
