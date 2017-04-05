/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/core/processing/qgsprocessingparameters.h                        *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/




class QgsProcessingParameter
{
%Docstring
 Abstract base class for processing algorithm parameters.
.. versionadded:: 3.0
%End

%TypeHeaderCode
#include "qgsprocessingparameters.h"
%End
  public:

    enum Flag
    {
      FlagAdvanced,
      FlagHidden,
      FlagOptional,
    };
    typedef QFlags<QgsProcessingParameter::Flag> Flags;

    QgsProcessingParameter( const QString &name, const QString &description = QString(),
                            const QVariant &defaultValue = QVariant(), bool optional = false );
%Docstring
 Constructor for QgsProcessingParameter. The name must be unique for each parameter/algorithm
 combination, and should consist of alphanumeric characters only. Setting a parameter as optional
 will add the FlagOptional flag to the parameter.
%End

    virtual ~QgsProcessingParameter();

    virtual QString type() const = 0;
%Docstring
 Returns a unique string representing the parameter type.
%End

    QString name() const;
%Docstring
 Returns the name of the parameter. This is the internal identifier by which
 algorithms access this parameter.
%End

    QString description() const;
%Docstring
 Returns the description for the parameter. This is the user-visible string
 used to identify this parameter.
 \see setDescription()
%End

    void setDescription( const QString &description );
%Docstring
 Sets the description for the parameter. This is the user-visible string
 used to identify this parameter.
 \see description()
%End

    QVariant defaultValue() const;
%Docstring
 Returns the default value for the parameter.
 \see setDefaultValue()
%End

    void setDefaultValue( const QVariant &value );
%Docstring
 Sets the default value for the parameter. Caller takes responsibility
 to ensure that value is a valid input for the parameter subclass.
 \see defaultValue()
%End

    Flags flags() const;
%Docstring
 Returns any flags associated with the parameter.
 \see setFlags()
%End

    void setFlags( const Flags &flags );
%Docstring
 Sets the flags associated with the parameter.
 \see flags()
%End

    virtual bool acceptsValue( const QVariant &value, const QgsProcessingContext &context ) const = 0;
%Docstring
 Returns true if the specified value is acceptable for the parameter
 within a given context.
 \see parseValue()
%End

    virtual QVariant parseValue( const QVariant &value, const QgsProcessingContext &context ) const;
%Docstring
 Parses a raw input value and converts it to a value usable by the parameter.
 E.g. a boolean parameter may take string inputs like "true", "yes", and convert
 to a boolean true value, and a map layer parmeter may take inputs like layer IDs
 or filesnames and return a map layer pointer. Subclasses should override this to
 implement suitable conversions for their parameter types.
 The context argument specifies the context under which the algorithm
 will be executed.
 Before parsing values, callers should test that input values are acceptable
 by calling acceptsValue(). Behavior is undefined when parsing unacceptable
 values.
 \see acceptsValue()
%End

    virtual QString asScriptCode() const;
%Docstring
 Returns a text string representing the parameter's configuration for use
 in processing scripts.
%End

};

QFlags<QgsProcessingParameter::Flag> operator|(QgsProcessingParameter::Flag f1, QFlags<QgsProcessingParameter::Flag> f2);


class QgsProcessingParameterBoolean : QgsProcessingParameter
{
%Docstring
 A boolean parameter for processing algorithms.
.. versionadded:: 3.0
%End

%TypeHeaderCode
#include "qgsprocessingparameters.h"
%End
  public:

    QgsProcessingParameterBoolean( const QString &name, const QString &description = QString(), bool defaultValue = false,
                                   bool optional = false );
%Docstring
 Constructor for QgsProcessingParameterBoolean.
%End

    QString type() const;
    bool acceptsValue( const QVariant &value, const QgsProcessingContext &context ) const;
    QVariant parseValue( const QVariant &value, const QgsProcessingContext &context ) const;
    virtual QString asScriptCode() const;

    static QgsProcessingParameter *createFromScriptCode( const QString &name, const QString &description, bool isOptional, const QString &definition ) /Factory/;
%Docstring
 Returns a new QgsProcessingParameterBoolean configured using the definition from a script code.
 This is not usually called directly but instead called when required by QgsProcessingRegistry.createParameterFromScriptCode().
%End

};


class QgsProcessingParameterCrs : QgsProcessingParameter
{
%Docstring
 A coordinate reference system parameter for processing algorithms.
.. versionadded:: 3.0
%End

%TypeHeaderCode
#include "qgsprocessingparameters.h"
%End
  public:

    QgsProcessingParameterCrs( const QString &name, const QString &description = QString(), const QVariant &defaultValue = QVariant(),
                               bool optional = false );
%Docstring
 Constructor for QgsProcessingParameterCrs.
%End

    QString type() const;
    bool acceptsValue( const QVariant &value, const QgsProcessingContext &context ) const;
    QVariant parseValue( const QVariant &value, const QgsProcessingContext &context ) const;
    virtual QString asScriptCode() const;

    static QgsProcessingParameter *createFromScriptCode( const QString &name, const QString &description, bool isOptional, const QString &definition ) /Factory/;
%Docstring
 Returns a new QgsProcessingParameterBoolean configured using the definition from a script code.
 This is not usually called directly but instead called when required by QgsProcessingRegistry.createParameterFromScriptCode().
%End

};



/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/core/processing/qgsprocessingparameters.h                        *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/
