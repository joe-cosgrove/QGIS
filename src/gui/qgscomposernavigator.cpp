/***************************************************************************
                         qgscomposernavigator.cpp
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

#include <QApplication>
#include <QMainWindow>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QClipboard>
#include <QMimeData>
#include <QGridLayout>

#include "qgscomposernavigator.h"
#include "qgscomposerarrow.h"
#include "qgscomposerframe.h"
#include "qgscomposerhtml.h"
#include "qgscomposerlabel.h"
#include "qgscomposerlegend.h"
#include "qgscomposermap.h"
#include "qgscomposeritemgroup.h"
#include "qgscomposerpicture.h"
#include "qgscomposerruler.h"
#include "qgscomposerscalebar.h"
#include "qgscomposershape.h"
#include "qgscomposerattributetable.h"
#include "qgslogger.h"
#include "qgsaddremovemultiframecommand.h"
#include "qgspaperitem.h"

QgsComposerNavigator::QgsComposerNavigator( QWidget* parent, const char* name, Qt::WFlags f )
    : QGraphicsView( parent )
    , mPaintingEnabled( true )
{
  Q_UNUSED( f );
  Q_UNUSED( name );

  setResizeAnchor( QGraphicsView::AnchorViewCenter );
  setMouseTracking( true );
  viewport()->setMouseTracking( true );
  setFrameShape( QFrame::NoFrame );
}

void QgsComposerNavigator::mousePressEvent( QMouseEvent* e )
{
  if ( !composition() )
  {
    return;
  }

  QPointF scenePoint = mapToScene( e->pos() );

}

void QgsComposerNavigator::mouseReleaseEvent( QMouseEvent* e )
{
  if ( !composition() )
  {
    return;
  }

  QPointF scenePoint = mapToScene( e->pos() );

}

void QgsComposerNavigator::mouseMoveEvent( QMouseEvent* e )
{
  if ( !composition() )
  {
    return;
  }

}

void QgsComposerNavigator::wheelEvent( QWheelEvent* event )
{
  QPointF scenePoint = mapToScene( event->pos() );

}

void QgsComposerNavigator::paintEvent( QPaintEvent* event )
{
  if ( mPaintingEnabled )
  {
    QGraphicsView::paintEvent( event );
    event->accept();
  }
  else
  {
    event->ignore();
  }
}

void QgsComposerNavigator::hideEvent( QHideEvent* e )
{
  emit( composerViewHide( this ) );
  e->ignore();
}

void QgsComposerNavigator::showEvent( QShowEvent* e )
{
  emit( composerViewShow( this ) );
  e->ignore();
}

void QgsComposerNavigator::setComposition( QgsComposition* c )
{
  setScene( c );
}

QgsComposition* QgsComposerNavigator::composition()
{
  if ( scene() )
  {
    QgsComposition* c = dynamic_cast<QgsComposition *>( scene() );
    if ( c )
    {
      return c;
    }
  }
  return 0;
}

QMainWindow* QgsComposerNavigator::composerWindow()
{
  QMainWindow* composerObject = 0;
  QObject* currentObject = parent();
  if ( !currentObject )
  {
    return qobject_cast<QMainWindow *>( currentObject );
  }

  while ( true )
  {
    composerObject = qobject_cast<QMainWindow*>( currentObject );
    if ( composerObject || currentObject->parent() == 0 )
    {
      return composerObject;
    }
    currentObject = currentObject->parent();
  }

  return 0;
}
