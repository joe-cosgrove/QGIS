/***************************************************************************
                         qgslayoutmeasurementconverter.h
                         -------------------------------
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

#ifndef QGSLAYOUTMEASUREMENTCONVERTER_H
#define QGSLAYOUTMEASUREMENTCONVERTER_H

#include "qgslayoutunits.h"
#include "qgslayoutmeasurement.h"
#include "qgslayoutsize.h"
#include "qgslayoutpoint.h"
#include <QSizeF>
#include <QPointF>


/**\ingroup Layout
 * \class QgsLayoutMeasurementConverter
 * \brief This class provides a method of converting QgsLayoutMeasurements from
 * one unit to another. Conversion to or from pixel units utilises a specified
 * dots per inch (DPI) property for the converter. Converters default to using
 * 300 DPI.
 * \see QgsLayoutMeasurement
 * \note added in QGIS 2.7
 */
class CORE_EXPORT QgsLayoutMeasurementConverter
{
  public:

    QgsLayoutMeasurementConverter();

    virtual ~QgsLayoutMeasurementConverter();

    /**Sets the dots per inch (DPI) for the measurement converter. This is used
     * when converting measurements to and from pixels.
     * @param dpi dots per inch for conversion
     * @see dpi
    */
    void setDpi( const double dpi ) { mDpi = dpi; }

    /**Dots per inch (DPI) of the measurement converter. This is used
     * when converting measurements to and from pixels.
     * @returns dots per inch for conversion
     * @see setDpi
    */
    double dpi() const { return mDpi; }

    /**Converts a measurement from one unit to another.
     * @param measurement measurement to convert
     * @param targetUnits units to convert measurement into
     * @returns measurement converted to target units
    */
    QgsLayoutMeasurement convert( const QgsLayoutMeasurement& measurement, const QgsLayoutUnits::Units targetUnits ) const;

    /**Converts a layout size from one unit to another.
     * @param size layout size to convert
     * @param targetUnits units to convert size into
     * @returns size converted to target units
    */
    QgsLayoutSize convert( const QgsLayoutSize& size, const QgsLayoutUnits::Units targetUnits ) const;

    /**Converts a layout point from one unit to another.
     * @param point layout point to convert
     * @param targetUnits units to convert point into
     * @returns point converted to target units
    */
    QgsLayoutPoint convert( const QgsLayoutPoint& point, const QgsLayoutUnits::Units targetUnits ) const;

  private:

    double mDpi;

    double convertToMillimeters( const QgsLayoutMeasurement &measurement ) const;
    double convertToCentimeters( const QgsLayoutMeasurement &measurement ) const;
    double convertToMeters( const QgsLayoutMeasurement &measurement ) const;
    double convertToInches( const QgsLayoutMeasurement &measurement ) const;
    double convertToFeet( const QgsLayoutMeasurement &measurement ) const;
    double convertToPoints( const QgsLayoutMeasurement &measurement ) const;
    double convertToPicas( const QgsLayoutMeasurement &measurement ) const;
    double convertToPixels( const QgsLayoutMeasurement &measurement ) const;

};

#endif // QGSLAYOUTMEASUREMENTCONVERTER_H
