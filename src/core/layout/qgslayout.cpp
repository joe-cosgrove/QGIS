/***************************************************************************
                              qgslayout.cpp
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

#include "qgslayout.h"

QgsLayout::QgsLayout()
    : QGraphicsScene()
    , mFlags( Antialiasing | UseAdvancedEffects )
{

}

QgsLayout::~QgsLayout()
{

}

void QgsLayout::setFlags( const Flags flags )
{
  mFlags = flags;
}

void QgsLayout::setFlag( const QgsLayout::Flag flag, const bool on )
{
  if ( on )
    mFlags |= flag;
  else
    mFlags &= ~flag;
}

QgsLayout::Flags QgsLayout::flags() const
{
  return mFlags;
}

bool QgsLayout::testFlag( const QgsLayout::Flag flag ) const
{
  return mFlags.testFlag( flag );
}
