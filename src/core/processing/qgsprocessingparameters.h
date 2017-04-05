/***************************************************************************
                         qgsprocessingparameters.h
                         -----------------------
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

#ifndef QGSPROCESSINGPARAMETERS_H
#define QGSPROCESSINGPARAMETERS_H

#include "qgis_core.h"
#include "qgis.h"
#include "qgsprocessingcontext.h"
#include <QString>
#include <QVariant>

/**
 * \class QgsProcessingParameter
 * \ingroup core
 * Abstract base class for processing algorithm parameters.
  * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingParameter
{
  public:

    //! Flags dictating behavior of parameter
    enum Flag
    {
      FlagAdvanced = 1 << 1, //!< Parameter is an advanced parameter which should be hidden from users by default
      FlagHidden = 1 << 2, //!< Parameter is hidden and should not be shown to users
      FlagOptional = 1 << 3, //!< Parameter is optional
    };
    Q_DECLARE_FLAGS( Flags, Flag )

    /**
     * Constructor for QgsProcessingParameter. The \a name must be unique for each parameter/algorithm
     * combination, and should consist of alphanumeric characters only. Setting a parameter as \a optional
     * will add the FlagOptional flag to the parameter.
     */
    QgsProcessingParameter( const QString &name, const QString &description = QString(),
                            const QVariant &defaultValue = QVariant(), bool optional = false );

    virtual ~QgsProcessingParameter() = default;

    /**
     * Returns a unique string representing the parameter type.
     */
    virtual QString type() const = 0;

    /**
     * Returns the name of the parameter. This is the internal identifier by which
     * algorithms access this parameter.
     */
    QString name() const { return mName; }

    /**
     * Returns the description for the parameter. This is the user-visible string
     * used to identify this parameter.
     * \see setDescription()
     */
    QString description() const { return mDescription; }

    /**
     * Sets the \a description for the parameter. This is the user-visible string
     * used to identify this parameter.
     * \see description()
     */
    void setDescription( const QString &description );

    /**
     * Returns the default value for the parameter.
     * \see setDefaultValue()
     */
    QVariant defaultValue() const { return mDefault; }

    /**
     * Sets the default \a value for the parameter. Caller takes responsibility
     * to ensure that \a value is a valid input for the parameter subclass.
     * \see defaultValue()
     */
    void setDefaultValue( const QVariant &value );

    /**
     * Returns any flags associated with the parameter.
     * \see setFlags()
     */
    Flags flags() const { return mFlags; }

    /**
     * Sets the \a flags associated with the parameter.
     * \see flags()
     */
    void setFlags( const Flags &flags );

    /**
     * Returns true if the specified \a value is acceptable for the parameter
     * within a given \a context.
     * \see parseValue()
     */
    virtual bool acceptsValue( const QVariant &value, const QgsProcessingContext &context ) const = 0;

    /**
     * Parses a raw input \a value and converts it to a value usable by the parameter.
     * E.g. a boolean parameter may take string inputs like "true", "yes", and convert
     * to a boolean true value, and a map layer parmeter may take inputs like layer IDs
     * or filesnames and return a map layer pointer. Subclasses should override this to
     * implement suitable conversions for their parameter types.
     * The \a context argument specifies the context under which the algorithm
     * will be executed.
     * Before parsing values, callers should test that input values are acceptable
     * by calling acceptsValue(). Behavior is undefined when parsing unacceptable
     * values.
     * \see acceptsValue()
     */
    virtual QVariant parseValue( const QVariant &value, const QgsProcessingContext &context ) const { Q_UNUSED( context ); return value; }

    /**
     * Returns a text string representing the parameter's configuration for use
     * in processing scripts.
     */
    virtual QString asScriptCode() const;

  private:

    //! Parameter name
    QString mName;

    //! Parameter description
    QString mDescription;

    //! Default value for parameter
    QVariant mDefault;

    //! Parameter flags
    Flags mFlags = 0;

};

Q_DECLARE_OPERATORS_FOR_FLAGS( QgsProcessingParameter::Flags )


/**
 * \class QgsProcessingParameterBoolean
 * \ingroup core
 * A boolean parameter for processing algorithms.
  * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingParameterBoolean : public QgsProcessingParameter
{
  public:

    /**
     * Constructor for QgsProcessingParameterBoolean.
     */
    QgsProcessingParameterBoolean( const QString &name, const QString &description = QString(), bool defaultValue = false,
                                   bool optional = false );

    QString type() const override { return QStringLiteral( "boolean" ); }
    bool acceptsValue( const QVariant &value, const QgsProcessingContext &context ) const override;
    QVariant parseValue( const QVariant &value, const QgsProcessingContext &context ) const override;
    virtual QString asScriptCode() const override;

    /**
     * Returns a new QgsProcessingParameterBoolean configured using the \a definition from a script code.
     * This is not usually called directly but instead called when required by QgsProcessingRegistry::createParameterFromScriptCode().
     */
    static QgsProcessingParameter *createFromScriptCode( const QString &name, const QString &description, bool isOptional, const QString &definition ) SIP_FACTORY;

  private:

    static bool convertToBool( const QVariant &value );

};


/**
 * \class QgsProcessingParameterCrs
 * \ingroup core
 * A coordinate reference system parameter for processing algorithms.
  * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingParameterCrs : public QgsProcessingParameter
{
  public:

    /**
     * Constructor for QgsProcessingParameterCrs.
     */
    QgsProcessingParameterCrs( const QString &name, const QString &description = QString(), const QVariant &defaultValue = QVariant(),
                               bool optional = false );

    QString type() const override { return QStringLiteral( "crs" ); }
    bool acceptsValue( const QVariant &value, const QgsProcessingContext &context ) const override;
    QVariant parseValue( const QVariant &value, const QgsProcessingContext &context ) const override;
    virtual QString asScriptCode() const override;

    /**
     * Returns a new QgsProcessingParameterBoolean configured using the \a definition from a script code.
     * This is not usually called directly but instead called when required by QgsProcessingRegistry::createParameterFromScriptCode().
     */
    static QgsProcessingParameter *createFromScriptCode( const QString &name, const QString &description, bool isOptional, const QString &definition ) SIP_FACTORY;

  private:

    static QVariant convertToCrs( const QVariant &value, const QgsProcessingContext &context );

};

#endif // QGSPROCESSINGPARAMETERS_H


