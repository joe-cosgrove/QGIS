/***************************************************************************
                         qgsprocessingparameters.cpp
                         --------------------------
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

#include "qgsprocessingparameters.h"
#include "qgscoordinatereferencesystem.h"
#include "qgsmaplayer.h"
#include <QRegularExpression>


//
// QgsProcessingParameter
//

QgsProcessingParameter::QgsProcessingParameter( const QString &name, const QString &description, bool optional )
  : mName( name )
  , mDescription( description )
  , mFlags( optional ? QgsProcessingParameter::FlagOptional : 0 )
{}

void QgsProcessingParameter::setDescription( const QString &description )
{
  mDescription = description;
}


bool QgsProcessingParameter::setDefaultValue( const QVariant &value )
{
  if ( !acceptsValue( value ) )
    return false;

  mDefault = parseValue( value );
  return true;
}

void QgsProcessingParameter::setFlags( const Flags &flags )
{
  mFlags = flags;
}

QString QgsProcessingParameter::asScriptCode() const
{
  QString code = QStringLiteral( "##%1=" ).arg( mName );
  if ( mFlags && FlagOptional )
    code += QStringLiteral( "optional " );
  code += type() + ' ';
  code += mDefault.toString();
  return code;
}

//
// QgsProcessingParameterBoolean
//

QgsProcessingParameterBoolean::QgsProcessingParameterBoolean( const QString &name, const QString &description, bool defaultValue, bool optional )
  : QgsProcessingParameter( name, description, optional )
{
  setDefaultValue( defaultValue );
}

bool QgsProcessingParameterBoolean::acceptsValue( const QVariant &value ) const
{
  if ( !value.isValid() && !( flags() & FlagOptional ) )
    return false;

  return true;
}

QVariant QgsProcessingParameterBoolean::parseValue( const QVariant &value ) const
{
  if ( !value.isValid() )
    return defaultValue();

  return convertToBool( value );
}

QgsProcessingParameter *QgsProcessingParameterBoolean::createFromScriptCode( const QString &name, const QString &description, bool isOptional, const QString &definition )
{
  bool defaultVal = convertToBool( definition );
  return new QgsProcessingParameterBoolean( name, description, defaultVal, isOptional );
}

QString QgsProcessingParameterBoolean::asScriptCode() const
{
  QString code = QStringLiteral( "##%1=" ).arg( name() );
  if ( flags() && FlagOptional )
    code += QStringLiteral( "optional " );
  code += type() + ' ';
  code += defaultValue().toBool() ? "true" : "false";
  return code;
}

bool QgsProcessingParameterBoolean::convertToBool( const QVariant &value )
{
  if ( value.type() == QVariant::String )
    return value.toString().toLower().trimmed() == QStringLiteral( "true" );
  else
    return value.toBool();
}


//
// QgsProcessingParameterCrs
//

QgsProcessingParameterCrs::QgsProcessingParameterCrs( const QString &name, const QString &description, const QVariant &defaultValue, bool optional )
  : QgsProcessingParameter( name, description, optional )
{
  setDefaultValue( defaultValue );
}

bool QgsProcessingParameterCrs::acceptsValue( const QVariant &value ) const
{
  if ( flags() & FlagOptional )
    return true;

  if ( !value.isValid() )
    return false;

  // is it a QgsCoordinateReferenceSystem?
  if ( value.canConvert< QgsCoordinateReferenceSystem >() )
  {
    QgsCoordinateReferenceSystem crs = value.value< QgsCoordinateReferenceSystem >();
    return crs.isValid();
  }

  // is it a map layer?
  QObject *o = qvariant_cast<QObject *>( value );
  if ( QgsMapLayer *l = qobject_cast< QgsMapLayer * >( o ) )
  {
    return l->crs().isValid();
  }

  // try various string parsing
  QString s = value.toString();
  if ( s == QStringLiteral( "ProjectCrs" ) )
    return true;

  QgsCoordinateReferenceSystem crs = QgsCoordinateReferenceSystem::createFromString( s );
  return crs.isValid();
}

QVariant QgsProcessingParameterCrs::parseValue( const QVariant &value ) const
{

}

QString QgsProcessingParameterCrs::asScriptCode() const
{

}

QgsProcessingParameter *QgsProcessingParameterCrs::createFromScriptCode( const QString &name, const QString &description, bool isOptional, const QString &definition )
{

}
