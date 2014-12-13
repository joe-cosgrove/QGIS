/***************************************************************************
                         qgslayoutunits.h
                         ----------------
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

#ifndef QGSLAYOUTUNITS_H
#define QGSLAYOUTUNITS_H

#include <QString>

/**\ingroup Layout
 * \class QgsLayoutUnits
 * \brief
 * \note added in QGIS 2.7
 */
class CORE_EXPORT QgsLayoutUnits
{
  public:

    /** Units for measurement
     */
    enum Units
    {
      Millimeters = 0, /*< millimetres */
      Centimeters, /*< centimeters */
      Meters, /*< meters*/
      Inches, /*< inches*/
      Feet, /*< feet*/
      Points, /*< typographic points*/
      Picas, /*< typographic picas*/
      Pixels /*< pixels */
    };

    /** Types of units
     */
    enum UnitType
    {
      Paper = 0, /*< unit is a paper based measurement unit*/
      Screen /*< unit is a screen based measurement unit */
    };

    /**Returns the type for a unit of measurement.
     * @returns measurement type
    */
    static UnitType unitType( const Units units );

    /**Encodes a unit type to a string
     * @param units unit type to encode
     * @returns encoded string
     * @see decodeUnits
    */
    static QString encodeUnits( const QgsLayoutUnits::Units units );

    /**Decodes a unit type from a string
     * @param string string to decode
     * @returns decoded units
     * @see encodeUnits
    */
    static QgsLayoutUnits::Units decodeUnits( const QString &string );
};

#endif // QGSLAYOUTUNITS_H
