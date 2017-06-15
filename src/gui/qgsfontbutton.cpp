/***************************************************************************
     qgsfontbutton.h
     ---------------
    Date                 : May 2017
    Copyright            : (C) 2017 by Nyall Dawson
    Email                : nyall dot dawson at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsfontbutton.h"
#include "qgstextformatwidget.h"
#include "qgssymbollayerutils.h"
#include "qgscolorscheme.h"
#include "qgsmapcanvas.h"
#include "qgscolorwidgets.h"
#include "qgscolorschemeregistry.h"
#include "qgscolorswatchgrid.h"
#include "qgsdoublespinbox.h"
#include "qgsunittypes.h"
#include "qgsmenuheader.h"
#include <QMenu>
#include <QClipboard>
#include <QDrag>
#include <QDesktopWidget>

QgsFontButton::QgsFontButton( QWidget *parent, const QString &dialogTitle )
  : QToolButton( parent )
  , mDialogTitle( dialogTitle.isEmpty() ? tr( "Text Format" ) : dialogTitle )
  , mMenu( nullptr )

{
  setAcceptDrops( true );
  setMinimumSize( QSize( 24, 16 ) );
  connect( this, &QAbstractButton::clicked, this, &QgsFontButton::showSettingsDialog );

  //setup dropdown menu
  mMenu = new QMenu( this );
  connect( mMenu, &QMenu::aboutToShow, this, &QgsFontButton::prepareMenu );
  setMenu( mMenu );
  setPopupMode( QToolButton::MenuButtonPopup );
}

QSize QgsFontButton::sizeHint() const
{
  //make sure height of button looks good under different platforms
#ifdef Q_OS_WIN
  return QSize( 120, 22 );
#else
  return QSize( 120, 28 );
#endif
}

void QgsFontButton::showSettingsDialog()
{
  QgsPanelWidget *panel = QgsPanelWidget::findParentPanel( this );
  if ( panel && panel->dockMode() )
  {
    QgsTextFormatPanelWidget *formatWidget = new QgsTextFormatPanelWidget( mFormat, mMapCanvas, this );
    formatWidget->setPanelTitle( mDialogTitle );

    connect( formatWidget, &QgsTextFormatPanelWidget::widgetChanged, this, [ this, formatWidget ] { this->setTextFormat( formatWidget->format() ); } );
    panel->openPanel( formatWidget );
    return;
  }

  QgsTextFormatDialog dialog( mFormat, mMapCanvas, this );
  dialog.setWindowTitle( mDialogTitle );
  if ( dialog.exec() )
  {
    setTextFormat( dialog.format() );
  }

  // reactivate button's window
  activateWindow();
}

QgsMapCanvas *QgsFontButton::mapCanvas() const
{
  return mMapCanvas;
}

void QgsFontButton::setMapCanvas( QgsMapCanvas *mapCanvas )
{
  mMapCanvas = mapCanvas;
}

void QgsFontButton::setTextFormat( const QgsTextFormat &format )
{
  mFormat = format;
  updatePreview();
  emit changed();
}

void QgsFontButton::setColor( const QColor &color )
{
  QColor opaque = color;
  opaque.setAlphaF( 1.0 );

  if ( mFormat.color() != opaque )
  {
    mFormat.setColor( opaque );
    updatePreview();
    emit changed();
  }
}

void QgsFontButton::copyFormat()
{
  QApplication::clipboard()->setMimeData( mFormat.toMimeData() );
}

void QgsFontButton::pasteFormat()
{
  QgsTextFormat tempFormat;
  if ( formatFromMimeData( QApplication::clipboard()->mimeData(), tempFormat ) )
  {
    setTextFormat( tempFormat );
  }
}

bool QgsFontButton::event( QEvent *e )
{
  if ( e->type() == QEvent::ToolTip )
  {
    QString toolTip = QStringLiteral( "%1\nSize: %2" ).arg( mFormat.font().family() ).arg( mFormat.size() );
    setToolTip( toolTip );
  }
  return QToolButton::event( e );
}

void QgsFontButton::mousePressEvent( QMouseEvent *e )
{
  if ( e->button() == Qt::RightButton )
  {
    QToolButton::showMenu();
    return;
  }
  else if ( e->button() == Qt::LeftButton )
  {
    mDragStartPosition = e->pos();
  }
  QToolButton::mousePressEvent( e );
}

void QgsFontButton::mouseMoveEvent( QMouseEvent *e )
{
  //handle dragging fonts from button

  if ( !( e->buttons() & Qt::LeftButton ) )
  {
    //left button not depressed, so not a drag
    QToolButton::mouseMoveEvent( e );
    return;
  }

  if ( ( e->pos() - mDragStartPosition ).manhattanLength() < QApplication::startDragDistance() )
  {
    //mouse not moved, so not a drag
    QToolButton::mouseMoveEvent( e );
    return;
  }

  //user is dragging color
  QDrag *drag = new QDrag( this );
  drag->setMimeData( mFormat.toMimeData() );
  drag->setPixmap( createDragIcon() );
  drag->exec( Qt::CopyAction );
  setDown( false );
}

bool QgsFontButton::colorFromMimeData( const QMimeData *mimeData, QColor &resultColor, bool &hasAlpha )
{
  hasAlpha = false;
  QColor mimeColor = QgsSymbolLayerUtils::colorFromMimeData( mimeData, hasAlpha );

  if ( mimeColor.isValid() )
  {
    resultColor = mimeColor;
    return true;
  }

  //could not get color from mime data
  return false;
}

void QgsFontButton::dragEnterEvent( QDragEnterEvent *e )
{
  //is dragged data valid color data?
  QColor mimeColor;
  QgsTextFormat format;
  bool hasAlpha = false;

  if ( formatFromMimeData( e->mimeData(), format ) )
  {
    e->acceptProposedAction();
    updatePreview( QColor(), &format );
  }
  else if ( colorFromMimeData( e->mimeData(), mimeColor, hasAlpha ) )
  {
    //if so, we accept the drag, and temporarily change the button's color
    //to match the dragged color. This gives immediate feedback to the user
    //that colors can be dropped here
    e->acceptProposedAction();
    updatePreview( mimeColor );
  }
}

void QgsFontButton::dragLeaveEvent( QDragLeaveEvent *e )
{
  Q_UNUSED( e );
  //reset button color
  updatePreview();
}

void QgsFontButton::dropEvent( QDropEvent *e )
{
  //is dropped data valid color data?
  QColor mimeColor;
  QgsTextFormat format;
  bool hasAlpha = false;
  if ( formatFromMimeData( e->mimeData(), format ) )
  {
    setTextFormat( format );
    return;
  }
  else if ( colorFromMimeData( e->mimeData(), mimeColor, hasAlpha ) )
  {
    //accept drop and set new color
    e->acceptProposedAction();

    if ( hasAlpha )
    {
      mFormat.setOpacity( mimeColor.alphaF() );
    }
    mimeColor.setAlphaF( 1.0 );
    mFormat.setColor( mimeColor );
    QgsRecentColorScheme::addRecentColor( mimeColor );
    updatePreview();
    emit changed();
  }
  updatePreview();
}

QPixmap QgsFontButton::createMenuIcon( const QColor &color ) const
{
  //create an icon pixmap
  QPixmap pixmap( 16, 16 );
  pixmap.fill( Qt::transparent );

  QPainter p;
  p.begin( &pixmap );

  //draw color over pattern
  p.setBrush( QBrush( color ) );

  //draw border
  p.setPen( QColor( 197, 197, 197 ) );
  p.drawRect( 0, 0, 15, 15 );
  p.end();
  return pixmap;
}

QPixmap QgsFontButton::createDragIcon( QSize size, const QgsTextFormat *tempFormat ) const
{
  if ( !tempFormat )
    tempFormat = &mFormat;

  //create an icon pixmap
  QPixmap pixmap( size.width(), size.height() );
  pixmap.fill( Qt::transparent );
  QPainter p;
  p.begin( &pixmap );
  p.setRenderHint( QPainter::Antialiasing );
  QRect rect( 0, 0, size.width(), size.height() );

  if ( tempFormat->color().lightnessF() < 0.7 )
  {
    p.setBrush( QBrush( QColor( 255, 255, 255 ) ) );
    p.setPen( QPen( QColor( 150, 150, 150 ), 0 ) );
  }
  else
  {
    p.setBrush( QBrush( QColor( 0, 0, 0 ) ) );
    p.setPen( QPen( QColor( 100, 100, 100 ), 0 ) );
  }
  p.drawRect( rect );
  p.setBrush( Qt::NoBrush );
  p.setPen( Qt::NoPen );


  QgsRenderContext context;
  QgsMapToPixel newCoordXForm;
  newCoordXForm.setParameters( 1, 0, 0, 0, 0, 0 );
  context.setMapToPixel( newCoordXForm );

  context.setScaleFactor( QgsApplication::desktop()->logicalDpiX() / 25.4 );
  context.setUseAdvancedEffects( true );
  context.setPainter( &p );

  // slightly inset text to account for buffer/background
  double xtrans = 0;
  if ( tempFormat->buffer().enabled() )
    xtrans = context.convertToPainterUnits( tempFormat->buffer().size(), tempFormat->buffer().sizeUnit(), tempFormat->buffer().sizeMapUnitScale() );
  if ( tempFormat->background().enabled() && tempFormat->background().sizeType() != QgsTextBackgroundSettings::SizeFixed )
    xtrans = qMax( xtrans, context.convertToPainterUnits( tempFormat->background().size().width(), tempFormat->background().sizeUnit(), tempFormat->background().sizeMapUnitScale() ) );

  double ytrans = 0.0;
  if ( tempFormat->buffer().enabled() )
    ytrans = qMax( ytrans, context.convertToPainterUnits( tempFormat->buffer().size(), tempFormat->buffer().sizeUnit(), tempFormat->buffer().sizeMapUnitScale() ) );
  if ( tempFormat->background().enabled() )
    ytrans = qMax( ytrans, context.convertToPainterUnits( tempFormat->background().size().height(), tempFormat->background().sizeUnit(), tempFormat->background().sizeMapUnitScale() ) );

  QRectF textRect = rect;
  textRect.setLeft( xtrans );
  textRect.setWidth( textRect.width() - xtrans );
  textRect.setTop( ytrans );
  if ( textRect.height() > 300 )
    textRect.setHeight( 300 );
  if ( textRect.width() > 2000 )
    textRect.setWidth( 2000 );

  QgsTextRenderer::drawText( textRect, 0, QgsTextRenderer::AlignCenter, QStringList() << tr( "Aa" ),
                             context, *tempFormat );

  p.end();
  return pixmap;
}

void QgsFontButton::prepareMenu()
{
  //we need to tear down and rebuild this menu every time it is shown. Otherwise the space allocated to any
  //QgsColorSwatchGridAction is not recalculated by Qt and the swatch grid may not be the correct size
  //for the number of colors shown in the grid. Note that we MUST refresh color swatch grids every time this
  //menu is opened, otherwise color schemes like the recent color scheme grid are meaningless
  mMenu->clear();

#if 0 // too slow if many fonts...
  QMenu *fontMenu = new QMenu( mFormat.font().family(), mMenu );
  QFontDatabase db;
  Q_FOREACH ( const QString &family, db.families() )
  {
    QAction *fontAction = new QAction( family, fontMenu );
    QFont f = fontAction->font();
    f.setFamily( family );
    fontAction->setFont( f );
    fontAction->setToolTip( family );
    fontMenu->addAction( fontAction );
    if ( family == mFormat.font().family() )
    {
      fontAction->setCheckable( true );
      fontAction->setChecked( true );
    }
    auto setFont = [this, family]
    {
      QFont f = mFormat.font();
      f.setFamily( family );
      mFormat.setFont( f );
      updatePreview();
      emit changed();
    };
    connect( fontAction, &QAction::triggered, this, setFont );
  }
  mMenu->addMenu( fontMenu );
#endif

  QWidgetAction *sizeAction = new QWidgetAction( mMenu );
  QWidget *sizeWidget = new QWidget();
  QVBoxLayout *sizeLayout = new QVBoxLayout();
  sizeLayout->setMargin( 0 );
  sizeLayout->setContentsMargins( 0, 0, 0, 3 );
  sizeLayout->setSpacing( 2 );

  QgsMenuHeader *sizeLabel = new QgsMenuHeader( tr( "Font size (%1)" ).arg( QgsUnitTypes::toString( mFormat.sizeUnit() ) ) );
  sizeLayout->addWidget( sizeLabel );

  QgsDoubleSpinBox *sizeSpin = new QgsDoubleSpinBox( nullptr );
  sizeSpin->setDecimals( 4 );
  sizeSpin->setMaximum( 1e+9 );
  sizeSpin->setShowClearButton( false );
  sizeSpin->setValue( mFormat.size() );
  connect( sizeSpin, static_cast < void ( QgsDoubleSpinBox::* )( double ) > ( &QgsDoubleSpinBox::valueChanged ),
           this, [ = ]( double value )
  {
    mFormat.setSize( value );
    updatePreview();
    emit changed();
  } );
  QHBoxLayout *spinLayout = new QHBoxLayout();
  spinLayout->setMargin( 0 );
  spinLayout->setContentsMargins( 4, 0, 4, 0 );
  spinLayout->addWidget( sizeSpin );
  sizeLayout->addLayout( spinLayout );
  sizeWidget->setLayout( sizeLayout );
  sizeAction->setDefaultWidget( sizeWidget );
  sizeWidget->setFocusProxy( sizeSpin );
  sizeWidget->setFocusPolicy( Qt::StrongFocus );
  mMenu->addAction( sizeAction );

  QAction *configureAction = new QAction( tr( "Configure format..." ), this );
  mMenu->addAction( configureAction );
  connect( configureAction, &QAction::triggered, this, &QgsFontButton::showSettingsDialog );

  QAction *copyFormatAction = new QAction( tr( "Copy format" ), this );
  mMenu->addAction( copyFormatAction );
  connect( copyFormatAction, &QAction::triggered, this, &QgsFontButton::copyFormat );
  QAction *pasteFormatAction = new QAction( tr( "Paste format" ), this );
  //enable or disable paste action based on current clipboard contents. We always show the paste
  //action, even if it's disabled, to give hint to the user that pasting colors is possible
  QgsTextFormat tempFormat;
  if ( formatFromMimeData( QApplication::clipboard()->mimeData(), tempFormat ) )
  {
    tempFormat.setSizeUnit( QgsUnitTypes::RenderPixels );
    tempFormat.setSize( 14 );
    pasteFormatAction->setIcon( createDragIcon( QSize( 16, 16 ), &tempFormat ) );
  }
  else
  {
    pasteFormatAction->setEnabled( false );
  }
  mMenu->addAction( pasteFormatAction );
  connect( pasteFormatAction, &QAction::triggered, this, &QgsFontButton::pasteFormat );

  mMenu->addSeparator();

  QgsColorWheel *colorWheel = new QgsColorWheel( mMenu );
  colorWheel->setColor( mFormat.color() );
  QgsColorWidgetAction *colorAction = new QgsColorWidgetAction( colorWheel, mMenu, mMenu );
  colorAction->setDismissOnColorSelection( false );
  connect( colorAction, &QgsColorWidgetAction::colorChanged, this, &QgsFontButton::setColor );
  mMenu->addAction( colorAction );

  QgsColorRampWidget *alphaRamp = new QgsColorRampWidget( mMenu, QgsColorWidget::Alpha, QgsColorRampWidget::Horizontal );
  QColor alphaColor = mFormat.color();
  alphaColor.setAlphaF( mFormat.opacity() );
  alphaRamp->setColor( alphaColor );
  QgsColorWidgetAction *alphaAction = new QgsColorWidgetAction( alphaRamp, mMenu, mMenu );
  alphaAction->setDismissOnColorSelection( false );
  connect( alphaAction, &QgsColorWidgetAction::colorChanged, this, [ = ]( const QColor & color )
  {
    double opacity = color.alphaF();
    mFormat.setOpacity( opacity );
    updatePreview();
    emit changed();
  } );
  connect( colorAction, &QgsColorWidgetAction::colorChanged, alphaRamp, [alphaRamp]( const QColor & color ) { alphaRamp->setColor( color, false ); }
         );
  mMenu->addAction( alphaAction );

  //get schemes with ShowInColorButtonMenu flag set
  QList< QgsColorScheme * > schemeList = QgsApplication::colorSchemeRegistry()->schemes( QgsColorScheme::ShowInColorButtonMenu );
  QList< QgsColorScheme * >::iterator it = schemeList.begin();
  for ( ; it != schemeList.end(); ++it )
  {
    QgsColorSwatchGridAction *colorAction = new QgsColorSwatchGridAction( *it, mMenu, QStringLiteral( "labeling" ), this );
    colorAction->setBaseColor( mFormat.color() );
    mMenu->addAction( colorAction );
    connect( colorAction, &QgsColorSwatchGridAction::colorChanged, this, &QgsFontButton::setColor );
    connect( colorAction, &QgsColorSwatchGridAction::colorChanged, this, &QgsFontButton::addRecentColor );
  }

  mMenu->addSeparator();

  QAction *copyColorAction = new QAction( tr( "Copy color" ), this );
  mMenu->addAction( copyColorAction );
  connect( copyColorAction, &QAction::triggered, this, &QgsFontButton::copyColor );

  QAction *pasteColorAction = new QAction( tr( "Paste color" ), this );
  //enable or disable paste action based on current clipboard contents. We always show the paste
  //action, even if it's disabled, to give hint to the user that pasting colors is possible
  QColor clipColor;
  bool hasAlpha = false;
  if ( colorFromMimeData( QApplication::clipboard()->mimeData(), clipColor, hasAlpha ) )
  {
    pasteColorAction->setIcon( createMenuIcon( clipColor ) );
  }
  else
  {
    pasteColorAction->setEnabled( false );
  }
  mMenu->addAction( pasteColorAction );
  connect( pasteColorAction, &QAction::triggered, this, &QgsFontButton::pasteColor );

}

void QgsFontButton::addRecentColor( const QColor &color )
{
  QgsRecentColorScheme::addRecentColor( color );
}

bool QgsFontButton::formatFromMimeData( const QMimeData *mimeData, QgsTextFormat &resultFormat ) const
{
  bool ok = false;
  resultFormat = QgsTextFormat::fromMimeData( mimeData, &ok );
  return ok;
}

void QgsFontButton::changeEvent( QEvent *e )
{
  if ( e->type() == QEvent::EnabledChange )
  {
    updatePreview();
  }
  QToolButton::changeEvent( e );
}

void QgsFontButton::showEvent( QShowEvent *e )
{
  updatePreview();
  QToolButton::showEvent( e );
}

void QgsFontButton::resizeEvent( QResizeEvent *event )
{
  QToolButton::resizeEvent( event );
  //recalculate icon size and redraw icon
  mIconSize = QSize();
  updatePreview();
}

void QgsFontButton::updatePreview( const QColor &color, QgsTextFormat *format )
{
  QgsTextFormat tempFormat;

  if ( format )
    tempFormat = *format;
  else
    tempFormat = mFormat;

  if ( color.isValid() )
    tempFormat.setColor( color );

  QSize currentIconSize;
  //icon size is button size with a small margin
  if ( menu() )
  {
    if ( !mIconSize.isValid() )
    {
      //calculate size of push button part of widget (ie, without the menu dropdown button part)
      QStyleOptionToolButton opt;
      initStyleOption( &opt );
      QRect buttonSize = QApplication::style()->subControlRect( QStyle::CC_ToolButton, &opt, QStyle::SC_ToolButton,
                         this );
      //make sure height of icon looks good under different platforms
#ifdef Q_OS_WIN
      mIconSize = QSize( buttonSize.width() - 10, height() - 6 );
#else
      mIconSize = QSize( buttonSize.width() - 10, height() - 12 );
#endif
    }
    currentIconSize = mIconSize;
  }
  else
  {
    //no menu
#ifdef Q_OS_WIN
    currentIconSize = QSize( width() - 10, height() - 6 );
#else
    currentIconSize = QSize( width() - 10, height() - 12 );
#endif
  }

  if ( !currentIconSize.isValid() || currentIconSize.width() <= 0 || currentIconSize.height() <= 0 )
  {
    return;
  }

  //create an icon pixmap
  QPixmap pixmap( currentIconSize );
  pixmap.fill( Qt::transparent );
  QPainter p;
  p.begin( &pixmap );
  p.setRenderHint( QPainter::Antialiasing );
  QRect rect( 0, 0, currentIconSize.width(), currentIconSize.height() );

  QgsRenderContext context;
  QgsMapToPixel newCoordXForm;
  newCoordXForm.setParameters( 1, 0, 0, 0, 0, 0 );
  context.setMapToPixel( newCoordXForm );

  context.setScaleFactor( QgsApplication::desktop()->logicalDpiX() / 25.4 );
  context.setUseAdvancedEffects( true );
  context.setPainter( &p );

  // slightly inset text to account for buffer/background
  double xtrans = 0;
  if ( tempFormat.buffer().enabled() )
    xtrans = context.convertToPainterUnits( tempFormat.buffer().size(), tempFormat.buffer().sizeUnit(), tempFormat.buffer().sizeMapUnitScale() );
  if ( tempFormat.background().enabled() && tempFormat.background().sizeType() != QgsTextBackgroundSettings::SizeFixed )
    xtrans = qMax( xtrans, context.convertToPainterUnits( tempFormat.background().size().width(), tempFormat.background().sizeUnit(), tempFormat.background().sizeMapUnitScale() ) );

  double ytrans = 0.0;
  if ( tempFormat.buffer().enabled() )
    ytrans = qMax( ytrans, context.convertToPainterUnits( tempFormat.buffer().size(), tempFormat.buffer().sizeUnit(), tempFormat.buffer().sizeMapUnitScale() ) );
  if ( tempFormat.background().enabled() )
    ytrans = qMax( ytrans, context.convertToPainterUnits( tempFormat.background().size().height(), tempFormat.background().sizeUnit(), tempFormat.background().sizeMapUnitScale() ) );

  QRectF textRect = rect;
  textRect.setLeft( xtrans );
  textRect.setWidth( textRect.width() - xtrans );
  textRect.setTop( ytrans );
  if ( textRect.height() > 300 )
    textRect.setHeight( 300 );
  if ( textRect.width() > 2000 )
    textRect.setWidth( 2000 );

  QgsTextRenderer::drawText( textRect, 0, QgsTextRenderer::AlignLeft, QStringList() << "Font",
                             context, tempFormat );

  p.end();
  setIconSize( currentIconSize );
  setIcon( pixmap );
}

void QgsFontButton::copyColor()
{
  //copy color
  QApplication::clipboard()->setMimeData( QgsSymbolLayerUtils::colorToMimeData( mFormat.color() ) );
}

void QgsFontButton::pasteColor()
{
  QColor clipColor;
  bool hasAlpha = false;
  if ( colorFromMimeData( QApplication::clipboard()->mimeData(), clipColor, hasAlpha ) )
  {
    //paste color
    setColor( clipColor );
    QgsRecentColorScheme::addRecentColor( clipColor );
  }
}

void QgsFontButton::setDialogTitle( const QString &title )
{
  mDialogTitle = title;
}

QString QgsFontButton::dialogTitle() const
{
  return mDialogTitle;
}

