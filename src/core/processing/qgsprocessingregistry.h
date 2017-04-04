/***************************************************************************
                         qgsprocessingregistry.h
                         ------------------------
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

#ifndef QGSPROCESSINGREGISTRY_H
#define QGSPROCESSINGREGISTRY_H

#include "qgis_core.h"
#include "qgis.h"
#include "qgsprocessingprovider.h"
#include <QMap>

class QgsProcessingParameter;

/**
 * \class QgsProcessingParameterAbstractMetadata
 * \ingroup core
 * Stores metadata about one processing parameter class.
 * \note In C++ you can use QgsProcessingParameterMetadata convenience class.
 * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingParameterAbstractMetadata
{
  public:

    /**
     * Constructor for parameter abstract metadata. The type parameter must be unique.
     */
    QgsProcessingParameterAbstractMetadata( const QString &type );

    virtual ~QgsProcessingParameterAbstractMetadata() = default;

    /**
     * Returns the unique type string which identifies the parameter type.
     */
    QString type() const { return mType; }

    /**
     * Creates a new parameter of this type from an encoded script code line. The name, description and optional flag will have already
     * been parsed when this function is called, so only any extra handling of the remaining definition string needs to be done by the class.
     */
    virtual QgsProcessingParameter *createParameterFromScriptCode( const QString &name, const QString &description, bool isOptional, const QString &definition );

  private:
    QString mType;
};

//! Function to create a parameter from an encoded script code line
typedef QgsProcessingParameter *( *QgsProcessingParameterFromScriptCodeFunc )( const QString &, const QString &, bool, const QString & );

#ifndef SIP_RUN

/**
 * \class QgsProcessingParameterMetadata
 * \ingroup core
 * Convenience class that uses static functions to create parameter metadata.
 * \note Not available in Python bindings
 * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingParameterMetadata : public QgsProcessingParameterAbstractMetadata
{
  public:

    /**
     * Constructor for QgsProcessingParameterMetadata which accepts static functions for the
     * parameter creation functions.
     */
    QgsProcessingParameterMetadata( const QString &type,
                                    QgsProcessingParameterFromScriptCodeFunc pfCreateFromScriptCode = nullptr );

    virtual QgsProcessingParameter *createParameterFromScriptCode( const QString &name, const QString &description, bool isOptional, const QString &definition ) override { return mCreateFromScriptCodeFunc ? mCreateFromScriptCodeFunc( name, description, isOptional, definition ) : nullptr; }

  private:
    QgsProcessingParameterFromScriptCodeFunc mCreateFromScriptCodeFunc;

};
#endif

/**
 * \class QgsProcessingRegistry
 * \ingroup core
 * Registry for various processing components, including providers, algorithms
 * and various parameters and outputs.
 *
 * QgsProcessingRegistry is not usually directly created, but rather accessed through
 * QgsApplication::processingRegistry().
 * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingRegistry : public QObject
{
    Q_OBJECT

  public:

    /**
     * Constructor for QgsProcessingRegistry.
     */
    QgsProcessingRegistry( QObject *parent SIP_TRANSFERTHIS = nullptr );

    ~QgsProcessingRegistry();

    //! Registry cannot be copied
    QgsProcessingRegistry( const QgsProcessingRegistry &other ) = delete;
    //! Registry cannot be copied
    QgsProcessingRegistry &operator=( const QgsProcessingRegistry &other ) = delete;

    /**
     * Get list of available providers.
     */
    QList<QgsProcessingProvider *> providers() const { return mProviders.values(); }

    /**
     * Add a processing provider to the registry. Ownership of the provider is transferred to the registry,
     * and the provider's parent will be set to the registry.
     * Returns false if the provider could not be added (eg if a provider with a duplicate ID already exists
     * in the registry).
     * Adding a provider to the registry automatically triggers the providers QgsProcessingProvider::load()
     * method to populate the provider with algorithms.
     * \see removeProvider()
     */
    bool addProvider( QgsProcessingProvider *provider SIP_TRANSFER );

    /**
     * Removes a provider implementation from the registry (the provider object is deleted).
     * Returns false if the provider could not be removed (eg provider does not exist in the registry).
     * \see addProvider()
     */
    bool removeProvider( QgsProcessingProvider *provider );

    /**
     * Removes a provider implementation from the registry (the provider object is deleted).
     * Returns false if the provider could not be removed (eg provider does not exist in the registry).
     * \see addProvider()
     */
    bool removeProvider( const QString &providerId );

    /**
     * Returns a matching provider by provider ID.
     */
    QgsProcessingProvider *providerById( const QString &id );

    /**
     * Returns a list of all available algorithms from registered providers.
     * \see algorithmById()
     */
    QList< QgsProcessingAlgorithm * > algorithms() const;

    /**
     * Finds an algorithm by its ID. If no matching algorithm is found, a nullptr
     * is returned.
     * \see algorithms()
     */
    QgsProcessingAlgorithm *algorithmById( const QString &id ) const;

    /**
     * Returns a pointer to the metadata for a parameter \a type.
     */
    QgsProcessingParameterAbstractMetadata *parameterMetadata( const QString &type ) const;

    /**
     * Adds \a metadata for a new parameter type to the registry. Ownership of the metadata is transferred.
     * Returns true if the parameter type was successfully registered, or false if the type could not
     * be registered (eg as a result of a duplicate type string).
     */
    bool addParameterType( QgsProcessingParameterAbstractMetadata *metadata SIP_TRANSFER );

    /**
     * Creates a new parameter from an encoded script \a code.
     */
    QgsProcessingParameter *createParameterFromScriptCode( const QString &code ) const;

  signals:

    //! Emitted when a provider has been added to the registry.
    void providerAdded( const QString &id );

    //! Emitted when a provider is removed from the registry
    void providerRemoved( const QString &id );

  private:

    //! Map of available providers by id. This class owns the pointers
    QMap<QString, QgsProcessingProvider *> mProviders;

    QMap<QString, QgsProcessingParameterAbstractMetadata *> mParameterMetadata;

    static bool parseScriptCodeParameterOptions( const QString &code, bool &isOptional, QString &name, QString &type, QString &definition );

    static QString createParameterDescription( const QString &name );

#ifdef SIP_RUN
    QgsProcessingRegistry( const QgsProcessingRegistry &other );
#endif
};

#endif // QGSPROCESSINGREGISTRY_H


