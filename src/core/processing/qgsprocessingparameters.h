/***************************************************************************
                         qgsprocessingparameters.h
                         -------------------------
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

#ifndef QGSPROCESSINGPARAMETERS_H
#define QGSPROCESSINGPARAMETERS_H

#include "qgis_core.h"
#include "qgis.h"
#include "qgsproperty.h"
#include "qgscoordinatereferencesystem.h"
#include <QMap>
#include <limits>

class QgsProcessingContext;
class QgsRasterLayer;
class QgsVectorLayer;

//
// Parameter definitions
//

/**
 * \class QgsProcessingParameterDefinition
 * \ingroup core
 *
 * Base class for the definition of processing parameters.
 *
 * Parameter definitions encapsulate properties regarding the behavior of parameters,
 * their acceptable ranges, defaults, etc.
 *
 * \since QGIS 3.0
 */

class CORE_EXPORT QgsProcessingParameterDefinition
{

#ifdef SIP_RUN
    SIP_CONVERT_TO_SUBCLASS_CODE
    if ( sipCpp->type() == "boolean" )
      sipType = sipType_QgsProcessingParameterBoolean;
    else if ( sipCpp->type() == "crs" )
      sipType = sipType_QgsProcessingParameterCrs;
    else if ( sipCpp->type() == "layer" )
      sipType = sipType_QgsProcessingParameterMapLayer;
    else if ( sipCpp->type() == "extent" )
      sipType = sipType_QgsProcessingParameterExtent;
    else if ( sipCpp->type() == "point" )
      sipType = sipType_QgsProcessingParameterPoint;
    else if ( sipCpp->type() == "file" )
      sipType = sipType_QgsProcessingParameterFile;
    else if ( sipCpp->type() == "matrix" )
      sipType = sipType_QgsProcessingParameterMatrix;
    else if ( sipCpp->type() == "multilayer" )
      sipType = sipType_QgsProcessingParameterMultipleLayers;
    else if ( sipCpp->type() == "number" )
      sipType = sipType_QgsProcessingParameterNumber;
    else if ( sipCpp->type() == "range" )
      sipType = sipType_QgsProcessingParameterRange;
    else if ( sipCpp->type() == "raster" )
      sipType = sipType_QgsProcessingParameterRasterLayer;
    else if ( sipCpp->type() == "enum" )
      sipType = sipType_QgsProcessingParameterEnum;
    else if ( sipCpp->type() == "string" )
      sipType = sipType_QgsProcessingParameterString;
    else if ( sipCpp->type() == "expression" )
      sipType = sipType_QgsProcessingParameterExpression;
    else if ( sipCpp->type() == "table" )
      sipType = sipType_QgsProcessingParameterTable;
    else if ( sipCpp->type() == "field" )
      sipType = sipType_QgsProcessingParameterTableField;
    else if ( sipCpp->type() == "vector" )
      sipType = sipType_QgsProcessingParameterVectorLayer;
    else if ( sipCpp->type() == "outputVector" )
      sipType = sipType_QgsProcessingOutputVectorLayer;

    // last!
    else if ( dynamic_cast < QgsProcessingOutputDefinition * >( sipCpp ) )
      sipType = sipType_QgsProcessingOutputDefinition;
    SIP_END
#endif

  public:

    //! Parameter flags
    enum Flag
    {
      FlagAdvanced = 1 << 1, //!< Parameter is an advanced parameter which should be hidden from users by default
      FlagHidden = 1 << 2, //!< Parameter is hidden and should not be shown to users
      FlagOptional = 1 << 3, //!< Parameter is optional
    };
    Q_DECLARE_FLAGS( Flags, Flag )

    //! Layer types enum
    enum LayerType
    {
      TypeAny = -2, //!< Any layer
      TypeVectorAny = -1, //!< Any vector layer with geometry
      TypeVectorPoint = 0, //!< Vector point layers
      TypeVectorLine = 1, //!< Vector line layers
      TypeVectorPolygon = 2, //!< Vector polygon layers
      TypeRaster = 3, //!< Raster layers
      TypeFile = 4, //!< Files
      TypeTable = 5, //!< Tables (i.e. vector layers with or without geometry)
    };

    /**
     * Constructor for QgsProcessingParameterDefinition.
     */
    QgsProcessingParameterDefinition( const QString &name, const QString &description = QString(), const QVariant &defaultValue = QVariant(),
                                      bool optional = false );

