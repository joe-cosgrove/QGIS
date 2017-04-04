/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/core/processing/qgsprocessingparameters.h                        *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/




class QgsProcessingParameter
{

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

    QgsProcessingParameter( const QString& name, const QString& description = QString(), const QVariant& defaultValue = QVariant(),
                            bool optional = false );

    virtual QString type() const = 0;

    QString name() const;
%Docstring
 Returns the name of the parameter. This is the internal identifier by which
 algorithms access this parameter.
 \see setName()
%End

    void setName( const QString& name );
%Docstring
 Sets the name of the parameter. This is the internal identifier by which
 algorithms access this parameter.
 \see name()
%End

    QString description() const;
%Docstring
 Returns the description for the parameter. This is the user-visible string
 used to identify this parameter.
 \see setDescription()
%End

    void setDescription( const QString& description );
%Docstring
 Sets the description for the parameter. This is the user-visible string
 used to identify this parameter.
 \see description()
%End

    virtual QVariant defaultValue() const;
%Docstring
 Returns the default value for the parameter.
 \see setDefaultValue()
%End

    virtual bool setDefaultValue( const QVariant& value );
%Docstring
 Sets the default value for the parameter. Returns true if default value was successfully set,
 or false if value is not acceptable for the parameter.
 \see defaultValue()
%End

    Flags flags() const;
%Docstring
 Returns any flags associated with the parameter.
 \see setFlags()
%End

    void setFlags( const Flags& flags );
%Docstring
 Sets the flags associated with the parameter.
 \see flags()
%End

    virtual bool acceptsValue( const QVariant& value ) const = 0;
%Docstring
 Returns true if the specified value is acceptable for the parameter.
%End

    virtual QVariant parseValue( const QVariant& value ) const;

    virtual QString valueAsCommandLineParameter( const QVariant& value ) const;
%Docstring
 Returns the value of this parameter as it should have been entered in the console if calling
 an algorithm manually.
%End

    virtual QString asScriptCode() const;

  protected:

    QString mName;
%Docstring
Parameter name
%End

    QString mDescription;
%Docstring
Parameter description
%End

    QVariant mDefault;
%Docstring
Default value for parameter
%End

    Flags mFlags;
%Docstring
Parameter flags
%End


};

QFlags<QgsProcessingParameter::Flag> operator|(QgsProcessingParameter::Flag f1, QFlags<QgsProcessingParameter::Flag> f2);


class QgsProcessingParameterBoolean : QgsProcessingParameter
{

%TypeHeaderCode
#include "qgsprocessingparameters.h"
%End
  public:

    QgsProcessingParameterBoolean( const QString& name, const QString& description = QString(), const QVariant& defaultValue = QVariant(),
                                   bool optional = false );

    QString type() const;

    bool acceptsValue( const QVariant& value ) const;

    QVariant parseValue( const QVariant& value ) const;

    static QgsProcessingParameter* createFromScriptCode( const QString& name, const QString& description, bool isOptional, const QString& definition );

};




/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/core/processing/qgsprocessingparameters.h                        *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/
