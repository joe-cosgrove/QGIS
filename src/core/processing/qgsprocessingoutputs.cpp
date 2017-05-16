/***************************************************************************
                         qgsprocessingoutputs.cpp
                         -------------------------
    begin                : May 2017
    copyright            : (C) 2017 by Nyall Dawson
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

#include "qgsprocessingoutputs.h"

QgsProcessingOutputDefinition::QgsProcessingOutputDefinition( const QString &name, const QString &description, const QVariant &defaultValue, bool optional )
  : QgsProcessingParameterDefinition( name, description, defaultValue, optional )
{

}

QgsProcessingOutputVectorLayer::QgsProcessingOutputVectorLayer( const QString &name, const QString &description, const QVariant &defaultValue, bool optional )
  : QgsProcessingOutputDefinition( name, description, defaultValue, optional )
{

}