    virtual ~QgsProcessingParameterDefinition() = default;

    /**
     * Unique parameter type name.
     */
    virtual QString type() const = 0;

    /**
     * Returns the name of the parameter. This is the internal identifier by which
     * algorithms access this parameter.
     * @see setName()
     */
    QString name() const { return mName; }

    /**
     * Sets the \a name of the parameter. This is the internal identifier by which
     * algorithms access this parameter.
     * @see name()
     */
    void setName( const QString &name ) { mName = name; }

    /**
     * Returns the description for the parameter. This is the user-visible string
     * used to identify this parameter.
     * @see setDescription()
     */
    QString description() const { return mDescription; }

    /**
     * Sets the \a description for the parameter. This is the user-visible string
     * used to identify this parameter.
     * @see description()
     */
    void setDescription( const QString &description ) { mDescription = description; }

    /**
     * Returns the default value for the parameter.
     * @see setDefaultValue()
     */
    QVariant defaultValue() const { return mDefault; }

    /**
     * Sets the default \a value for the parameter. Caller takes responsibility
     * to ensure that \a value is a valid input for the parameter subclass.
     * @see defaultValue()
     */
    void setDefaultValue( const QVariant &value ) { mDefault = value; }

    /**
     * Returns any flags associated with the parameter.
     * @see setFlags()
     */
    Flags flags() const { return mFlags; }

    /**
     * Sets the \a flags associated with the parameter.
     * @see flags()
     */
    void setFlags( const Flags &flags ) { mFlags = flags; }

    /**
     * Checks whether the specified \a input value is acceptable for the
     * parameter. Returns true if the value can be accepted.
     */
    virtual bool checkValueIsAcceptable( const QVariant &input ) const;

  protected:

    //! Parameter name
    QString mName;

    //! Parameter description
    QString mDescription;

    //! Default value for parameter
    QVariant mDefault;

    //! Parameter flags
    Flags mFlags;

};

Q_DECLARE_OPERATORS_FOR_FLAGS( QgsProcessingParameterDefinition::Flags )

//! List of processing parameters
typedef QList< const QgsProcessingParameterDefinition * > QgsProcessingParameterDefinitions;

/**
 * \class QgsProcessingParameters
 * \ingroup core
 *
 * A collection of utilities for working with parameters when running a processing algorithm.
 *
 * Parameters are stored in a QVariantMap and referenced by a unique string key.
 * The QVariants in parameters are not usually accessed
 * directly, and instead the high level API provided through QgsProcessingParameters
 * parameterAsString(), parameterAsDouble() are used instead.
 *
 * Parameters are evaluated using a provided QgsProcessingContext, allowing
 * the evaluation to understand available map layers and expression contexts
 * (for expression based parameters).
 *
 * \since QGIS 3.0
*/

class CORE_EXPORT QgsProcessingParameters
{

  public:

    /**
     * Returns true if the parameter with matching \a name is a dynamic parameter, and must
     * be evaluated once for every input feature processed.
     */
    static bool isDynamic( const QVariantMap &parameters, const QString &name );

    /**
     * Evaluates the parameter with matching \a name to a static string value.
     */
    static QString parameterAsString( const QgsProcessingParameterDefinition *definition, const QVariantMap &parameters, const QString &name, const QgsProcessingContext &context );

    /**
     * Evaluates the parameter with matching \a name to an expression.
     */
    static QString parameterAsExpression( const QgsProcessingParameterDefinition *definition, const QVariantMap &parameters, const QString &name, const QgsProcessingContext &context );

    /**
     * Evaluates the parameter with matching \a name to a static double value.
     */
    static double parameterAsDouble( const QgsProcessingParameterDefinition *definition, const QVariantMap &parameters, const QString &name, const QgsProcessingContext &context );

    /**
     * Evaluates the parameter with matching \a name to a static integer value.
     */
    static int parameterAsInt( const QgsProcessingParameterDefinition *definition, const QVariantMap &parameters, const QString &name, const QgsProcessingContext &context );

    /**
     * Evaluates the parameter with matching \a name to a enum value.
     */
    static int parameterAsEnum( const QgsProcessingParameterDefinition *definition, const QVariantMap &parameters, const QString &name, const QgsProcessingContext &context );

