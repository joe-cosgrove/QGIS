/***************************************************************************
                         qgscomposernavigator.h
                             -------------------
    begin                : January 2005
    copyright            : (C) 2005 by Radim Blazek
    email                : blazek@itc.it
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QGSCOMPOSERNAVIGATOR_H
#define QGSCOMPOSERNAVIGATOR_H

#include <QGraphicsView>
#include "qgsaddremoveitemcommand.h"

class QDomDocument;
class QDomElement;
class QKeyEvent;
class QMainWindow;
class QMouseEvent;
class QgsComposition;
class QgsComposerArrow;
class QgsComposerItem;
class QgsComposerLabel;
class QgsComposerLegend;
class QgsComposerMap;
class QgsComposerPicture;
class QgsComposerRuler;
class QgsComposerScaleBar;
class QgsComposerShape;
class QgsComposerAttributeTable;

/** \ingroup MapComposer
 * \ingroup gui
 * Widget to display the composer items. Manages the composer tools and the
 * mouse/key events.
 * Creates the composer items according to the current map tools and keeps track
 * of the rubber band item.
 */
class GUI_EXPORT QgsComposerNavigator: public QGraphicsView
{
    Q_OBJECT

  public:

    QgsComposerNavigator( QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0 );

    /**Sets composition (derived from QGraphicsScene)*/
    void setComposition( QgsComposition* c );
    /**Returns the composition or 0 in case of error*/
    QgsComposition* composition();

    /**Returns the composer main window*/
    QMainWindow* composerWindow();

    void setPaintingEnabled( bool enabled ) { mPaintingEnabled = enabled; }
    bool paintingEnabled() const { return mPaintingEnabled; }

  protected:
    void mousePressEvent( QMouseEvent* );
    void mouseReleaseEvent( QMouseEvent* );
    void mouseMoveEvent( QMouseEvent* );

    void wheelEvent( QWheelEvent* event );

    void paintEvent( QPaintEvent* event );

    void hideEvent( QHideEvent* e );
    void showEvent( QShowEvent* e );

  private:
    bool mPaintingEnabled;

  signals:

    /**Emitted before composerview is shown*/
    void composerViewShow( QgsComposerNavigator* );
    /**Emitted before composerview is hidden*/
    void composerViewHide( QgsComposerNavigator* );
};

#endif //QGSCOMPOSERNAVIGATOR
