/***************************************************************************
                              qgslayoutpage.cpp
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

#include "qgslayoutpage.h"

QgsLayoutPage::QgsLayoutPage( QgsLayout* layout )
    : QgsLayoutItem( layout )
{

}

QgsLayoutPage::~QgsLayoutPage()
{

}

void QgsLayoutPage::draw( QPainter *painter, const QStyleOptionGraphicsItem *itemStyle, QWidget *pWidget )
{

}

bool QgsLayoutPage::decodePageOrientation( const QString &orientationString, QgsLayoutPage::Orientation &orientation )
{
  QString trimmedString = orientationString.trimmed();
  if ( trimmedString.compare( "portrait", Qt::CaseInsensitive ) == 0 )
  {
    orientation = QgsLayoutPage::Portrait;
    return true;
  }
  else if ( trimmedString.compare( "landscape", Qt::CaseInsensitive ) == 0 )
  {
    orientation = QgsLayoutPage::Landscape;
    return true;
  }
  return false;
}