    /**
     * Evaluates the parameter with matching \a name to list of enum values.
     */
    static QList<int> parameterAsEnums( const QgsProcessingParameterDefinition *definition, const QVariantMap &parameters, const QString &name, const QgsProcessingContext &context );

    /**
     * Evaluates the parameter with matching \a name to a static boolean value.
     */
    static bool parameterAsBool( const QgsProcessingParameterDefinition *definition, const QVariantMap &parameters, const QString &name, const QgsProcessingContext &context );

    /**
     * Evaluates the parameter with matching \a name to a map layer.
     *
     * Layers will either be taken from \a context's active project, or loaded from external
     * sources and stored temporarily in the \a context. In either case, callers do not
     * need to handle deletion of the returned layer.
     */
    static QgsMapLayer *parameterAsLayer( const QgsProcessingParameterDefinition *definition, const QVariantMap &parameters, const QString &name, QgsProcessingContext &context );

    /**
     * Evaluates the parameter with matching \a name to a raster layer.
     *
     * Layers will either be taken from \a context's active project, or loaded from external
     * sources and stored temporarily in the \a context. In either case, callers do not
     * need to handle deletion of the returned layer.
     */
    static QgsRasterLayer *parameterAsRasterLayer( const QgsProcessingParameterDefinition *definition, const QVariantMap &parameters, const QString &name, QgsProcessingContext &context );

    /**
     * Evaluates the parameter with matching \a name to a vector layer.
     *
     * Layers will either be taken from \a context's active project, or loaded from external
     * sources and stored temporarily in the \a context. In either case, callers do not
     * need to handle deletion of the returned layer.
     */
    static QgsVectorLayer *parameterAsVectorLayer( const QgsProcessingParameterDefinition *definition, const QVariantMap &parameters, const QString &name, QgsProcessingContext &context );

    /**
     * Evaluates the parameter with matching \a name to a coordinate reference system.
     */
    static QgsCoordinateReferenceSystem parameterAsCrs( const QgsProcessingParameterDefinition *definition, const QVariantMap &parameters, const QString &name, QgsProcessingContext &context );

    /**
     * Evaluates the parameter with matching \a name to a rectangular extent.
     */
    static QgsRectangle parameterAsExtent( const QgsProcessingParameterDefinition *definition, const QVariantMap &parameters, const QString &name, QgsProcessingContext &context );

    /**
     * Evaluates the parameter with matching \a name to a point.
     */
    static QgsPoint parameterAsPoint( const QgsProcessingParameterDefinition *definition, const QVariantMap &parameters, const QString &name, QgsProcessingContext &context );

    /**
     * Evaluates the parameter with matching \a name to a file/folder name.
     */
    static QString parameterAsFile( const QgsProcessingParameterDefinition *definition, const QVariantMap &parameters, const QString &name, QgsProcessingContext &context );

    /**
     * Evaluates the parameter with matching \a name to a matrix/table of values.
     * Tables are collapsed to a 1 dimensional list.
     */
    static QVariantList parameterAsMatrix( const QgsProcessingParameterDefinition *definition, const QVariantMap &parameters, const QString &name, QgsProcessingContext &context );

    /**
     * Evaluates the parameter with matching \a name to a list of map layers.
     */
    static QList< QgsMapLayer *> parameterAsLayerList( const QgsProcessingParameterDefinition *definition, const QVariantMap &parameters, const QString &name, QgsProcessingContext &context );

    /**
     * Evaluates the parameter with matching \a name to a range of values.
     */
    static QList<double> parameterAsRange( const QgsProcessingParameterDefinition *definition, const QVariantMap &parameters, const QString &name, QgsProcessingContext &context );

    /**
     * Evaluates the parameter with matching \a name to a list of fields.
     */
    static QStringList parameterAsFields( const QgsProcessingParameterDefinition *definition, const QVariantMap &parameters, const QString &name, QgsProcessingContext &context );

};



/**
 * \class QgsProcessingParameterBoolean
 * \ingroup core
 * A boolean parameter for processing algorithms.
  * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingParameterBoolean : public QgsProcessingParameterDefinition
{
  public:

    /**
     * Constructor for QgsProcessingParameterBoolean.
     */
    QgsProcessingParameterBoolean( const QString &name, const QString &description = QString(), const QVariant &defaultValue = QVariant(),
                                   bool optional = false );

    QString type() const override { return QStringLiteral( "boolean" ); }
};

