/***************************************************************************
                            qgspagesizeregistry.h
                            ----------------------
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
#ifndef QGSPAGESIZEREGISTRY_H
#define QGSPAGESIZEREGISTRY_H

#include "qgslayoutmeasurement.h"
#include <QString>
#include <QSizeF>
#include <QList>

/**\ingroup Layout
 * \class QgsPageSize
 * \brief A named page size for layouts.
 * \note added in QGIS 2.7
 */
class CORE_EXPORT QgsPageSize
{

  public:

    QgsPageSize();

    /**Constructor
     * @param pageName name of page size
     * @param pageSize size of page
    */
    QgsPageSize( const QString& pageName, const QgsLayoutSize& pageSize );

    /**Constructor
     * @param pageSize size of page
    */
    QgsPageSize( const QgsLayoutSize& pageSize );

    QgsPageSize( const QgsPageSize& other );

    /**Name of page size*/
    QString name;

    /**Page size*/
    QgsLayoutSize size;

    bool operator==( const QgsPageSize &other ) const;
    bool operator!=( const QgsPageSize &other ) const;
};

/**\ingroup Layout
 * \class QgsPageSizeRegistry
 * \brief Singleton registry for known page sizes.
 * \note added in QGIS 2.7
 */
class CORE_EXPORT QgsPageSizeRegistry
{
  public:

    /**Returns the instance pointer, creating and populating the registry on the first call
     */
    static QgsPageSizeRegistry* instance();

    virtual ~QgsPageSizeRegistry();

    /**Adds a page size to the registry.
     * @param pageSize page size to add
     */
    void add( const QgsPageSize pageSize );

    /**Returns a list of page sizes in the registry
     * @returns list of known page sizes
     */
    QList< QgsPageSize > entries() const;

    /**Finds matching page sizes from the registry, using a case insensitive match
     * on the page size name
     * @param name name of page size to match
     * @returns list of matching page sizes
     */
    QList< QgsPageSize > find( const QString& name ) const;

    /**Decodes a string representing a preset page size.
     * @param pageSizeName string to decode
     * @param pageSize QgsPageSize for storing decoded page size
     * @returns true if string was successfully decoded
    */
    bool decodePageSize( const QString& pageSizeName, QgsPageSize& pageSize );

  protected:

    /**Creates a registry and populates it with known sizes
     */
    QgsPageSizeRegistry();

  private:

    static QgsPageSizeRegistry *mInstance;

    QList< QgsPageSize > mPageSizes;
};

#endif //QGSPAGESIZEREGISTRY_H
