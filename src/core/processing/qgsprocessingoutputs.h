/***************************************************************************
                         qgsprocessingoutputs.h
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

#ifndef QGSPROCESSINGOUTPUTS_H
#define QGSPROCESSINGOUTPUTS_H

#include "qgis_core.h"
#include "qgis.h"
#include "qgsprocessingparameters.h"

//
// Output definitions
//

/**
 * \class QgsProcessingOutputDefinition
 * \ingroup core
 *
 * Base class for the definition of processing outputs.
 *
 * Output definitions encapsulate the properties regarding the outputs from algorithms, such
 * as generated layers or calculated values.
 *
 * \since QGIS 3.0
 */

class CORE_EXPORT QgsProcessingOutputDefinition : public QgsProcessingParameterDefinition
{
  public:

    /**
     * Constructor for QgsProcessingOutputDefinition.
     */
    QgsProcessingOutputDefinition( const QString &name, const QString &description = QString(), const QVariant &defaultValue = QVariant(),
                                   bool optional = false );

};

//! List of processing parameters
typedef QList< const QgsProcessingOutputDefinition * > QgsProcessingOutputDefinitions;


/**
 * \class QgsProcessingOutputVectorLayer
 * \ingroup core
 * A vector layer output for processing algorithms.
  * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingOutputVectorLayer : public QgsProcessingOutputDefinition
{
  public:

    /**
     * Constructor for QgsProcessingOutputVectorLayer.
     */
    QgsProcessingOutputVectorLayer( const QString &name, const QString &description = QString(), const QVariant &defaultValue = QVariant(),
                                    bool optional = false );

    QString type() const override { return QStringLiteral( "outputVector" ); }

};

#endif // QGSPROCESSINGOUTPUTS_H