/**
 * \class QgsProcessingParameterCrs
 * \ingroup core
 * A coordinate reference system parameter for processing algorithms.
  * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingParameterCrs : public QgsProcessingParameterDefinition
{
  public:

    /**
     * Constructor for QgsProcessingParameterCrs.
     */
    QgsProcessingParameterCrs( const QString &name, const QString &description = QString(), const QVariant &defaultValue = QVariant(),
                               bool optional = false );

    QString type() const override { return QStringLiteral( "crs" ); }
};

/**
 * \class QgsProcessingParameterMapLayer
 * \ingroup core
 * A map layer parameter for processing algorithms.
  * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingParameterMapLayer : public QgsProcessingParameterDefinition
{
  public:

    /**
     * Constructor for QgsProcessingParameterMapLayer.
     */
    QgsProcessingParameterMapLayer( const QString &name, const QString &description = QString(), const QVariant &defaultValue = QVariant(),
                                    bool optional = false );

    QString type() const override { return QStringLiteral( "layer" ); }
};

/**
 * \class QgsProcessingParameterExtent
 * \ingroup core
 * A rectangular map extent parameter for processing algorithms.
  * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingParameterExtent : public QgsProcessingParameterDefinition
{
  public:

    /**
     * Constructor for QgsProcessingParameterExtent.
     */
    QgsProcessingParameterExtent( const QString &name, const QString &description = QString(), const QVariant &defaultValue = QVariant(),
                                  bool optional = false );

    QString type() const override { return QStringLiteral( "extent" ); }

};


/**
 * \class QgsProcessingParameterPoint
 * \ingroup core
 * A point parameter for processing algorithms.
  * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingParameterPoint : public QgsProcessingParameterDefinition
{
  public:

    /**
     * Constructor for QgsProcessingParameterPoint.
     */
    QgsProcessingParameterPoint( const QString &name, const QString &description = QString(), const QVariant &defaultValue = QVariant(),
                                 bool optional = false );

    QString type() const override { return QStringLiteral( "point" ); }
    bool checkValueIsAcceptable( const QVariant &input ) const override;

};

/**
 * \class QgsProcessingParameterFile
 * \ingroup core
 * An input file or folder parameter for processing algorithms.
  * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingParameterFile : public QgsProcessingParameterDefinition
{
  public:

    //! Parameter behavior
    enum Behavior
    {
      File = 0, //!< Parameter is a single file
      Folder, //!< Parameter is a folder
    };

    /**
     * Constructor for QgsProcessingParameterFile.
     */
    QgsProcessingParameterFile( const QString &name, const QString &description = QString(), Behavior behavior = File, const QString &extension = QString(), const QVariant &defaultValue = QVariant(),
                                bool optional = false );

    QString type() const override { return QStringLiteral( "file" ); }
    bool checkValueIsAcceptable( const QVariant &input ) const override;

    /**
     * Returns the parameter behavior (e.g. File or Folder).
     * \see setBehavior()
     */
    Behavior behavior() const { return mBehavior; }

    /**
     * Sets the parameter \a behavior (e.g. File or Folder).
     * \see behavior()
     */
    void setBehavior( Behavior behavior ) { mBehavior = behavior; }

    /**
     * Returns any specified file extension for the parameter.
     * \see setExtension()
     */
    QString extension() const { return mExtension; }

    /**
     * Sets a file \a extension for the parameter.
     * \see extension()
     */
    void setExtension( const QString &extension ) { mExtension = extension; }

  private:

    Behavior mBehavior = File;
    QString mExtension;
};

