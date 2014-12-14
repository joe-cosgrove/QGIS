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
class QgsFeature;
class QgsVectorLayer;

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

    /**Stores object properties within an XML DOM element.
     * @param parentElement is the parent DOM element to store the object's properties in
     * @param document DOM document
     * @returns true if write was successful
     * @see readObjectPropertiesFromElement
     */
    bool writeObjectPropertiesToElement( QDomElement& parentElement, QDomDocument& document ) const;

    /**Sets object properties from a DOM element
     * @param parentElement is the parent DOM element for the object
     * @param document DOM document
     * @returns true if read was successful
     * @see writeObjectPropertiesToElement
     */
    bool readObjectPropertiesFromElement( const QDomElement& parentElement, const QDomDocument& document );

    /**Returns a reference to the data defined settings for one of the item's data defined properties.
     * @param property data defined property to return
     * @note this method was added in version 2.5
    */
    QgsDataDefined* dataDefinedProperty( const DataDefinedProperty property ) const;

    /**Sets parameters for a data defined property for the item
     * @param property data defined property to set
     * @param dataDefined data defined container for property. Ownership of dataDefined will be
     * transferred to the layout object. Set to NULL to clear the property.
    */
    void setDataDefinedProperty( const DataDefinedProperty property, QgsDataDefined *dataDefined );

  public slots:

    /**Triggers a redraw for the item*/
    //TODO
    virtual void repaint();

    /**Refreshes a data defined property for the item by reevaluating the property's value
     * and redrawing the item with this new value.
     * @param property data defined property to refresh. If property is set to
     * QgsLayoutObject::AllProperties then all data defined properties for the item will be
     * refreshed.
    */
    //TODO
    virtual void refreshDataDefinedProperty( const DataDefinedProperty property = AllProperties );

  protected:

    QgsLayout* mLayout;

    /**Map of data defined properties for the item to string name to use when exporting item to xml*/
    QMap< QgsLayoutObject::DataDefinedProperty, QString > mDataDefinedNames;

    /**Evaluate a data defined property and return the calculated value
     * @param property data defined property to evaluate
     * @param expressionValue QVariant for storing the evaluated value
     * @returns true if data defined property could be successfully evaluated
    */
    bool evaluateDataDefinedProperty( const DataDefinedProperty property, QVariant &expressionValue );

    /**Returns a double value after a data defined property override has been applied.
     * If the data defined property is not set, not active, or evaluates to null, then the original
     * value will be returned. If the data defined property can be successfully calculated,
     * then the calculated value will be returned.
     * @param originalValue original value of property
     * @param property data defined property to apply
     * @returns either the calculated data defined value or the original value, depending on
     * the success of evaluating the data defined property.
    */
    double applyDataDefinedProperty( const double originalValue, const DataDefinedProperty property );

    /**Returns a double value after a data defined property override has been applied.
     * If the data defined property is not set or not active, then the original
     * value will be returned. If the data defined property can be successfully calculated,
     * then the calculated value will be returned. If the evaluated property is null,
     * then the value of valueIfNull will be returned.
     * @param originalValue original value of property
     * @param property data defined property to apply
     * @param valueIfNull value to returned if evaluated property is null
     * @returns either the calculated data defined value, null value, or the original value, depending on
     * the success of evaluating the data defined property.
    */
    double applyDataDefinedProperty( const double originalValue, const DataDefinedProperty property, const double valueIfNull );

    /**Writes data defined properties to xml
     * @param element DOM element in which to store data defined properties
     * @param document DOM document
     * @see readDataDefinedPropertyMap
    */
    void writeDataDefinedPropertyMap( QDomElement &element, QDomDocument &document ) const;

    /**Reads a single data defined property from xml DOM element
     * @param property data defined property to read
     * @param element dom element containing settings for data defined property
     * @returns true if property was successfully read from xml
     * @see readDataDefinedPropertyMap
    */
    bool readDataDefinedProperty( const QgsLayoutObject::DataDefinedProperty property, const QDomElement &element );

    /**Reads all data defined properties from xml
     * @param element DOM element containing data defined properties
     * @see readDataDefinedProperty
     * @see writeDataDefinedPropertyMap
    */
    void readDataDefinedPropertyMap( const QDomElement &element );

  signals:
    /**Emitted when the object changes. Signifies that the widgets must update their
     * gui elements.
    */
    //TODO
    void itemChanged();

  private slots:
    /**Prepares all layout object data defined expressions using the current layout context layer if set.
    */
    void prepareDataDefinedExpressions() const;

  private:

    /**Map of current data defined properties*/
    QMap< QgsLayoutObject::DataDefinedProperty, QgsDataDefined* > mDataDefinedProperties;

    bool propertyIsValid( const QgsLayoutObject::DataDefinedProperty property ) const;
    bool hasDataDefinedProperty( const QgsLayoutObject::DataDefinedProperty property ) const;
    bool shouldEvaluateDataDefinedProperty( const QgsDataDefined *dd ) const;
    QVariant dataDefinedValue( QgsDataDefined *dd, const QgsFeature* feature, QgsVectorLayer *layer = 0 ) const;

    friend class TestQgsLayoutObject;
};

#endif //QGSLAYOUTOBJECT_H
