/**\ingroup Layout
 * \class QgsLayoutMeasurement
 * \brief This class provides a method of storing measurements for use in QGIS layouts
 * using a variety of different measurement units.
 * \see QgsLayoutMeasurementConverter
 * \note added in QGIS 2.7
 */
class QgsLayoutMeasurement
{
%TypeHeaderCode
#include <qgslayoutmeasurement.h>
%End

  public:

    /** Units for measurement
     */
    enum Units
    {
      Millimeters, /*< millimetres */
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
    double length() const;

    /**Sets length of the measurement.
     * @param length new measurement length
     * @see length
    */
    void setLength( const double length );

    /**Units for the measurement.
     * @returns measurement units
     * @see setUnits
    */
    Units units() const;

    /**Sets the units for the measurement. Does not alter the stored length,
     * ie. no length conversion is done.
     * @param units new measurement units
     * @see units
    */
    void setUnits( const Units units );

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
};

/**\ingroup Layout
 * \class QgsLayoutSize
 * \brief This class provides a method of storing sizes, consisting of a width and height,
 * for use in QGIS layouts. Measurement units are stored alongside the size.
 * \see QgsLayoutMeasurementConverter
 * \note added in QGIS 2.7
 */
class QgsLayoutSize
{
%TypeHeaderCode
#include <qgslayoutmeasurement.h>
%End

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
    void setSize( const double width, const double height );

    /**Width of the size.
     * @returns width
     * @see setWidth
     * @see height
    */
    double width() const;

    /**Sets width for the size.
     * @param width new width
     * @see width
     * @see setHeight
    */
    void setWidth( const double width );

    /**Height of the size.
     * @returns height
     * @see setHeight
     * @see width
    */
    double height() const;

    /**Sets height for the size.
     * @param height new height
     * @see height
     * @see setWidth
    */
    void setHeight( const double height );

    /**Units for the size.
     * @returns size units
     * @see setUnits
    */
    QgsLayoutMeasurement::Units units() const;

    /**Sets the units for the size. Does not alter the stored width or height,
     * ie. no conversion is done.
     * @param units new size units
     * @see units
    */
    void setUnits( const QgsLayoutMeasurement::Units units );

    bool operator==( const QgsLayoutSize &other ) const;
    bool operator!=( const QgsLayoutSize &other ) const;
    QgsLayoutSize operator*( const double v ) const;
    QgsLayoutSize operator*=( const double v );
    QgsLayoutSize operator/( const double v ) const;
    QgsLayoutSize operator/=( const double v );

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
class QgsLayoutMeasurementConverter
{
%TypeHeaderCode
#include <qgslayoutmeasurement.h>
%End

  public:

    QgsLayoutMeasurementConverter();
    QgsLayoutMeasurementConverter( const QgsLayoutMeasurementConverter &other );

    virtual ~QgsLayoutMeasurementConverter();

    /**Sets the dots per inch (DPI) for the measurement converter. This is used
     * when converting measurements to and from pixels.
     * @param dpi dots per inch for conversion
     * @see dpi
    */
    void setDpi( const double dpi );

    /**Dots per inch (DPI) of the measurement converter. This is used
     * when converting measurements to and from pixels.
     * @returns dots per inch for conversion
     * @see setDpi
    */
    double dpi() const;

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
};
