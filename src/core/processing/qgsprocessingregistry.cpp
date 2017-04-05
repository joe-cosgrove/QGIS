/***************************************************************************
                         qgsprocessingregistry.cpp
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

#include "qgsprocessingregistry.h"
#include "qgsvectorfilewriter.h"
#include "qgsprocessingparameters.h"

QgsProcessingRegistry::QgsProcessingRegistry( QObject *parent SIP_TRANSFERTHIS )
  : QObject( parent )
{
  //add known parameter types
  addParameterType( new QgsProcessingParameterMetadata( QStringLiteral( "boolean" ), QgsProcessingParameterBoolean::createFromScriptCode ) );
  addParameterType( new QgsProcessingParameterMetadata( QStringLiteral( "crs" ), QgsProcessingParameterCrs::createFromScriptCode ) );
}

QgsProcessingRegistry::~QgsProcessingRegistry()
{
  Q_FOREACH ( QgsProcessingProvider *p, mProviders )
  {
    removeProvider( p );
  }
  qDeleteAll( mParameterMetadata );
}

bool QgsProcessingRegistry::addProvider( QgsProcessingProvider *provider )
{
  if ( !provider )
    return false;

  if ( mProviders.contains( provider->id() ) )
    return false;

  if ( !provider->load() )
    return false;

  provider->setParent( this );
  mProviders[ provider->id()] = provider;
  emit providerAdded( provider->id() );
  return true;
}

bool QgsProcessingRegistry::removeProvider( QgsProcessingProvider *provider )
{
  if ( !provider )
    return false;

  QString id = provider->id();

  if ( !mProviders.contains( id ) )
    return false;

  provider->unload();

  delete mProviders.take( id );
  emit providerRemoved( id );
  return true;
}

bool QgsProcessingRegistry::removeProvider( const QString &providerId )
{
  QgsProcessingProvider *p = providerById( providerId );
  return removeProvider( p );
}

QgsProcessingProvider *QgsProcessingRegistry::providerById( const QString &id )
{
  return mProviders.value( id, nullptr );
}

QList< QgsProcessingAlgorithm * > QgsProcessingRegistry::algorithms() const
{
  QList< QgsProcessingAlgorithm * > algs;
  QMap<QString, QgsProcessingProvider *>::const_iterator it = mProviders.constBegin();
  for ( ; it != mProviders.constEnd(); ++it )
  {
    algs.append( it.value()->algorithms() );
  }
  return algs;
}

QgsProcessingAlgorithm *QgsProcessingRegistry::algorithmById( const QString &id ) const
{
  QMap<QString, QgsProcessingProvider *>::const_iterator it = mProviders.constBegin();
  for ( ; it != mProviders.constEnd(); ++it )
  {
    Q_FOREACH ( QgsProcessingAlgorithm *alg, it.value()->algorithms() )
      if ( alg->id() == id )
        return alg;
  }
  return nullptr;
}

QgsProcessingParameterAbstractMetadata *QgsProcessingRegistry::parameterMetadata( const QString &type ) const
{
  return mParameterMetadata.value( type );
}

bool QgsProcessingRegistry::addParameterType( QgsProcessingParameterAbstractMetadata *metadata )
{
  if ( !metadata || mParameterMetadata.contains( metadata->type() ) )
    return false;

  mParameterMetadata[metadata->type()] = metadata;
  return true;
}

QgsProcessingParameter *QgsProcessingRegistry::createParameterFromScriptCode( const QString &code ) const
{
  bool isOptional = false;
  QString name;
  QString definition;
  QString type;
  if ( !parseScriptCodeParameterOptions( code, isOptional, name, type, definition ) )
    return nullptr;

  if ( !mParameterMetadata.contains( type ) )
    return nullptr;

  QString description = createParameterDescription( name );

  return mParameterMetadata[type]->createParameterFromScriptCode( name, description, isOptional, definition );
}

bool QgsProcessingRegistry::parseScriptCodeParameterOptions( const QString &code, bool &isOptional, QString &name, QString &type, QString &definition )
{
  QRegularExpression re( "##(.*?)=\\s*(.*)" );
  QRegularExpressionMatch m = re.match( code );
  if ( !m.hasMatch() )
    return false;

  name = m.captured( 1 );
  QString tokens = m.captured( 2 );
  if ( tokens.toLower().startsWith( QStringLiteral( "optional" ) ) )
  {
    isOptional = true;
    tokens.remove( 0, 9 ); // length "optional" = 8
  }
  else
  {
    isOptional = false;
  }

  QRegularExpression re2( "(.*?)\\s+(.*)" );
  m = re2.match( tokens );
  if ( !m.hasMatch() )
  {
    type = tokens;
    definition.clear();
  }
  else
  {
    type = m.captured( 1 );
    definition = m.captured( 2 );
  }
  return true;
}

QString QgsProcessingRegistry::createParameterDescription( const QString &name )
{
  QString desc = name;
  return desc.replace( '_', ' ' );
}


QgsProcessingParameterAbstractMetadata::QgsProcessingParameterAbstractMetadata( const QString &type )
  : mType( type )
{}

QgsProcessingParameter *QgsProcessingParameterAbstractMetadata::createParameterFromScriptCode( const QString &name, const QString &description, bool isOptional, const QString &definition )
{
  Q_UNUSED( definition );
  Q_UNUSED( description );
  Q_UNUSED( name );
  Q_UNUSED( isOptional );
  return nullptr;
}

QgsProcessingParameterMetadata::QgsProcessingParameterMetadata( const QString &type, QgsProcessingParameterFromScriptCodeFunc pfCreateFromScriptCode )
  : QgsProcessingParameterAbstractMetadata( type )
  , mCreateFromScriptCodeFunc( pfCreateFromScriptCode )
{}
