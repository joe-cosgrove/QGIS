/***************************************************************************
  qgsgeometryvalidator.h - geometry validation thread
  -------------------------------------------------------------------
Date                 : 03.01.2012
Copyright            : (C) 2012 by Juergen E. Fischer
email                : jef at norbit dot de
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSGEOMETRYVALIDATOR_H
#define QGSGEOMETRYVALIDATOR_H

#include <QThread>
#include "qgsgeometry.h"
class QgsSurfaceV2;
class QgsLineStringV2;

class CORE_EXPORT QgsGeometryValidator : public QThread
{
    Q_OBJECT

  public:
    //! Constructor
    QgsGeometryValidator( const QgsGeometry *g, QList<QgsGeometry::Error> *errors = nullptr );
    ~QgsGeometryValidator();

    void run() override;
    void stop();

    /** Validate geometry and produce a list of geometry errors */
    static void validateGeometry( const QgsGeometry *g, QList<QgsGeometry::Error> &errors );

  signals:
    void errorFound( const QgsGeometry::Error& );

  public slots:
    void addError( const QgsGeometry::Error& );

  private:
    void validatePolyline( int i, QgsPolyline polyline, bool ring = false );
    void validatePolyline(int i, const QgsLineStringV2* line, bool ring);
    void validatePolygon( int i, const QgsPolygon &polygon );
    void validatePolygon( int idx, const QgsPolygonV2* polygon );
    void checkRingIntersections( int p0, int i0, const QgsPolyline &ring0, int p1, int i1, const QgsPolyline &ring1 );
    void checkRingIntersections(int p0, int i0, const QgsLineStringV2* ring0, int p1, int i1, const QgsLineStringV2* ring1);
    double distLine2Point( double px, double py, QgsVector v, const QgsPoint& q );
    bool intersectLines( double px, double py, QgsVector v, double qx, double qy, QgsVector w, QgsPoint& s );
    bool ringInRing( const QgsPolyline &inside, const QgsPolyline &outside );
    bool ringInRing( const QgsLineStringV2* inside, const QgsLineStringV2* outside );
    bool pointInRing( const QgsPolyline &ring, const QgsPoint &p );
    bool pointInRing( const QgsLineStringV2* ring, const QgsPointV2& p );

    QgsGeometry mG;
    QList<QgsGeometry::Error> *mErrors;
    bool mStop;
    int mErrorCount;

}; // class QgsGeometryValidator

#endif
