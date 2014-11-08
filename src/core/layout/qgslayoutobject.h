/***************************************************************************
                         qgslayoutobject.h
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
#ifndef QGSLAYOUTOBJECT_H
#define QGSLAYOUTOBJECT_H

#include <QObject>
#include <QDomNode>
#include <QMap>

class QgsLayout;
class QPainter;
class QgsDataDefined;

/** \ingroup Layout
 * A base class for objects which belong to a layout.
 */
class CORE_EXPORT QgsLayoutObject: public QObject
{
    Q_OBJECT
  public:

    /** Data defined properties for different item types
     */
    enum DataDefinedProperty
    {
      NoProperty = 0, /*< no property */
      AllProperties, /*< all properties for item */
      TestProperty, /*< dummy property with no effect on item*/
      //layout page properties
      PresetPaperSize, /*< preset paper size for layout */
      PaperWidth, /*< paper width */
      PaperHeight, /*< paper height */
      NumPages, /*< number of pages in layout */
      PaperOrientation, /*< paper orientation */
      //general layout item properties
      PageNumber, /*< page number for item placement */
      PositionX, /*< x position on page */
      PositionY, /*< y position on page */
      ItemWidth, /*< width of item */
      ItemHeight, /*< height of item */
      ItemRotation, /*< rotation of item */
      Transparency, /*< item transparency */
      BlendMode, /*< item blend mode */
      ExcludeFromExports, /*< exclude item from exports */
      //map item
      MapRotation, /*< map rotation */
      MapScale, /*< map scale */
      MapXMin, /*< map extent x minimum */
      MapYMin, /*< map extent y minimum */
      MapXMax, /*< map extent x maximum */
      MapYMax, /*< map extent y maximum */
      MapAtlasMargin, /*< map atlas margin*/
      //picture item
      PictureSource, /*< picture source url */
      //html item
      SourceUrl /*< html source utl */
    };

    /** Specifies whether the value returned by a function should be the original, user
     * set value, or the current evaluated value for the property. This may differ if
     * a property has a data defined expression active.
     */
    enum PropertyValueType
    {
      EvaluatedValue = 0, /*< return the current evaluated value for the property */
      OriginalValue /*< return the original, user set value */
    };

    /**Constructor
     * @param layout parent layout.
     * @note while ownership of a QgsLayoutObject is not passed to the layout,
     * classes which are derived from QgsLayoutObject (such as QgsLayoutItem)
     * may transfer their ownership to a layout upon construction.
     */
    QgsLayoutObject( QgsLayout* layout );
    virtual ~QgsLayoutObject();

    /**Returns the layout the item is attached to.
     * @returns QgsLayout for item.
     */
    const QgsLayout* layout() const { return mLayout; }
    QgsLayout* layout() { return mLayout; }

  protected:

    QgsLayout* mLayout;

  private:

    friend class TestQgsLayoutObject;
};

#endif //QGSLAYOUTOBJECT_H
