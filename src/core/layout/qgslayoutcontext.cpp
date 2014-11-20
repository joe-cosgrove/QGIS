/***************************************************************************
                             qgslayoutcontext.cpp
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

#include "qgslayoutcontext.h"
#include "qgsfeature.h"


QgsLayoutContext::QgsLayoutContext()
    : QObject( 0 )
    , mFlags( Antialiasing | UseAdvancedEffects )
    , mFeature( 0 )
    , mLayer( 0 )
{

}

QgsLayoutContext::~QgsLayoutContext()
{
  delete mFeature;
}

void QgsLayoutContext::setFlags( const QgsLayoutContext::Flags flags )
{
  mFlags = flags;
}

void QgsLayoutContext::setFlag( const QgsLayoutContext::Flag flag, const bool on )
{
  if ( on )
    mFlags |= flag;
  else
    mFlags &= ~flag;
}

QgsLayoutContext::Flags QgsLayoutContext::flags() const
{
  return mFlags;
}

bool QgsLayoutContext::testFlag( const QgsLayoutContext::Flag flag ) const
{
  return mFlags.testFlag( flag );
}

void QgsLayoutContext::setFeature( const QgsFeature *feature )
{
  if ( !mFeature && !feature )
  {
    return;
  }

  delete mFeature;
  mFeature = 0;

  if ( !feature )
  {
    emit featureChanged( NULL );
  }
  else
  {
    //make a copy of the feature
    mFeature = new QgsFeature( *feature );
    emit featureChanged( mFeature );
  }
}

void QgsLayoutContext::setLayer( QgsVectorLayer *layer )
{
  if ( layer == mLayer )
  {
    return;
  }

  mLayer = layer;
  emit layerChanged( mLayer );
}
