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
    explicit QgsLayoutObject( QgsLayout* layout );
    virtual ~QgsLayoutObject();

    /**Returns the layout the item is attached to.
     * @returns QgsLayout for item.
     */
    const QgsLayout* layout() const { return mLayout; }
    QgsLayout* layout() { return mLayout; }

    /**Stores item state in DOM element
     * @param elem is DOM element corresponding to item tag
     * @param doc is the DOM document
     */
    //TODO
    virtual bool writeXML( QDomElement& elem, QDomDocument & doc ) const;

    /**Sets item state from DOM element
     * @param itemElem is DOM node corresponding to item tag
     * @param doc is DOM document
     */
    //TODO
    virtual bool readXML( const QDomElement& itemElem, const QDomDocument& doc );

    /**Returns a reference to the data defined settings for one of the item's data defined properties.
     * @param property data defined property to return
     * @note this method was added in version 2.5
    */
    //TODO
    QgsDataDefined* dataDefinedProperty( const DataDefinedProperty property ) const;

    /**Sets parameters for a data defined property for the item
     * @param property data defined property to set
     * @param active true if data defined property is active, false if it is disabled
     * @param useExpression true if the expression should be used
     * @param expression expression for data defined property
     * @param field field name if the data defined property should take its value from a field
     * @note this method was added in version 2.5
    */
    //TODO
    void setDataDefinedProperty( const DataDefinedProperty property, const bool active, const bool useExpression, const QString &expression, const QString &field );

  public slots:

    /**Triggers a redraw for the item*/
    //TODO
    virtual void repaint();

    /**Refreshes a data defined property for the item by reevaluating the property's value
     * and redrawing the item with this new value.
     * @param property data defined property to refresh. If property is set to
     * QgsLayoutObject::AllProperties then all data defined properties for the item will be
     * refreshed.
     * @note this method was added in version 2.5
    */
    //TODO
    virtual void refreshDataDefinedProperty( const DataDefinedProperty property = AllProperties );

  protected:

    QgsLayout* mLayout;

    /**Map of data defined properties for the item to string name to use when exporting item to xml*/
    QMap< QgsLayoutObject::DataDefinedProperty, QString > mDataDefinedNames;

    /**Evaluate a data defined property and return the calculated value
     * @returns true if data defined property could be successfully evaluated
     * @param property data defined property to evaluate
     * @param expressionValue QVariant for storing the evaluated value
     * @note this method was added in version 2.5
    */
    //TODO
    bool dataDefinedEvaluate( const QgsLayoutObject::DataDefinedProperty property, QVariant &expressionValue );

  signals:
    /**Emitted when the object changes. Signifies that the widgets must update their
     * gui elements.
    */
    //TODO
    void itemChanged();

  private slots:
    /**Prepares all layout object data defined expressions using the current atlas coverage layer if set.
     * @note this method was added in version 2.5
    */
    //TODO
    void prepareDataDefinedExpressions() const;

  private:

    /**Map of current data defined properties*/
    QMap< QgsLayoutObject::DataDefinedProperty, QgsDataDefined* > mDataDefinedProperties;

    friend class TestQgsLayoutObject;
};

#endif //QGSLAYOUTOBJECT_H
