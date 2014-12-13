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

QString QgsLayoutUnits::encodeUnits( const QgsLayoutUnits::Units units )
{
  switch ( units )
  {
    case Centimeters:
      return QString( "cm" );
    case Meters:
      return QString( "m" );
    case Inches:
      return QString( "in" );
    case Feet:
      return QString( "ft" );
    case Points:
      return QString( "pt" );
    case Picas:
      return QString( "pi" );
    case Pixels:
      return QString( "px" );
    case  Millimeters:
    default:
      return QString( "mm" );
  }
}

QgsLayoutUnits::Units QgsLayoutUnits::decodeUnits( const QString& string )
{
  if ( string == QString( "mm" ) )
    return QgsLayoutUnits::Millimeters;
  else if ( string == QString( "cm" ) )
    return QgsLayoutUnits::Centimeters;
  else if ( string == QString( "m" ) )
    return QgsLayoutUnits::Meters;
  else if ( string == QString( "in" ) )
    return QgsLayoutUnits::Inches;
  else if ( string == QString( "ft" ) )
    return QgsLayoutUnits::Feet;
  else if ( string == QString( "pt" ) )
    return QgsLayoutUnits::Points;
  else if ( string == QString( "pi" ) )
    return QgsLayoutUnits::Picas;
  else if ( string == QString( "px" ) )
    return QgsLayoutUnits::Pixels;
  else //default to mm
    return QgsLayoutUnits::Millimeters;
}
