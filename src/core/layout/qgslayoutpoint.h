/***************************************************************************
                         qgslayoutpoint.h
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

#ifndef QGSLAYOUTPOINT_H
#define QGSLAYOUTPOINT_H

#include "qgslayoutunits.h"
#include <QPointF>

/**\ingroup Layout
 * \class QgsLayoutPoint
 * \brief This class provides a method of storing points, consisting of an x and y coordinate,
 * for use in QGIS layouts. Measurement units are stored alongside the position.
 *
 * \see QgsLayoutMeasurementConverter
 * \note added in QGIS 2.7
 * \note This class does not inherit from QPointF since QPointF includes methods which should not apply
 * to positions with with units. For instance, the + and - operators would mislead users of this class
 * to believe that addition of two QgsLayoutPoints with different unit types would automatically convert
 * units. Instead, all unit conversion must be handled by a QgsLayoutMeasurementConverter so that
 * conversion between paper and screen units can be correctly performed.
 */
class CORE_EXPORT QgsLayoutPoint
{
  public:

    /**Constructor for QgsLayoutPoint.
     * @param x x coordinate
     * @param x y coordinate
     * @param units units for x and y
    */
    QgsLayoutPoint( const double x, const double y, const QgsLayoutUnits::Units units = QgsLayoutUnits::Millimeters );

    /**Constructor for an empty point, where both x and y are set to 0.
     * @param units units for measurement
    */
    explicit QgsLayoutPoint( const QgsLayoutUnits::Units units = QgsLayoutUnits::Millimeters );

    virtual ~QgsLayoutPoint();

    /**Sets new x and y coordinates for the point.
     * @param x new x coordinate
     * @param y new y coordinate
     * @see setX
     * @see setY
     * @see setUnits
    */
    void setPoint( const double x, const double y ) { mX = x; mY = y; }

    /**Returns x coordinate of point.
     * @returns x coordinate
     * @see setX
     * @see y
    */
    double x() const { return mX; }

    /**Sets x coordinate of point.
     * @param x x coordinate
     * @see x
     * @see setY
    */
    void setX( const double x ) { mX = x; }

    /**Returns y coordinate of point.
     * @returns y coordinate
     * @see setY
     * @see x
    */
    double y() const { return mY; }

    /**Sets y coordinate of point.
     * @param y y coordinate
     * @see y
     * @see setX
    */
    void setY( const double y ) { mY = y; }

    /**Units for the point.
     * @returns size units
     * @see setUnits
    */
    QgsLayoutUnits::Units units() const { return mUnits; }

    /**Sets the units for the point. Does not alter the stored coordinates,
     * ie. no conversion is done.
     * @param units new position units
     * @see units
    */
    void setUnits( const QgsLayoutUnits::Units units ) { mUnits = units; }

    /**Tests whether the position is null, ie both its x and y coordinates
     * are zero.
     * @returns true if point is null
    */
    bool isNull() const;

    /**Converts the layout point to a QPointF. The unit information is discarded
     * during this operation.
     * @returns QPointF with same x and y coordinates as layout point
    */
    QPointF toQPointF() const;

    /**Encodes the layout point to a string
     * @returns point encoded as string
     * @see decodePoint
    */
    QString encodePoint() const;

    /**Decodes a point from a string
     * @param string string to decode
     * @returns string decoded as point
     * @see encodePoint
    */
    static QgsLayoutPoint decodePoint( const QString &string );

    bool operator==( const QgsLayoutPoint &other ) const;
    bool operator!=( const QgsLayoutPoint &other ) const;
    QgsLayoutPoint operator*( const double v ) const;
    QgsLayoutPoint operator*=( const double v );
    QgsLayoutPoint operator/( const double v ) const;
    QgsLayoutPoint operator/=( const double v );

  private:

    double mX;
    double mY;
    QgsLayoutUnits::Units mUnits;

};

#endif // QGSLAYOUTPOINT_H
