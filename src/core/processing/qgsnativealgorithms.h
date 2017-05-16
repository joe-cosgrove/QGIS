/***************************************************************************
                         qgsnativealgorithms.h
                         ---------------------
    begin                : April 2017
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

#ifndef QGSNATIVEALGORITHMS_H
#define QGSNATIVEALGORITHMS_H

#include "qgis_core.h"
#include "qgis.h"
#include "qgsprocessingalgorithm.h"
#include "qgsprocessingprovider.h"

///@cond PRIVATE

class QgsNativeAlgorithms: public QgsProcessingProvider
{
  public:

    QgsNativeAlgorithms( QObject *parent = nullptr );

    QIcon icon() const override;
    QString svgIconPath() const override;
    QString id() const override;
    QString name() const override;
    bool supportsNonFileBasedOutput() const override;

  protected:

    void loadAlgorithms() override;

};

/**
 * Native centroid algorithm.
 */
class QgsCentroidAlgorithm : public QgsProcessingAlgorithm
{

  public:

    QgsCentroidAlgorithm();

    QString name() const override { return QStringLiteral( "centroids" ); }
    QString displayName() const override { return QObject::tr( "Centroids" ); }
    virtual QStringList tags() const override { return QObject::tr( "centroid,center,average,point,middle" ).split( ',' ); }
    QString group() const override { return QObject::tr( "Vector geometry tools" ); }
    QString shortHelpString() const override;

    virtual QVariantMap run( const QVariantMap &parameters,
                             QgsProcessingContext &context, QgsProcessingFeedback *feedback ) const override;

};

/**
 * Native buffer algorithm.
 */
class QgsBufferAlgorithm : public QgsProcessingAlgorithm
{

  public:

    QgsBufferAlgorithm();

    QString name() const override { return QStringLiteral( "fixeddistancebuffer" ); }
    QString displayName() const override { return QObject::tr( "Buffer" ); }
    virtual QStringList tags() const override { return QObject::tr( "buffer,grow" ).split( ',' ); }
    QString group() const override { return QObject::tr( "Vector geometry tools" ); }
    QString shortHelpString() const override;

    virtual QVariantMap run( const QVariantMap &parameters,
                             QgsProcessingContext &context, QgsProcessingFeedback *feedback ) const override;

};

///@endcond PRIVATE

#endif // QGSNATIVEALGORITHMS_H


