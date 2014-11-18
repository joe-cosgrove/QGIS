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
#include "qgslayoutsize.h"
#include "qgslayoutpoint.h"

class QgsLayoutContext;
class QgsLayoutMeasurementConverter;

/**\ingroup Layout
 * \class QgsLayout
 * \brief Base class for layouts, which can contain items such as maps, labels, scalebars, etc.
 * \note added in QGIS 2.7
 */
class CORE_EXPORT QgsLayout : public QGraphicsScene
{
    Q_OBJECT

  public:

    QgsLayout();

    virtual ~QgsLayout();

    /**Set combination of flags that will be used for rendering the layout
     * @param flags rendering flags
     * @see setFlag
     * @see flags
     * @see testFlag
     */

    QgsLayoutContext* context() { return mContext; }

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
    void setUnits( const QgsLayoutUnits::Units& units );

    /**Native units for the layout.
     * @returns native measurement units
     * @see setUnits
     * @see convertToLayoutUnits
    */
    QgsLayoutUnits::Units units() const { return mUnits; }

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
    QgsLayoutMeasurement convertFromLayoutUnits( const double length, const QgsLayoutUnits::Units units ) const;

    /**Converts a size from the layout's native units to a specified unit.
     * @param size size in layout units to convert
     * @param units target units
     * @returns size of measurement in specified units
     * @see measurementConverter
     * @see units
    */
    QgsLayoutSize convertFromLayoutUnits( const QSizeF& size, const QgsLayoutUnits::Units units ) const;

    /**Converts a point from the layout's native units to a specified unit.
     * @param point point in layout units to convert
     * @param units target units
     * @returns point in specified units
     * @see measurementConverter
     * @see units
    */
    QgsLayoutPoint convertFromLayoutUnits( const QPointF& point, const QgsLayoutUnits::Units units ) const;

  signals:

    void dpiChanged( const double dpi );

  private:

    QgsLayoutContext* mContext;

    QString mName;
    QgsLayoutUnits::Units mUnits;
    QgsLayoutMeasurementConverter* mMeasurementConverter;

};

#endif //QGSLAYOUT_H



