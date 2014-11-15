/***************************************************************************
                         qgslayoutsize.h
                         ---------------
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

#ifndef QGSLAYOUTSIZE_H
#define QGSLAYOUTSIZE_H

#include "qgslayoutunits.h"
#include <QSizeF>


/**\ingroup Layout
 * \class QgsLayoutSize
 * \brief This class provides a method of storing sizes, consisting of a width and height,
 * for use in QGIS layouts. Measurement units are stored alongside the size.
 *
 * \see QgsLayoutMeasurementConverter
 * \note added in QGIS 2.7
 * \note This class does not inherit from QSizeF since QSizeF includes methods which should not apply to sizes
 * with units. For instance, the + and - operators would mislead users of this class to believe that
 * addition of two QgsLayoutSize with different unit types would automatically convert units. Instead,
 * all unit conversion must be handled by a QgsLayoutMeasurementConverter so that conversion between
 * paper and screen units can be correctly performed.
 */
class CORE_EXPORT QgsLayoutSize
{
  public:

    /**Constructor for QgsLayoutSize.
     * @param width width
     * @param height height
     * @param units units for width and height
    */
    QgsLayoutSize( const double width, const double height, const QgsLayoutUnits::Units units = QgsLayoutUnits::Millimeters );

    /**Constructor for an empty layout size
     * @param units units for measurement
    */
    explicit QgsLayoutSize( const QgsLayoutUnits::Units units = QgsLayoutUnits::Millimeters );

    virtual ~QgsLayoutSize();

    /**Sets new width and height for the size.
     * @param width new width
     * @param height new height
     * @see setWidth
     * @see setHeight
     * @see setUnits
    */
    void setSize( const double width, const double height ) { mWidth = width; mHeight = height; }

    /**Width of the size.
     * @returns width
     * @see setWidth
     * @see height
    */
    double width() const { return mWidth; }

    /**Sets width for the size.
     * @param width new width
     * @see width
     * @see setHeight
    */
    void setWidth( const double width ) { mWidth = width; }

    /**Height of the size.
     * @returns height
     * @see setHeight
     * @see width
    */
    double height() const { return mHeight; }

    /**Sets height for the size.
     * @param height new height
     * @see height
     * @see setWidth
    */
    void setHeight( const double height ) { mHeight = height; }

    /**Units for the size.
     * @returns size units
     * @see setUnits
    */
    QgsLayoutUnits::Units units() const { return mUnits; }

    /**Sets the units for the size. Does not alter the stored width or height,
     * ie. no conversion is done.
     * @param units new size units
     * @see units
    */
    void setUnits( const QgsLayoutUnits::Units units ) { mUnits = units; }

    /**Tests whether the size is empty, ie both its width and height
     * are zero.
     * @returns true if size is empty
    */
    bool isEmpty() const;

    /**Converts the layout size to a QSizeF. The unit information is discarded
     * during this operation.
     * @returns QSizeF with same dimensions as layout size
    */
    QSizeF toQSizeF() const;

    bool operator==( const QgsLayoutSize &other ) const;
    bool operator!=( const QgsLayoutSize &other ) const;
    QgsLayoutSize operator*( const double v ) const;
    QgsLayoutSize operator*=( const double v );
    QgsLayoutSize operator/( const double v ) const;
    QgsLayoutSize operator/=( const double v );

  private:

    double mWidth;
    double mHeight;
    QgsLayoutUnits::Units mUnits;

};

#endif // QGSLAYOUTSIZE_H
