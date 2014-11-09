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
    //connect to atlas toggling on/off and coverage layer and feature changes
    //to update data defined values
//    connect( &mLayout->atlasComposition(), SIGNAL( toggled( bool ) ), this, SLOT( refreshDataDefinedProperty() ) );
//    connect( &mLayout->atlasComposition(), SIGNAL( coverageLayerChanged( QgsVectorLayer* ) ), this, SLOT( refreshDataDefinedProperty() ) );
//    connect( &mLayout->atlasComposition(), SIGNAL( featureChanged( QgsFeature* ) ), this, SLOT( refreshDataDefinedProperty() ) );
    //also, refreshing composition triggers a recalculation of data defined properties
//    connect( mLayout, SIGNAL( refreshItemsTriggered() ), this, SLOT( refreshDataDefinedProperty() ) );

    //toggling atlas or changing coverage layer requires data defined expressions to be reprepared
//    connect( &mLayout->atlasComposition(), SIGNAL( toggled( bool ) ), this, SLOT( prepareDataDefinedExpressions() ) );
//    connect( &mLayout->atlasComposition(), SIGNAL( coverageLayerChanged( QgsVectorLayer* ) ), this, SLOT( prepareDataDefinedExpressions() ) );
  }

}

QgsLayoutObject::~QgsLayoutObject()
{

}

bool QgsLayoutObject::writeXML( QDomElement &elem, QDomDocument &doc ) const
{
  if ( elem.isNull() )
  {
    return false;
  }

  //data defined properties
  QgsLayoutUtils::writeDataDefinedPropertyMap( elem, doc, &mDataDefinedNames, &mDataDefinedProperties );

  return true;
}

bool QgsLayoutObject::readXML( const QDomElement &itemElem, const QDomDocument &doc )
{
  Q_UNUSED( doc );
  if ( itemElem.isNull() )
  {
    return false;
  }

  //data defined properties
  QgsLayoutUtils::readDataDefinedPropertyMap( itemElem, &mDataDefinedNames, &mDataDefinedProperties );

  return true;
}

QgsDataDefined *QgsLayoutObject::dataDefinedProperty( const QgsLayoutObject::DataDefinedProperty property ) const
{
  if ( property == QgsLayoutObject::AllProperties || property == QgsLayoutObject::NoProperty )
  {
    //bad property requested, don't return anything
    return 0;
  }

  //find corresponding QgsDataDefined and return it
  QMap< QgsLayoutObject::DataDefinedProperty, QgsDataDefined* >::const_iterator it = mDataDefinedProperties.find( property );
  if ( it != mDataDefinedProperties.constEnd() )
  {
    return it.value();
  }

  //could not find matching QgsDataDefined
  return 0;
}

void QgsLayoutObject::setDataDefinedProperty( const QgsLayoutObject::DataDefinedProperty property, const bool active, const bool useExpression, const QString &expression, const QString &field )
{
  if ( property == QgsLayoutObject::AllProperties || property == QgsLayoutObject::NoProperty )
  {
    //bad property requested
    return;
  }

  bool defaultVals = ( !active && !useExpression && expression.isEmpty() && field.isEmpty() );

  if ( mDataDefinedProperties.contains( property ) )
  {
    QMap< QgsLayoutObject::DataDefinedProperty, QgsDataDefined* >::const_iterator it = mDataDefinedProperties.find( property );
    if ( it != mDataDefinedProperties.constEnd() )
    {
      QgsDataDefined* dd = it.value();
      dd->setActive( active );
      dd->setUseExpression( useExpression );
      dd->setExpressionString( expression );
      dd->setField( field );
    }
  }
  else if ( !defaultVals )
  {
    QgsDataDefined* dd = new QgsDataDefined( active, useExpression, expression, field );
    mDataDefinedProperties.insert( property, dd );
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

bool QgsLayoutObject::dataDefinedEvaluate( const DataDefinedProperty property, QVariant &expressionValue )
{
  if ( !mLayout )
  {
    return false;
  }
  //return mComposition->dataDefinedEvaluate( property, expressionValue, &mDataDefinedProperties );
}

void QgsLayoutObject::prepareDataDefinedExpressions() const
{
  //use atlas coverage layer if set
  QgsVectorLayer* atlasLayer = 0;
  if ( mLayout )
  {
    /*    QgsAtlasComposition* atlas = &mLayout->atlasComposition();
        if ( atlas && atlas->enabled() )
        {
          atlasLayer = atlas->coverageLayer();
        }*/
  }

  //prepare all QgsDataDefineds
  QMap< DataDefinedProperty, QgsDataDefined* >::const_iterator it = mDataDefinedProperties.constBegin();
  if ( it != mDataDefinedProperties.constEnd() )
  {
    it.value()->prepareExpression( atlasLayer );
  }
}
