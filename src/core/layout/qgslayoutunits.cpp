/***************************************************************************
                         qgslayoutunits.cpp
                         ------------------
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

#include "qgslayoutunits.h"

QgsLayoutUnits::UnitType QgsLayoutUnits::unitType( const QgsLayoutUnits::Units units )
{
  switch ( units )
  {
    case Pixels:
      return QgsLayoutUnits::Screen;
    case  Millimeters:
    case Centimeters:
    case Meters:
    case Inches:
    case Feet:
    case Points:
    case Picas:
    default:
      return QgsLayoutUnits::Paper;
  }
}

