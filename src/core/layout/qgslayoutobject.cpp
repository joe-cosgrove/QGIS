/***************************************************************************
                            qgslayoutobject.cpp
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

#include <QPainter>

#include "qgslayout.h"
#include "qgslayoutcontext.h"
#include "qgslayoututils.h"
#include "qgslayoutobject.h"
#include "qgsdatadefined.h"

#define FONT_WORKAROUND_SCALE 10 //scale factor for upscaling fontsize and downscaling painter

#ifndef M_DEG2RAD
#define M_DEG2RAD 0.0174532925
#endif

QgsLayoutObject::QgsLayoutObject( QgsLayout* layout )
    : QObject( 0 )
    , mLayout( layout )
{

  // data defined strings
  mDataDefinedNames.insert( QgsLayoutObject::TestProperty, QString( "dataDefinedTestProperty" ) );

  if ( mLayout )
  {
    //TODO

    //connect to atlas toggling on/off and coverage layer and feature changes
    //to update data defined values
//    connect( &mLayout->atlasComposition(), SIGNAL( toggled( bool ) ), this, SLOT( refreshDataDefinedProperty() ) );
//    connect( &mLayout->atlasComposition(), SIGNAL( coverageLayerChanged( QgsVectorLayer* ) ), this, SLOT( refreshDataDefinedProperty() ) );
//    connect( &mLayout->atlasComposition(), SIGNAL( featureChanged( QgsFeature* ) ), this, SLOT( refreshDataDefinedProperty() ) );
    //also, refreshing composition triggers a recalculation of data defined properties
//    connect( mLayout, SIGNAL( refreshItemsTriggered() ), this, SLOT( refreshDataDefinedProperty() ) );

    //toggling changing layout's context layer requires data defined expressions to be reprepared
    connect( mLayout->context(), SIGNAL( layerChanged( QgsVectorLayer* ) ), this, SLOT( prepareDataDefinedExpressions() ) );
  }

}

QgsLayoutObject::~QgsLayoutObject()
{
  qDeleteAll( mDataDefinedProperties );
}

bool QgsLayoutObject::writeObjectPropertiesToElement( QDomElement &parentElement, QDomDocument &document ) const
{
  if ( parentElement.isNull() )
  {
    return false;
  }

  //create object element
  QDomElement objectElement = document.createElement( "LayoutObject" );

  //data defined properties
  writeDataDefinedPropertyMap( objectElement, document );

  parentElement.appendChild( objectElement );
  return true;
}

bool QgsLayoutObject::readObjectPropertiesFromElement( const QDomElement& parentElement, const QDomDocument& document )
{
  Q_UNUSED( document );
  if ( parentElement.isNull() )
  {
    return false;
  }

  QDomNodeList objectNodeList = parentElement.elementsByTagName( "LayoutObject" );
  if ( objectNodeList.size() < 1 )
  {
    return false;
  }
  QDomElement objectElement = objectNodeList.at( 0 ).toElement();

  //data defined properties
  readDataDefinedPropertyMap( objectElement );
  return true;
}

bool QgsLayoutObject::propertyIsValid( const QgsLayoutObject::DataDefinedProperty property ) const
{
  return ( property != QgsLayoutObject::AllProperties && property != QgsLayoutObject::NoProperty );
}

bool QgsLayoutObject::hasDataDefinedProperty( const QgsLayoutObject::DataDefinedProperty property ) const
{
  return ( mDataDefinedProperties.contains( property ) );
}

QgsDataDefined *QgsLayoutObject::dataDefinedProperty( const QgsLayoutObject::DataDefinedProperty property ) const
{
  if ( !propertyIsValid( property ) || !hasDataDefinedProperty( property ) )
  {
    return NULL;
  }

  return mDataDefinedProperties.value( property );
}

void QgsLayoutObject::setDataDefinedProperty( const QgsLayoutObject::DataDefinedProperty property, QgsDataDefined* dataDefined )
{
  if ( !propertyIsValid( property ) )
  {
    return;
  }

  if ( hasDataDefinedProperty( property ) )
  {
    delete mDataDefinedProperties.take( property );
  }
  if ( !dataDefined )
  {
    return;
  }
  else if ( !dataDefined->hasDefaultValues() )
  {
    mDataDefinedProperties.insert( property, dataDefined );
  }
  else
  {
    //since ownership was transferred to object and we are not using the data defined, delete it
    delete dataDefined;
  }
}

void QgsLayoutObject::repaint()
{
  //nothing to do in base class for now
}

void QgsLayoutObject::refreshDataDefinedProperty( const DataDefinedProperty property )
{
  Q_UNUSED( property );

  //nothing to do in base class for now
}

bool QgsLayoutObject::evaluateDataDefinedProperty( const DataDefinedProperty property, QVariant &expressionValue )
{
  if ( !mLayout || !propertyIsValid( property ) || !hasDataDefinedProperty( property ) )
  {
    return false;
  }

  //null passed-around QVariant
  expressionValue.clear();

  //get layer and feature from context
  const QgsFeature* currentFeature = mLayout->context()->feature();
  QgsVectorLayer* layer = mLayout->context()->layer();

  //evaluate data defined property using current context
  QgsDataDefined* dd = mDataDefinedProperties.value( property );
  QVariant result = dataDefinedValue( dd, currentFeature, layer );

  if ( result.isValid() )
  {
    expressionValue = result;
    return true;
  }
  else
  {
    return false;
  }
}

double QgsLayoutObject::applyDataDefinedProperty( const double originalValue, const QgsLayoutObject::DataDefinedProperty property )
{
  return applyDataDefinedProperty( originalValue, property, originalValue );
}

double QgsLayoutObject::applyDataDefinedProperty( const double originalValue, const QgsLayoutObject::DataDefinedProperty property, const double valueIfNull )
{
  QVariant evaluatedValue;
  if ( evaluateDataDefinedProperty( property, evaluatedValue ) )
  {
    if ( evaluatedValue.isNull() )
    {
      return valueIfNull;
    }

    bool ok;
    double convertedEvaluatedValue = evaluatedValue.toDouble( &ok );
    if ( ok )
    {
      return convertedEvaluatedValue;
    }
    else
    {
      return valueIfNull;
    }
  }
  return originalValue;
}

void QgsLayoutObject::writeDataDefinedPropertyMap( QDomElement &element, QDomDocument &document ) const
{
  QMap<QgsLayoutObject::DataDefinedProperty, QString >::const_iterator i = mDataDefinedNames.constBegin();
  for ( ; i != mDataDefinedNames.constEnd(); ++i )
  {
    QString name = i.value();
    QgsDataDefined* dd = dataDefinedProperty( i.key() );
    if ( dd && !dd->hasDefaultValues() )
    {
      element.appendChild( dd->toXmlElement( document, name ) );
    }
  }
}

void QgsLayoutObject::readDataDefinedPropertyMap( const QDomElement &element )
{
  QMap<QgsLayoutObject::DataDefinedProperty, QString>::const_iterator i = mDataDefinedNames.constBegin();
  for ( ; i != mDataDefinedNames.constEnd(); ++i )
  {
    QString elemName = i.value();
    QDomNodeList ddNodeList = element.elementsByTagName( elemName );
    if ( ddNodeList.size() > 0 )
    {
      QDomElement ddElem = ddNodeList.at( 0 ).toElement();
      readDataDefinedProperty( i.key(), ddElem );
    }
  }
}

bool QgsLayoutObject::readDataDefinedProperty( const QgsLayoutObject::DataDefinedProperty property, const QDomElement &element )
{
  if ( !propertyIsValid( property ) || element.isNull() )
  {
    return false;
  }

  QgsDataDefined* dd = 0;
  if ( hasDataDefinedProperty( property ) )
  {
    dd = dataDefinedProperty( property );
  }
  else
  {
    dd = new QgsDataDefined();
    mDataDefinedProperties.insert( property, dd );
  }

  return dd->setFromXmlElement( element );
}

bool QgsLayoutObject::shouldEvaluateDataDefinedProperty( const QgsDataDefined* dd ) const
{
  if ( !dd || dd->hasDefaultValues() || !dd->isActive() )
  {
    return false;
  }

  return true;
}

QVariant QgsLayoutObject::dataDefinedValue( QgsDataDefined* dd, const QgsFeature *feature, QgsVectorLayer* layer ) const
{
  if ( !shouldEvaluateDataDefinedProperty( dd ) )
  {
    return QVariant();
  }

  QVariant result = QVariant();
  if ( !dd->expressionIsPrepared() )
  {
    dd->prepareExpression( layer );
  }

  if ( dd->useExpression() && dd->expressionIsPrepared() )
  {
    result = dd->expression()->evaluate( feature );
    if ( dd->expression()->hasEvalError() )
    {
      QgsDebugMsgLevel( QString( "Evaluate error:" ) + dd->expression()->evalErrorString(), 4 );
      return QVariant();
    }
  }
  else if ( feature && !dd->useExpression() && !dd->field().isEmpty() && layer )
  {
    // use direct attribute access instead of evaluating "field" expression (much faster)
    int fieldIndex = layer->pendingFields().indexFromName( dd->field() );
    if ( fieldIndex != -1 )
    {
      result = feature->attribute( fieldIndex );
    }
  }
  return result;
}

void QgsLayoutObject::prepareDataDefinedExpressions() const
{
  //use layout context layer if set
  QgsVectorLayer* layer = 0;
  if ( mLayout )
  {
    layer = mLayout->context()->layer();
  }

  //prepare all QgsDataDefined containers
  QMap< DataDefinedProperty, QgsDataDefined* >::const_iterator it = mDataDefinedProperties.constBegin();
  if ( it != mDataDefinedProperties.constEnd() )
  {
    it.value()->prepareExpression( layer );
  }
}