/**
 * \class QgsProcessingParameterMatrix
 * \ingroup core
 * An table (matrix) parameter for processing algorithms.
  * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingParameterMatrix : public QgsProcessingParameterDefinition
{
  public:

    /**
     * Constructor for QgsProcessingParameterMatrix.
     */
    QgsProcessingParameterMatrix( const QString &name, const QString &description = QString(), int numberRows = 3,
                                  bool hasFixedNumberRows = false, const QStringList &headers = QStringList(),
                                  const QVariant &defaultValue = QVariant(),
                                  bool optional = false );

    QString type() const override { return QStringLiteral( "matrix" ); }
    bool checkValueIsAcceptable( const QVariant &input ) const override;

    /**
     * Returns a list of column headers (if set).
     * \see setHeaders()
     */
    QStringList headers() const;

    /**
     * Sets the list of column \a headers.
     * \see headers()
     */
    void setHeaders( const QStringList &headers );

    /**
     * Returns the fixed number of rows in the table. This parameter only has an
     * effect if hasFixedNumberRows() is true.
     * \see setNumberRows()
     * \see setFixedNumberRows()
     */
    int numberRows() const;

    /**
     * Sets the fixed number of \a rows in the table. This parameter only has an
     * effect if hasFixedNumberRows() is true.
     * \see numberRows()
     * \see setFixedNumberRows()
     */
    void setNumberRows( int rows );

    /**
     * Returns whether the table has a fixed number of rows.
     * \see numberRows()
     * \see setHasFixedNumberRows()
     */
    bool hasFixedNumberRows() const;

    /**
     * Sets whether the table has a fixed number of rows.
     * \see setNumberRows()
     * \see hasFixedNumberRows()
     */
    void setHasFixedNumberRows( bool hasFixedNumberRows );

  private:

    QStringList mHeaders;
    int mNumberRows = 3;
    bool mFixedNumberRows = false;

};

/**
 * \class QgsProcessingParameterMultipleLayers
 * \ingroup core
 * A parameter for processing algorithms which accepts multiple map layers.
  * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingParameterMultipleLayers : public QgsProcessingParameterDefinition
{
  public:

    /**
     * Constructor for QgsProcessingParameterMultipleLayers.
     */
    QgsProcessingParameterMultipleLayers( const QString &name, const QString &description = QString(), QgsProcessingParameterDefinition::LayerType layerType = QgsProcessingParameterDefinition::TypeVectorAny,
                                          const QVariant &defaultValue = QVariant(),
                                          bool optional = false );

    QString type() const override { return QStringLiteral( "multilayer" ); }
    bool checkValueIsAcceptable( const QVariant &input ) const override;

    /**
     * Returns the layer type for layers acceptable by the parameter.
     * \see setLayerType()
     */
    QgsProcessingParameterDefinition::LayerType layerType() const;

    /**
     * Sets the layer \a type for layers acceptable by the parameter.
     * \see layerType()
     */
    void setLayerType( QgsProcessingParameterDefinition::LayerType type );

    /**
     * Returns the minimum number of layers required for the parameter. If the return value is < 1
     * then the parameter accepts any number of layers.
     * \see setMinimumNumberInputs()
     */
    int minimumNumberInputs() const;

    /**
     * Sets the \a minimum number of layers required for the parameter. The minimum must be >= 1
     * if the parameter is not optional.
     * \see minimumNumberInputs()
     */
    void setMinimumNumberInputs( int minimum );

  private:

    LayerType mLayerType = TypeVectorAny;
    int mMinimumNumberInputs = 0;

};

/**
 * \class QgsProcessingParameterNumber
 * \ingroup core
 * A numeric parameter for processing algorithms.
  * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingParameterNumber : public QgsProcessingParameterDefinition
{
  public:

    //! Numeric data type
    enum Type
    {
      Integer, //!< Integer values
      Double, //!< Double/float values
    };

    /**
     * Constructor for QgsProcessingParameterNumber.
     */
    explicit QgsProcessingParameterNumber( const QString &name, const QString &description = QString(),
                                           Type type = Integer,
                                           const QVariant &defaultValue = QVariant(),
                                           bool optional = false,
                                           double minValue = -DBL_MAX,
                                           double maxValue = DBL_MAX
                                         );

    QString type() const override { return QStringLiteral( "number" ); }
    bool checkValueIsAcceptable( const QVariant &input ) const override;

    /**
     * Returns the minimum value acceptable by the parameter.
     * \see setMinimum()
     */
    double minimum() const;

    /**
     * Sets the \a minimum value acceptable by the parameter.
     * \see minimum()
     */
    void setMinimum( double minimum );

    /**
     * Returns the maximum value acceptable by the parameter.
     * \see setMaximum()
     */
    double maximum() const;

    /**
     * Sets the \a maximum value acceptable by the parameter.
     * \see maximum()
     */
    void setMaximum( double maximum );

    /**
     * Returns the acceptable data type for the parameter.
     * \see setDataType()
     */
    Type dataType() const;

    /**
     * Sets the acceptable data \a type for the parameter.
     * \see dataType()
     */
    void setDataType( const Type &type );

  private:

    double mMin = -DBL_MAX;
    double mMax = DBL_MAX;
    Type mDataType = Integer;
};

