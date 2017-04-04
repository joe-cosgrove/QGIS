/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/core/processing/qgsprocessingregistry.h                          *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/





class QgsProcessingParameterAbstractMetadata
{
%Docstring
 Stores metadata about one processing parameter class.
.. note::

   In C++ you can use QgsProcessingParameterMetadata convenience class.
.. versionadded:: 3.0
%End

%TypeHeaderCode
#include "qgsprocessingregistry.h"
%End
  public:

    QgsProcessingParameterAbstractMetadata( const QString &type );
%Docstring
 Constructor for parameter abstract metadata. The type parameter must be unique.
%End

    virtual ~QgsProcessingParameterAbstractMetadata();

    QString type() const;
%Docstring
 Returns the unique type string which identifies the parameter type.
%End

    virtual QgsProcessingParameter *createParameterFromScriptCode( const QString &name, const QString &description, bool isOptional, const QString &definition );
%Docstring
 Creates a new parameter of this type from an encoded script code line. The name, description and optional flag will have already
 been parsed when this function is called, so only any extra handling of the remaining definition string needs to be done by the class.
%End

};

typedef QgsProcessingParameter *( *QgsProcessingParameterFromScriptCodeFunc )( const QString &, const QString &, bool, const QString & );


class QgsProcessingRegistry : QObject
{
%Docstring
 Registry for various processing components, including providers, algorithms
 and various parameters and outputs.

 QgsProcessingRegistry is not usually directly created, but rather accessed through
 QgsApplication.processingRegistry().
.. versionadded:: 3.0
%End

%TypeHeaderCode
#include "qgsprocessingregistry.h"
%End

  public:

    QgsProcessingRegistry( QObject *parent /TransferThis/ = 0 );
%Docstring
 Constructor for QgsProcessingRegistry.
%End

    ~QgsProcessingRegistry();

//     QgsProcessingRegistry &operator=( const QgsProcessingRegistry &other ) = delete;

    QList<QgsProcessingProvider *> providers() const;
%Docstring
 Get list of available providers.
%End

    bool addProvider( QgsProcessingProvider *provider /Transfer/ );
%Docstring
 Add a processing provider to the registry. Ownership of the provider is transferred to the registry,
 and the provider's parent will be set to the registry.
 Returns false if the provider could not be added (eg if a provider with a duplicate ID already exists
 in the registry).
 Adding a provider to the registry automatically triggers the providers QgsProcessingProvider.load()
 method to populate the provider with algorithms.
 \see removeProvider()
%End

    bool removeProvider( QgsProcessingProvider *provider );
%Docstring
 Removes a provider implementation from the registry (the provider object is deleted).
 Returns false if the provider could not be removed (eg provider does not exist in the registry).
 \see addProvider()
%End

    bool removeProvider( const QString &providerId );
%Docstring
 Removes a provider implementation from the registry (the provider object is deleted).
 Returns false if the provider could not be removed (eg provider does not exist in the registry).
 \see addProvider()
%End

    QgsProcessingProvider *providerById( const QString &id );
%Docstring
 Returns a matching provider by provider ID.
%End

    QList< QgsProcessingAlgorithm * > algorithms() const;
%Docstring
 Returns a list of all available algorithms from registered providers.
 \see algorithmById()
%End

    QgsProcessingAlgorithm *algorithmById( const QString &id ) const;
%Docstring
 Finds an algorithm by its ID. If no matching algorithm is found, a nullptr
 is returned.
 \see algorithms()
%End

    QgsProcessingParameterAbstractMetadata *parameterMetadata( const QString &type ) const;
%Docstring
 Returns a pointer to the metadata for a parameter type.
%End

    bool addParameterType( QgsProcessingParameterAbstractMetadata *metadata /Transfer/ );
%Docstring
 Adds metadata for a new parameter type to the registry. Ownership of the metadata is transferred.
 Returns true if the parameter type was successfully registered, or false if the type could not
 be registered (eg as a result of a duplicate type string).
%End

    QgsProcessingParameter *createParameterFromScriptCode( const QString &code ) const;
%Docstring
 Creates a new parameter from an encoded script code.
%End

  signals:

    void providerAdded( const QString &id );
%Docstring
Emitted when a provider has been added to the registry.
%End

    void providerRemoved( const QString &id );
%Docstring
Emitted when a provider is removed from the registry
%End

  private:
    QgsProcessingRegistry( const QgsProcessingRegistry &other );
};



/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/core/processing/qgsprocessingregistry.h                          *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/
