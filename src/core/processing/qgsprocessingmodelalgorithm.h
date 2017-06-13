/***************************************************************************
                         qgsprocessingmodelalgorithm.h
                         -----------------------------
    begin                : June 2017
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

#ifndef QGSPROCESSINGMODELALGORITHM_H
#define QGSPROCESSINGMODELALGORITHM_H

#include "qgis_core.h"
#include "qgis.h"
#include "qgsprocessingalgorithm.h"


/**
 * \class QgsProcessingModelAlgorithm
 * \ingroup core
 * Model based algorithm with processing.
  * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingModelAlgorithm : public QgsProcessingAlgorithm
{
  public:

    /**
     * Constructor for QgsProcessingModelAlgorithm.
     */
    QgsProcessingModelAlgorithm( const QString &name = QString() );

    QString name() const override;
    QString displayName() const override;

  private:

    QString mModelName;
}

#endif // QGSPROCESSINGMODELALGORITHM_H


