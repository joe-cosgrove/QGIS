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

#include <QSizeF>

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

    /**Constructor for QgsLayoutMeasurement.
     * @param length measurement length
     * @param units measurement units
    */
    QgsLayoutMeasurement( const double length, const Units units = Millimeters );

    QgsLayoutMeasurement( const QgsLayoutMeasurement &other );

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
    Units units() const { return mUnits; }

    /**Sets the units for the measurement. Does not alter the stored length,
     * ie. no length conversion is done.
     * @param units new measurement units
     * @see units
    */
    void setUnits( const Units units ) { mUnits = units; }

    bool operator==( const QgsLayoutMeasurement &other ) const;
    bool operator!=( const QgsLayoutMeasurement &other ) const;
    QgsLayoutMeasurement & operator=( const QgsLayoutMeasurement &other );
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
    Units mUnits;

};


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
    QgsLayoutSize( const double width, const double height, const QgsLayoutMeasurement::Units units = QgsLayoutMeasurement::Millimeters );

    QgsLayoutSize( const QgsLayoutSize &other );

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
    QgsLayoutMeasurement::Units units() const { return mUnits; }

    /**Sets the units for the size. Does not alter the stored width or height,
     * ie. no conversion is done.
     * @param units new size units
     * @see units
    */
    void setUnits( const QgsLayoutMeasurement::Units units ) { mUnits = units; }

    /**Converts the layout size to a QSizeF.
     * @returns QSizeF with same dimensions as layout size
    */
    QSizeF toQSizeF() const;

    bool operator==( const QgsLayoutSize &other ) const;
    bool operator!=( const QgsLayoutSize &other ) const;
    QgsLayoutSize & operator=( const QgsLayoutSize &other );
    QgsLayoutSize operator*( const double v ) const;
    QgsLayoutSize operator*=( const double v );
    QgsLayoutSize operator/( const double v ) const;
    QgsLayoutSize operator/=( const double v );

  private:

    double mWidth;
    double mHeight;
    QgsLayoutMeasurement::Units mUnits;

};


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
    QgsLayoutMeasurementConverter( const QgsLayoutMeasurementConverter &other );

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
    QgsLayoutMeasurement convert( const QgsLayoutMeasurement& measurement, const QgsLayoutMeasurement::Units targetUnits ) const;

    /**Converts a layout size from one unit to another.
     * @param size layout size to convert
     * @param targetUnits units to convert size into
     * @returns size converted to target units
    */
    QgsLayoutSize convert( const QgsLayoutSize& size, const QgsLayoutMeasurement::Units targetUnits ) const;

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

#endif // QGSLAYOUTMEASUREMENT_H
