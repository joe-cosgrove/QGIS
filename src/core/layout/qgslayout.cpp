/***************************************************************************
                              qgslayout.cpp
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

#include "qgslayout.h"
#include "qgspagecollection.h"

QgsLayout::QgsLayout()
    : QGraphicsScene()
    , mFlags( Antialiasing | UseAdvancedEffects )
    , mUnits( QgsLayoutMeasurement::Millimeters )
    , mMeasurementConverter( 0 )
{
  mMeasurementConverter = new QgsLayoutMeasurementConverter();
  mMeasurementConverter->setDpi( 300.0 );
}

QgsLayout::~QgsLayout()
{
  delete mMeasurementConverter;
}

void QgsLayout::setFlags( const Flags flags )
{
  mFlags = flags;
}

void QgsLayout::setFlag( const QgsLayout::Flag flag, const bool on )
{
  if ( on )
    mFlags |= flag;
  else
    mFlags &= ~flag;
}

QgsLayout::Flags QgsLayout::flags() const
{
  return mFlags;
}

bool QgsLayout::testFlag( const QgsLayout::Flag flag ) const
{
  return mFlags.testFlag( flag );
}

void QgsLayout::setName( const QString &name )
{
  mName = name;
}

void QgsLayout::setUnits( const QgsLayoutMeasurement::Units &units )
{
  mUnits = units;
}

void QgsLayout::setDpi( const double dpi )
{
  if ( dpi == mMeasurementConverter->dpi() )
  {
    return;
  }
  mMeasurementConverter->setDpi( dpi );
}

double QgsLayout::dpi() const
{
  return mMeasurementConverter->dpi();
}

const QgsLayoutMeasurementConverter *QgsLayout::measurementConverter() const
{
  return mMeasurementConverter;
}

double QgsLayout::convertToLayoutUnits( const QgsLayoutMeasurement &measurement ) const
{
  return mMeasurementConverter->convert( measurement, mUnits ).length();
}

QSizeF QgsLayout::convertToLayoutUnits( const QgsLayoutSize &size ) const
{
  return mMeasurementConverter->convert( size, mUnits ).toQSizeF();
}


