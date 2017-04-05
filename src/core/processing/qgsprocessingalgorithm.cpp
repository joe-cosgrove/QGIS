/***************************************************************************
                         qgsprocessingalgorithm.cpp
                         --------------------------
    begin                : December 2016
    copyright            : (C) 2016 by Nyall Dawson
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

#include "qgsprocessingalgorithm.h"
#include "qgsapplication.h"
#include "qgsprocessingprovider.h"

QgsProcessingAlgorithm::~QgsProcessingAlgorithm()
{
  qDeleteAll( mParameters );
}

QString QgsProcessingAlgorithm::id() const
{
  if ( mProvider )
    return QString( "%1:%2" ).arg( mProvider->id(), name() );
  else
    return name();
}

QIcon QgsProcessingAlgorithm::icon() const
{
  return QgsApplication::getThemeIcon( "/processingAlgorithm.svg" );
}

QString QgsProcessingAlgorithm::svgIconPath() const
{
  return QgsApplication::iconPath( "processingAlgorithm.svg" );
}

QgsProcessingAlgorithm::Flags QgsProcessingAlgorithm::flags() const
{
  return FlagSupportsBatch;
}

QgsProcessingProvider *QgsProcessingAlgorithm::provider() const
{
  return mProvider;
}

bool QgsProcessingAlgorithm::addParameter( QgsProcessingParameter *parameter )
{
  if ( !parameter )
    return false;

  // check for duplicate named parameters
  Q_FOREACH ( const QgsProcessingParameter *p, mParameters )
  {
    if ( p->name() == parameter->name() )
      return false;
  }

  mParameters << parameter;
  return true;
}

void QgsProcessingAlgorithm::setProvider( QgsProcessingProvider *provider )
{
  mProvider = provider;
}
