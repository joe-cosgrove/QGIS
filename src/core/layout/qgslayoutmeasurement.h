/***************************************************************************
                         qgslayoutmeasurement.h
                         --------------------
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

#ifndef QGSLAYOUTMEASUREMENT_H
#define QGSLAYOUTMEASUREMENT_H

#include "qgslayoutunits.h"

/**\ingroup Layout
 * \class QgsLayoutMeasurement
 * \brief This class provides a method of storing measurements for use in QGIS layouts
 * using a variety of different measurement units.
 * \see QgsLayoutMeasurementConverter
 * \note added in QGIS 2.7
 */
class CORE_EXPORT QgsLayoutMeasurement
{
  public:

    /**Constructor for QgsLayoutMeasurement.
     * @param length measurement length
     * @param units measurement units
    */
    explicit QgsLayoutMeasurement( const double length, const QgsLayoutUnits::Units units = QgsLayoutUnits::Millimeters );

    virtual ~QgsLayoutMeasurement();

    /**Length of the measurement.
     * @returns measurement length
     * @see setLength
    */
    double length() const { return mLength; }

    /**Sets length of the measurement.
     * @param length new measurement length
     * @see length
    */
    void setLength( const double length ) { mLength = length; }

    /**Units for the measurement.
     * @returns measurement units
     * @see setUnits
    */
    QgsLayoutUnits::Units units() const { return mUnits; }

    /**Sets the units for the measurement. Does not alter the stored length,
     * ie. no length conversion is done.
     * @param units new measurement units
     * @see units
    */
    void setUnits( const QgsLayoutUnits::Units units ) { mUnits = units; }

    bool operator==( const QgsLayoutMeasurement &other ) const;
    bool operator!=( const QgsLayoutMeasurement &other ) const;
    QgsLayoutMeasurement operator+( const double v ) const;
    QgsLayoutMeasurement operator+=( const double v );
    QgsLayoutMeasurement operator-( const double v ) const;
    QgsLayoutMeasurement operator-=( const double v );
    QgsLayoutMeasurement operator*( const double v ) const;
    QgsLayoutMeasurement operator*=( const double v );
    QgsLayoutMeasurement operator/( const double v ) const;
    QgsLayoutMeasurement operator/=( const double v );

  private:

    double mLength;
    QgsLayoutUnits::Units mUnits;

};

#endif // QGSLAYOUTMEASUREMENT_H
