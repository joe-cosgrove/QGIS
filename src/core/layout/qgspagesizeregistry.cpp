/***************************************************************************
                            qgspagesizeregistry.cpp
                            ------------------------
    begin                : November 2014
    copyright            : (C) 2014 by Nyall Dawson
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

#include "qgspagesizeregistry.h"

//
// QgsPageSizeRegistry
//

QgsPageSizeRegistry::QgsPageSizeRegistry()
{
  add( QgsPageSize( QString( "A6" ), QgsLayoutSize( 105, 148, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "A5" ), QgsLayoutSize( 148, 210, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "A4" ), QgsLayoutSize( 210, 297, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "A3" ), QgsLayoutSize( 297, 420, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "A2" ), QgsLayoutSize( 420, 594, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "A1" ), QgsLayoutSize( 594, 841, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "A0" ), QgsLayoutSize( 841, 1189, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "B6" ), QgsLayoutSize( 125, 176, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "B5" ), QgsLayoutSize( 176, 250, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "B4" ), QgsLayoutSize( 250, 353, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "B3" ), QgsLayoutSize( 353, 500, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "B2" ), QgsLayoutSize( 500, 707, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "B1" ), QgsLayoutSize( 707, 1000, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "B0" ), QgsLayoutSize( 1000, 1414, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "Legal" ), QgsLayoutSize( 215.9, 355.6, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "Letter" ), QgsLayoutSize( 215.9, 279.4, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "ANSI A" ), QgsLayoutSize( 215.9, 279.4, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "ANSI B" ), QgsLayoutSize( 279.4, 431.8, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "ANSI C" ), QgsLayoutSize( 431.8, 558.8, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "ANSI D" ), QgsLayoutSize( 558.8, 863.6, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "ANSI E" ), QgsLayoutSize( 863.6, 1117.6, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "Arch A" ), QgsLayoutSize( 228.6, 304.8, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "Arch B" ), QgsLayoutSize( 304.8, 457.2, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "Arch C" ), QgsLayoutSize( 457.2, 609.6, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "Arch D" ), QgsLayoutSize( 609.6, 914.4, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "Arch E" ), QgsLayoutSize( 914.4, 1219.2, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "Arch E1" ), QgsLayoutSize( 762, 1066.8, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "Arch E2" ), QgsLayoutSize( 660, 965, QgsLayoutUnits::Millimeters ) ) );
  add( QgsPageSize( QString( "Arch E3" ), QgsLayoutSize( 686, 991, QgsLayoutUnits::Millimeters ) ) );
}

QgsPageSizeRegistry *QgsPageSizeRegistry::mInstance = 0;
QgsPageSizeRegistry *QgsPageSizeRegistry::instance()
{
  if ( mInstance == 0 )
  {
    mInstance = new QgsPageSizeRegistry();
  }
  return mInstance;
}

QgsPageSizeRegistry::~QgsPageSizeRegistry()
{

}

void QgsPageSizeRegistry::add( const QgsPageSize pageSize )
{
  mPageSizes.append( pageSize );
}

QList<QgsPageSize> QgsPageSizeRegistry::entries() const
{
  QList< QgsPageSize > result;
  QList< QgsPageSize >::const_iterator it = mPageSizes.constBegin();
  for ( ; it != mPageSizes.constEnd(); ++it )
  {
    result.push_back( *it );
  }
  return result;
}

QList<QgsPageSize> QgsPageSizeRegistry::find( const QString &name ) const
{
  QList< QgsPageSize > result;
  QList< QgsPageSize >::const_iterator it = mPageSizes.constBegin();
  for ( ; it != mPageSizes.constEnd(); ++it )
  {
    if (( *it ).name.compare( name, Qt::CaseInsensitive ) == 0 )
    {
      result.push_back( *it );
    }
  }
  return result;
}

bool QgsPageSizeRegistry::decodePageSize( const QString &pageSizeName, QgsPageSize &pageSize )
{
  QgsPageSizeRegistry* registry = QgsPageSizeRegistry::instance();
  QList< QgsPageSize > matches = registry->find( pageSizeName.trimmed() );
  if ( matches.length() > 0 )
  {
    pageSize = matches.at( 0 );
    return true;
  }
  return false;
}

//
// QgsPageSize
//

QgsPageSize::QgsPageSize()
    : name( QString() )
    , size( QgsLayoutSize( 0.0, 0.0 ) )
{
}

QgsPageSize::QgsPageSize( const QString& pageName, const QgsLayoutSize &pageSize )
    : name( pageName )
    , size( pageSize )
{
}

QgsPageSize::QgsPageSize( const QgsLayoutSize &pageSize )
    : name( QString() )
    , size( pageSize )
{

}

QgsPageSize::QgsPageSize( const QgsPageSize &other )
    : name( other.name )
    , size( other.size )
{

}

bool QgsPageSize::operator==( const QgsPageSize &other ) const
{
  return ( name == other.name && size == other.size );
}

bool QgsPageSize::operator!=( const QgsPageSize &other ) const
{
  return ( ! operator==( other ) );
}
