/***************************************************************************
                              qgslayout.h
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
#ifndef QGSLAYOUT_H
#define QGSLAYOUT_H

#include <QGraphicsScene>
#include "qgslayoutmeasurement.h"

/**\ingroup Layout
 * \class QgsLayout
 * \brief Base class for layouts, which can contain items such as maps, labels, scalebars, etc.
 * \note added in QGIS 2.7
 */
class CORE_EXPORT QgsLayout : public QGraphicsScene
{
    Q_OBJECT

  public:

    /**Flags for controlling how a layout is rendered
     */
    enum Flag
    {
      Debug       = 0x01,  /*< debug/testing mode, items are drawn as solid rectangles */
      OutlineOnly    = 0x02, /*< render items as outlines only */
      Antialiasing  = 0x04, /*< use antialiasing when drawing items  */
      UseAdvancedEffects = 0x08 /*< enable advanced effects such as blend modes */
    };
    Q_DECLARE_FLAGS( Flags, Flag )

    QgsLayout();

    virtual ~QgsLayout();

    /**Set combination of flags that will be used for rendering the layout
     * @param flags rendering flags
     * @see setFlag
     * @see flags
     * @see testFlag
     */
    void setFlags( const QgsLayout::Flags flags );

    /**Enable or disable a particular rendering flag for the layout. Other existing
     * flags are not affected.
     * @param flag rendering flag to set
     * @param on set to true to enable flag
     * @see setFlags
     * @see flags
     * @see testFlag
     */
    void setFlag( const Flag flag, const bool on = true );

    /**Returns the current combination of flags used for rendering the layout.
     * @returns layout rendering flags
     * @see setFlags
     * @see setFlag
     * @see testFlag
     */
    Flags flags() const;

    /**Check whether a particular rendering flag is enabled for the layout.
     * @param flag flag to test
     * @returns true if flag is enabled
     * @see setFlags
     * @see setFlag
     * @see flags
     */
    bool testFlag( const Flag flag ) const;

    /**Sets the name for the layout.
     * @param name new name
     * @see name
    */
    void setName( const QString& name );

    /**Name of the layout.
     * @returns layout name
     * @see setName
    */
    QString name() const { return mName; }

    /**Sets the native measurement unit for the layout. These also form the default unit
     * for measurements for the layout.
     * @param units measurement units
     * @see units
     * @see convertToLayoutUnits
    */
    void setUnits( const QgsLayoutMeasurement::Units& units );

    /**Native units for the layout.
     * @returns native measurement units
     * @see setUnits
     * @see convertToLayoutUnits
    */
    QgsLayoutMeasurement::Units units() const { return mUnits; }

    /**Sets the resolution of a layout.
     * @param dpi resolution of layout in dots per inch
     * @see dpi
     * @see measurementConverter
    */
    void setDpi( const double dpi );

    /**Resolution of the layout.
     * @returns resolution of layout in dots per inch
     * @see setDpi
     * @see measurementConverter
    */
    double dpi() const;

    /**Returns a pointer to the measurement converter used by the layout. This converter is used
     * for translating between other measurement units and the layout's native unit.
     * @returns layout's measurement converter
     * @see dpi
     * @see units
     * @see convertToLayoutUnits
    */
    const QgsLayoutMeasurementConverter* measurementConverter() const;

    /**Converts a measurement into the layout's native units.
     * @param measurement measurement to convert
     * @returns length of measurement in layout units
     * @see measurementConverter
     * @see convertFromLayoutUnits
     * @see units
    */
    double convertToLayoutUnits( const QgsLayoutMeasurement& measurement ) const;

    /**Converts a size into the layout's native units.
     * @param size size to convert
     * @returns size of measurement in layout units
     * @see measurementConverter
     * @see convertFromLayoutUnits
     * @see units
    */
    QSizeF convertToLayoutUnits( const QgsLayoutSize& size ) const;

    /**Converts a point into the layout's native units.
     * @param point point to convert
     * @returns point in layout units
     * @see measurementConverter
     * @see convertFromLayoutUnits
     * @see units
    */
    QPointF convertToLayoutUnits( const QgsLayoutPoint& point ) const;

    /**Converts a measurement from the layout's native units to a specified unit.
     * @param length length in layout units to convert
     * @param units target units
     * @returns length of measurement in specified units
     * @see measurementConverter
     * @see convertToLayoutUnits
     * @see units
    */
    QgsLayoutMeasurement convertFromLayoutUnits( const double length, const QgsLayoutMeasurement::Units units ) const;

    /**Converts a size from the layout's native units to a specified unit.
     * @param size size in layout units to convert
     * @param units target units
     * @returns size of measurement in specified units
     * @see measurementConverter
     * @see units
    */
    QgsLayoutSize convertFromLayoutUnits( const QSizeF& size, const QgsLayoutMeasurement::Units units ) const;

    /**Converts a point from the layout's native units to a specified unit.
     * @param point point in layout units to convert
     * @param units target units
     * @returns point in specified units
     * @see measurementConverter
     * @see units
    */
    QgsLayoutPoint convertFromLayoutUnits( const QPointF& point, const QgsLayoutMeasurement::Units units ) const;

  signals:

    void dpiChanged( const double dpi );

  private:

    Flags mFlags;

    QString mName;
    QgsLayoutMeasurement::Units mUnits;
    QgsLayoutMeasurementConverter* mMeasurementConverter;

};

#endif //QGSLAYOUT_H



