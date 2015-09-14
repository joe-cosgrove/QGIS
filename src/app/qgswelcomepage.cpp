/***************************************************************************

               ----------------------------------------------------
              date                 : 18.8.2015
              copyright            : (C) 2015 by Matthias Kuhn
              email                : matthias (at) opengis.ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgswelcomepage.h"
#include "qgsproject.h"
#include "qgisapp.h"
#include "qgsversioninfo.h"
#include "qgsapplication.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListView>
#include <QSettings>
#include <QDesktopServices>

//
// QgsProjectExplorer
//

QgsProjectExplorer::QgsProjectExplorer( QWidget *parent )
    : QWidget( parent )
{
  QVBoxLayout* mainLayout = new QVBoxLayout;

  setLayout( mainLayout );

  QHBoxLayout* hlayout = new QHBoxLayout();

  QLabel* recentProjectsTitle = new QLabel( QString( "<h1>%1</h1>" ).arg( tr( "Recent Projects" ) ) );
  hlayout->addWidget( recentProjectsTitle );

  QPushButton* closeButton = new QPushButton( tr( "Close" ) );
  hlayout->addWidget( closeButton );

  mainLayout->addLayout( hlayout );


  QListView* recentProjectsListView = new QListView();
  mModel = new QgsWelcomePageItemsModel( this );
  recentProjectsListView->setModel( mModel );
  recentProjectsListView->setItemDelegate( new QgsWelcomePageItemDelegate( recentProjectsListView ) );

  mainLayout->addWidget( recentProjectsListView );

  connect( recentProjectsListView, SIGNAL( activated( QModelIndex ) ), this, SLOT( itemActivated( QModelIndex ) ) );
  connect( closeButton, SIGNAL( clicked() ), this, SIGNAL( closePressed() ) );
}

QgsProjectExplorer::~QgsProjectExplorer()
{

}

void QgsProjectExplorer::setRecentProjects( const QList<QgsWelcomePageItemsModel::RecentProjectData> &recentProjects )
{
  mModel->setRecentProjects( recentProjects );
}

void QgsProjectExplorer::itemActivated( const QModelIndex &index )
{
  QgisApp::instance()->openProject( mModel->data( index, Qt::ToolTipRole ).toString() );


  QgisApp::instance()->fileNew();
}


//
// QgsWelcomePage
//

QgsWelcomePage::QgsWelcomePage( QWidget* parent )
    : QTabWidget( parent )
{
  QVBoxLayout* mainLayout = new QVBoxLayout;
  mainLayout->setMargin( 0 );
  setLayout( mainLayout );

  QHBoxLayout* layout = new QHBoxLayout();
  layout->setMargin( 9 );

  mainLayout->addLayout( layout );

  mProjectExplorer = new QgsProjectExplorer( this );
  addTab( mProjectExplorer, "Recent Projects" );

  QWidget* whatsNewContainer = new QWidget;
  whatsNewContainer->setLayout( new QVBoxLayout );
  QLabel* whatsNewTitle = new QLabel( QString( "<h1>%1</h1>" ).arg( tr( "QGIS News" ) ) );
  whatsNewContainer->layout()->addWidget( whatsNewTitle );

  QgsWebView* whatsNewPage = new QgsWebView();
  whatsNewPage->setUrl( QUrl::fromLocalFile( QgsApplication::whatsNewFilePath() ) );
  whatsNewPage->page()->setLinkDelegationPolicy( QWebPage::DelegateAllLinks );
  whatsNewPage->setContextMenuPolicy( Qt::NoContextMenu );
  whatsNewPage->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  whatsNewPage->setStyleSheet( "background:transparent" );
  whatsNewPage->setAttribute( Qt::WA_TranslucentBackground );

  whatsNewContainer->layout()->addWidget( whatsNewPage );
//  whatsNewContainer->setMaximumWidth( 250 );
//  whatsNewContainer->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
  addTab( whatsNewContainer, "News" );

  connect( whatsNewPage, SIGNAL( linkClicked( QUrl ) ), this, SLOT( whatsNewLinkClicked( QUrl ) ) );

  mVersionInformation = new QLabel;
  mainLayout->addWidget( mVersionInformation );
  mVersionInformation->setVisible( false );

  mVersionInfo = new QgsVersionInfo();
  connect( mVersionInfo, SIGNAL( versionInfoAvailable() ), this, SLOT( versionInfoReceived() ) );
  mVersionInfo->checkVersion();

  connect( mProjectExplorer, SIGNAL( closePressed() ), this, SLOT( closePressed() ) );
}

QgsWelcomePage::~QgsWelcomePage()
{
  delete mVersionInfo;
}

void QgsWelcomePage::setRecentProjects( const QList<QgsWelcomePageItemsModel::RecentProjectData>& recentProjects )
{
  mProjectExplorer->setRecentProjects( recentProjects );
}

void QgsWelcomePage::versionInfoReceived()
{
  QgsVersionInfo* versionInfo = qobject_cast<QgsVersionInfo*>( sender() );
  Q_ASSERT( versionInfo );

  if ( versionInfo->newVersionAvailable() )
  {
    mVersionInformation->setVisible( true );
    mVersionInformation->setText( QString( "<b>%1</b>: %2" )
                                  .arg( tr( "There is a new QGIS version available" ) )
                                  .arg( versionInfo->downloadInfo() ) );
    mVersionInformation->setStyleSheet( "QLabel{"
                                        "  background-color: #dddd00;"
                                        "  padding: 5px;"
                                        "}" );
  }
}

void QgsWelcomePage::whatsNewLinkClicked( const QUrl& url )
{
  QDesktopServices::openUrl( url );
}

void QgsWelcomePage::closePressed()
{
  QgisApp::instance()->fileNew();
}

