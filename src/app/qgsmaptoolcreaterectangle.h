/***************************************************************************
    qgsmaptoolcreaterectangle.h
    ---------------------------
    begin                : Dec 2016
    copyright            : (C) 2016 by Nyall Dawson
    email                : nyall dot dawson at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSMAPTOOLCREATERECTANGLE_H
#define QGSMAPTOOLCREATERECTANGLE_H

#include "qgsmaptooladdfeature.h"
#include "qgsrubberband.h"

class APP_EXPORT QgsMapToolCreateRectangle: public QgsMapToolAddFeature
{
    Q_OBJECT

  public:
    QgsMapToolCreateRectangle( QgsMapCanvas* canvas );

    virtual QgsGeometry captureCurve() const override;

  protected:
    virtual QList< QgsPoint > temporaryRubberBandPoints( const QVector< QgsPoint >& clickedPoints, const QgsPoint& mousePoint,
        const QVector< QgsPoint >& tracedPoints, QgsMapMouseEvent * e ) const override;


  private:

    QList< QgsPointV2 > mPoints;

    //! Temporary rubber band for polylines and polygons. this connects the last added point to the mouse cursor position
    QScopedPointer< QgsRubberBand > mRectRubberBand;

};

#endif // QGSMAPTOOLCREATERECTANGLE_H