/**
 * \class QgsProcessingParameterRange
 * \ingroup core
 * A numeric range parameter for processing algorithms.
  * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingParameterRange : public QgsProcessingParameterDefinition
{
  public:

    /**
     * Constructor for QgsProcessingParameterRange.
     */
    QgsProcessingParameterRange( const QString &name, const QString &description = QString(),
                                 QgsProcessingParameterNumber::Type type = QgsProcessingParameterNumber::Integer,
                                 const QVariant &defaultValue = QVariant(),
                                 bool optional = false );

    QString type() const override { return QStringLiteral( "range" ); }
    bool checkValueIsAcceptable( const QVariant &input ) const override;

    /**
     * Returns the acceptable data type for the range.
     * \see setDataType()
     */
    QgsProcessingParameterNumber::Type dataType() const;

    /**
     * Sets the acceptable data \a type for the range.
     * \see dataType()
     */
    void setDataType( const QgsProcessingParameterNumber::Type &dataType );

  private:

    QgsProcessingParameterNumber::Type mDataType = QgsProcessingParameterNumber::Integer;
};

/**
 * \class QgsProcessingParameterRasterLayer
 * \ingroup core
 * A raster layer parameter for processing algorithms.
  * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingParameterRasterLayer : public QgsProcessingParameterDefinition
{
  public:

    /**
     * Constructor for QgsProcessingParameterRasterLayer.
     */
    QgsProcessingParameterRasterLayer( const QString &name, const QString &description = QString(), const QVariant &defaultValue = QVariant(),
                                       bool optional = false );

    QString type() const override { return QStringLiteral( "raster" ); }

};

/**
 * \class QgsProcessingParameterEnum
 * \ingroup core
 * An enum based parameter for processing algorithms, allowing for selection from predefined values.
  * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingParameterEnum : public QgsProcessingParameterDefinition
{
  public:

    /**
     * Constructor for QgsProcessingParameterEnum.
     */
    QgsProcessingParameterEnum( const QString &name, const QString &description = QString(), const QStringList &options = QStringList(),
                                bool allowMultiple = false,
                                const QVariant &defaultValue = QVariant(),
                                bool optional = false );

    QString type() const override { return QStringLiteral( "enum" ); }
    bool checkValueIsAcceptable( const QVariant &input ) const override;

    /**
     * Returns the list of acceptable options for the parameter.
     * \see setOptions()
     */
    QStringList options() const;

    /**
     * Sets the list of acceptable \a options for the parameter.
     * \see options()
     */
    void setOptions( const QStringList &options );

    /**
     * Returns true if the parameter allows multiple selected values.
     * \see setAllowMultiple()
     */
    bool allowMultiple() const;

    /**
     * Sets whether the parameter allows multiple selected values.
     * \see allowMultiple()
     */
    void setAllowMultiple( bool allowMultiple );

  private:

    QStringList mOptions;
    bool mAllowMultiple = false;

};

/**
 * \class QgsProcessingParameterString
 * \ingroup core
 * A string parameter for processing algorithms.
  * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingParameterString : public QgsProcessingParameterDefinition
{
  public:

    /**
     * Constructor for QgsProcessingParameterString.
     */
    QgsProcessingParameterString( const QString &name, const QString &description = QString(), const QVariant &defaultValue = QVariant(),
                                  bool multiLine = false,
                                  bool optional = false );

    QString type() const override { return QStringLiteral( "string" ); }

    /**
     * Returns true if the parameter allows multiline strings.
     * \see setMultiLine()
     */
    bool multiLine() const;

    /**
     * Sets whether the parameter allows multiline strings.
     * \see multiLine()
     */
    void setMultiLine( bool multiLine );

  private:

    bool mMultiLine = false;

};

