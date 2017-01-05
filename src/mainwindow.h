/***************************************************************************
 *   Copyright (C) 2010 by Amine Roukh           <amineroukh@gmail.com>    *
 *   Copyright (C) 2010 by Abdelkadir Sadouki    <sadouki_aek@hotmail.fr>  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "titleslisting.h"
#include "httphandler.h"
#include "data.h"
#include "corpus.h"
#include "findwidget.h"
#include "finddialog.h"
#include "deletedialog.h"
#include "settingsdialog.h"
#include "fetch.h"
#include "proxymodel.h"
#include "settings.h"

#include <QMainWindow>
#include <QDebug>
#include <QSplitter>
#include <QSortFilterProxyModel>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QTimer>
#include <QTextCodec>
#include <QClipboard>
#include <QMessageBox>
#include <QSettings>
#include <QHeaderView>
#include <QDateEdit>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0, Qt::WFlags fl = 0);
    ~MainWindow();

protected:
    void closeEvent( QCloseEvent *event );

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void setupViews();
    void setupSites();
    void setupTitles();
    void setupTimer();
    void setupConnections();
    void readSettings();
    void writeSettings();
    void initializeVar();
    void readSeeds();
    void checkEmptyModels();
    //void beforeReadArticles();
    void beforeFetch();
    void setSiteColor(QTreeWidgetItem *item, QColor color);
    void setAricleColor(QStandardItemModel *model, int row, QColor color, ArticleStatus Status);
    void setArticleActEnabled(bool enable);
    QTreeWidgetItem *getItemFromCat(int category);
    void enableActions(bool enabled);

public slots:
    void about();
    void sitesContextMenu(const QPoint &position);
    void titlesContextMenu(const QPoint &position);
    void itemActivated(QTreeWidgetItem * item);
    void activated(const QModelIndex & index);
    void openBrowser(const QModelIndex & index);
    void openBrowserSlot();
    void openHomepage();
    void openFile();
    void openLocation();
    void formatedArticleReceived(QString);
    void abortFetch();
    void seedReceived(QString category, QString catUrl, QString id, QString site, QString siteUrl, QString path);
    void itemReceived(QStandardItemModel *model);
    void updateSiteCount(int cat);
    void selectedArtChanged(const QItemSelection &);
    void afterFetch();
    void setStatusBarTextSlot(QString status, int timeout);
    void fetchLatest();
    void fetchAll();
    void fetchSelected();
    void fetchSite();
    void fetchCorrupt();
    void updateTime();
    void genStatistics();
    void searchHandler(QString);
    void dateFilterChanged();
    //void filterHandler(QString);
    void goToNextCat();
    void goToPrevCat();
    void goToNextArt();
    void goToPrevArt();
    void copyLink();
    void deleteArticle();
    void showFindWidget();
    void searchDialogSlot();
    void deleteDialogSlot();
    void settingsDialogSlot();

private:
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *downloadMenu;
    QMenu *goToMenu;
    QMenu *articleMenu;
    QMenu *helpMenu;
    QToolBar *toolBar;
    QAction *abortFetchAct;
    QAction *fetchLatestAct;
    QAction *fetchAllAct;
    QAction *fetchSelAct;
    QAction *fetchSiteAct;
    QAction *fetchCorAct;
    QAction *genStatiAct;
    //QAction *viewHtmlAct;
    QAction *goToNextCatAct;
    QAction *goToPrevCatAct;
    QAction *goToNextArtAct;
    QAction *goToPrevArtAct;
    QAction *openBrowserAct;
    QAction *openHomepageAct;
    QAction *openFileAct;
    QAction *openLocationAct;
    QAction *copyLinkAct;
    QAction *deleteArtAct;
    QAction *findAct;
    QAction *findInAct;
    QAction *deleteAct;
    QAction *settingsAct;
    QAction *exitAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    QLineEdit *searchEdit;
    QLabel *searchLabel;
    //QComboBox *filterBox;
    QDateEdit *fromDateEdit;
    QDateEdit *toDateEdit;
    QLabel *fromDateLabel;
    QLabel *toDateLabel;

    QSplitter *splitter;
    QSplitter *verticalSplitter;
    QTreeView *titlesView;
    QTreeWidget *sitesWidget;
    QWebView *articlesWebView;

    QTreeWidgetItem *parentItem;
    QTreeWidgetItem *currentItem;

    //QSortFilterProxyModel *proxyModel;
    QStandardItemModel *model;
    QList<QStandardItemModel*> modelList;

    QLabel* timeLabel;
    QTimer *timer;
    int sec, min, hou, currentTime;

    int currentCat;
    FetchNbr m_FetchNbr;

    //TitlesListing *titlesListing;
    HttpHandler *httpHandler;
    Data *data;
    Corpus *corpus;
    FindWidget *findWidget;
    FindDialog *findDialog;
    DeleteDialog *deleteDialog;
    SettingsDialog *settingsDialog;
    Fetch *fetch;
    ProxyModel *proxyModel;
    Settings settings;
};

#endif // MAINWINDOW_H

