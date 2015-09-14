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

#ifndef QGSWELCOMEDIALOG_H
#define QGSWELCOMEDIALOG_H

#include <QTabWidget>
#include <QWidget>
#include <QLabel>

#include "qgswelcomepageitemsmodel.h"

class QgsVersionInfo;


class APP_EXPORT QgsProjectExplorer : public QWidget
{
    Q_OBJECT

  public:
    QgsProjectExplorer( QWidget* parent = 0 );

    ~QgsProjectExplorer();

    void setRecentProjects( const QList<QgsWelcomePageItemsModel::RecentProjectData>& recentProjects );

  signals:

    void closePressed();

  private:

    QgsWelcomePageItemsModel* mModel;

  private slots:

    void itemActivated( const QModelIndex& index );

};


class APP_EXPORT QgsWelcomePage : public QTabWidget
{
    Q_OBJECT

  public:
    QgsWelcomePage( QWidget* parent = 0 );

    ~QgsWelcomePage();

    void setRecentProjects( const QList<QgsWelcomePageItemsModel::RecentProjectData>& recentProjects );

  private slots:

    void versionInfoReceived();
    void whatsNewLinkClicked( const QUrl& url );
    void closePressed();

  private:
    QLabel* mVersionInformation;
    QgsVersionInfo* mVersionInfo;
    QgsProjectExplorer* mProjectExplorer;
};



#endif // QGSWELCOMEDIALOG_H