/**
 * \class QgsProcessingParameterExpression
 * \ingroup core
 * An expression parameter for processing algorithms.
  * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingParameterExpression : public QgsProcessingParameterDefinition
{
  public:

    /**
     * Constructor for QgsProcessingParameterExpression.
     */
    QgsProcessingParameterExpression( const QString &name, const QString &description = QString(), const QVariant &defaultValue = QVariant(),
                                      const QString &parentLayerParameterName = QString(),
                                      bool optional = false );

    QString type() const override { return QStringLiteral( "expression" ); }

    /**
     * Returns the name of the parent layer parameter, or an empty string if this is not set.
     * \see setParentLayerParameter()
     */
    QString parentLayerParameter() const;

    /**
     * Sets the name of the parent layer parameter. Use an empty string if this is not required.
     * \see parentLayerParameter()
     */
    void setParentLayerParameter( const QString &parentLayerParameter );

  private:

    QString mParentLayerParameter;

};

/**
 * \class QgsProcessingParameterTable
 * \ingroup core
 * A table (i.e. vector layers with or without geometry) parameter for processing algorithms.
  * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingParameterTable : public QgsProcessingParameterDefinition
{
  public:

    /**
     * Constructor for QgsProcessingParameterTable.
     */
    QgsProcessingParameterTable( const QString &name, const QString &description = QString(), const QVariant &defaultValue = QVariant(),
                                 bool optional = false );

    QString type() const override { return QStringLiteral( "table" ); }

};

/**
 * \class QgsProcessingParameterTableField
 * \ingroup core
 * A table field parameter for processing algorithms.
  * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingParameterTableField : public QgsProcessingParameterDefinition
{
  public:

    //! Field data types
    enum DataType
    {
      Any = -1, //!< Accepts any field
      Numeric = 0, //!< Accepts numeric fields
      String = 1, //!< Accepts string fields
      DateTime = 2 //!< Accepts datetime fields
    };

    /**
     * Constructor for QgsProcessingParameterTableField.
     */
    QgsProcessingParameterTableField( const QString &name, const QString &description = QString(), const QVariant &defaultValue = QVariant(),
                                      const QString &parentLayerParameterName = QString(),
                                      DataType type = Any,
                                      bool allowMultiple = false,
                                      bool optional = false );

    QString type() const override { return QStringLiteral( "field" ); }
    bool checkValueIsAcceptable( const QVariant &input ) const override;

    /**
     * Returns the name of the parent layer parameter, or an empty string if this is not set.
     * \see setParentLayerParameter()
     */
    QString parentLayerParameter() const;

    /**
     * Sets the name of the parent layer parameter. Use an empty string if this is not required.
     * \see parentLayerParameter()
     */
    void setParentLayerParameter( const QString &parentLayerParameter );

    /**
     * Returns the acceptable data type for the field.
     * \see setDataType()
     */
    DataType dataType() const;

    /**
     * Sets the acceptable data \a type for the field.
     * \see dataType()
     */
    void setDataType( const DataType &type );

    /**
     * Returns whether multiple field selections are permitted.
     * \see setAllowMultiple()
     */
    bool allowMultiple() const;

    /**
     * Sets whether multiple field selections are permitted.
     * \see allowMultiple()
     */
    void setAllowMultiple( bool allowMultiple );

  private:

    QString mParentLayerParameter;
    DataType mDataType = Any;
    bool mAllowMultiple = false;

};

/**
 * \class QgsProcessingParameterVectorLayer
 * \ingroup core
 * A vector layer parameter for processing algorithms.
  * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingParameterVectorLayer : public QgsProcessingParameterDefinition
{
  public:

    /**
     * Constructor for QgsProcessingParameterVectorLayer.
     */
    QgsProcessingParameterVectorLayer( const QString &name, const QString &description = QString(), QgsProcessingParameterDefinition::LayerType type = QgsProcessingParameterDefinition::TypeVectorAny, const QVariant &defaultValue = QVariant(),
                                       bool optional = false );

    QString type() const override { return QStringLiteral( "vector" ); }


    /**
     * Returns the layer type for layers acceptable by the parameter.
     * \see setDataType()
     */
    QgsProcessingParameterDefinition::LayerType dataType() const;

    /**
     * Sets the layer \a type for layers acceptable by the parameter.
     * \see dataType()
     */
    void setDataType( QgsProcessingParameterDefinition::LayerType type );

  private:

    QgsProcessingParameterDefinition::LayerType mDataType;

};

#endif // QGSPROCESSINGPARAMETERS_H


