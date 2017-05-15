/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/core/processing/qgsprocessingoutputs.h                           *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/






class QgsProcessingOutputDefinition : QgsProcessingParameterDefinition
{
%Docstring

 Base class for the definition of processing outputs.

 Output definitions encapsulate the properties regarding the outputs from algorithms, such
 as generated layers or calculated values.

.. versionadded:: 3.0
%End

%TypeHeaderCode
#include "qgsprocessingoutputs.h"
%End
  public:

    QgsProcessingOutputDefinition( const QString &name, const QString &description = QString(), const QVariant &defaultValue = QVariant(),
                                   bool optional = false );
%Docstring
 Constructor for QgsProcessingOutputDefinition.
%End

};

typedef QList< const QgsProcessingOutputDefinition * > QgsProcessingOutputDefinitions;


class QgsProcessingOutputVectorLayer : QgsProcessingOutputDefinition
{
%Docstring
 A vector layer output for processing algorithms.
.. versionadded:: 3.0
%End

%TypeHeaderCode
#include "qgsprocessingoutputs.h"
%End
  public:

    QgsProcessingOutputVectorLayer( const QString &name, const QString &description = QString(), const QVariant &defaultValue = QVariant(),
                                    bool optional = false );
%Docstring
 Constructor for QgsProcessingOutputVectorLayer.
%End

    virtual QString type() const;

};



/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/core/processing/qgsprocessingoutputs.h                           *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/
