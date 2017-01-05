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


#include "mainwindow.h"
#include "constants.h"

MainWindow::MainWindow( QWidget* parent, Qt::WFlags fl )
        : QMainWindow( parent, fl )
{
    settings.loadSettings();

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    setupViews();
    setupSites();
    setupTitles();
    setupConnections();

    initializeVar();
    data->readSeeds();
    data->loadData();
    checkEmptyModels();

    //qDebug() << data->dataMaximumDate() << data->dataMinimumDate();
    fromDateEdit->setDate(data->dataMinimumDate());

    readSettings();

    //settingsDialogSlot();
    //searchDialogSlot();
    //deleteDialogSlot();
    //qDebug() << "List size:" << modelList.size();
}

MainWindow::~MainWindow()
{
}

void MainWindow::createActions()
{
    abortFetchAct = new QAction(QIcon( ":/resources/process-stop.png" ), tr("&Annuler"), this);
    abortFetchAct->setShortcut(tr("Ctrl+A"));
    abortFetchAct->setStatusTip(tr("Annuler le téléchargement"));
    abortFetchAct->setEnabled(false);
    connect(abortFetchAct, SIGNAL(triggered()), this, SLOT(abortFetch()));

    fetchLatestAct = new QAction(QIcon( ":/resources/go-down.png" ), tr("Télécharger les &derniers"), this);
    fetchLatestAct->setShortcut(tr("Ctrl+D"));
    fetchLatestAct->setStatusTip(tr("Télécharger les derniers articles"));
    fetchLatestAct->setEnabled(true);
    connect(fetchLatestAct, SIGNAL(triggered()), this, SLOT(fetchLatest()));

    fetchAllAct = new QAction(QIcon( ":/resources/go-bottom.png" ), tr("Télécharger &tous"), this);
    fetchAllAct->setShortcut(tr("Ctrl+T"));
    fetchAllAct->setStatusTip(tr("Télécharger tous les articles"));
    fetchAllAct->setEnabled(true);
    connect(fetchAllAct, SIGNAL(triggered()), this, SLOT(fetchAll()));

    fetchSelAct = new QAction(QIcon(":/resources/arrow-down.png"), tr("Télécharger le sélectionné"), this);
    fetchSelAct->setShortcut(tr("Ctrl+S"));
    fetchSelAct->setStatusTip(tr("Télécharger les articles de la catégorie sélectionnée"));
    fetchSelAct->setEnabled(false);
    connect(fetchSelAct, SIGNAL(triggered()), this, SLOT(fetchSelected()));

    fetchSiteAct = new QAction(QIcon(":/resources/arrow-down.png"), tr("Télécharger le site"), this);
    fetchSiteAct->setShortcut(tr("Ctrl+E"));
    fetchSiteAct->setStatusTip(tr("Télécharger les articles du site sélectionné"));
    connect(fetchSiteAct, SIGNAL(triggered()), this, SLOT(fetchSite()));

    fetchCorAct = new QAction(QIcon(":/resources/arrow-down-double.png"), tr("Télécharger les corrompus"), this);
    fetchCorAct->setShortcut(tr("Ctrl+C"));
    fetchCorAct->setStatusTip(tr("Télécharger les articles corrompus"));
    connect(fetchCorAct, SIGNAL(triggered()), this, SLOT(fetchCorrupt()));

    genStatiAct = new QAction(QIcon(":/resources/view-statistics.png"), tr("Générer les statistiques"), this);
    genStatiAct->setShortcut(tr("Ctrl+G"));
    genStatiAct->setStatusTip(tr("Générer les statistiques de tout les catégories dans un fichier statistics.txt"));
    connect(genStatiAct, SIGNAL(triggered()), this, SLOT(genStatistics()));

    /*viewHtmlAct = new QAction(QIcon(":/resources/text-html.png"), tr("Afficher en HTML"), this);
    viewHtmlAct->setShortcut(tr("Ctrl+X"));
    viewHtmlAct->setStatusTip(tr("Afficher les articles en HTML s'il ne sont pas encore télécharger"));
    viewHtmlAct->setCheckable(true);*/

    goToNextArtAct = new QAction(QIcon(":/resources/go-next.png"), tr("Article suivant"), this);
    goToNextArtAct->setShortcut(tr("Left"));
    connect(goToNextArtAct, SIGNAL(triggered()), this, SLOT(goToNextArt()));

    goToPrevArtAct = new QAction(QIcon(":/resources/go-previous.png"), tr("Article précédent"), this);
    goToPrevArtAct->setShortcut(tr("Right"));
    connect(goToPrevArtAct, SIGNAL(triggered()), this, SLOT(goToPrevArt()));

    goToNextCatAct = new QAction(QIcon(":/resources/go-down.png"), tr("Site suivant"), this);
    goToNextCatAct->setShortcut(tr("N"));
    connect(goToNextCatAct, SIGNAL(triggered()), this, SLOT(goToNextCat()));

    goToPrevCatAct = new QAction(QIcon(":/resources/go-up.png"), tr("Site précédent"), this);
    goToPrevCatAct->setShortcut(tr("P"));
    connect(goToPrevCatAct, SIGNAL(triggered()), this, SLOT(goToPrevCat()));

    openHomepageAct = new QAction(QIcon(":/resources/window-new.png"), tr("&Ouvrir la page d'accueil"), this);
    openHomepageAct->setShortcut(tr("Ctrl+H"));
    connect(openHomepageAct, SIGNAL(triggered()), this, SLOT(openHomepage()));

    openBrowserAct = new QAction(QIcon(":/resources/window-new.png"), tr("Ouvrir dans le navigateur externe"), this);
    openBrowserAct->setEnabled(false);
    connect(openBrowserAct, SIGNAL(triggered()), this, SLOT(openBrowserSlot()));

    copyLinkAct = new QAction(QIcon(":/resources/edit-copy.png"), tr("Copier l'adresse du lien"), this);
    copyLinkAct->setEnabled(false);
    connect(copyLinkAct, SIGNAL(triggered()), this, SLOT(copyLink()));

    openFileAct = new QAction(QIcon(":/resources/document-open.png"), tr("Ouvrir le fichier"), this);
    openFileAct->setEnabled(false);
    openFileAct->setShortcut(tr("Ctrl+O"));
    connect(openFileAct, SIGNAL(triggered()), this, SLOT(openFile()));

    openLocationAct = new QAction(QIcon(":/resources/document-open-folder.png"), tr("Ouvrir l'emplacement du fichier"), this);
    openLocationAct->setEnabled(false);
    connect(openLocationAct, SIGNAL(triggered()), this, SLOT(openLocation()));

    deleteArtAct = new QAction(QIcon(":/resources/edit-delete.png"), tr("Supprimer"), this);
    deleteArtAct->setShortcut(tr("Delete"));
    deleteArtAct->setEnabled(false);
    connect(deleteArtAct, SIGNAL(triggered()), this, SLOT(deleteArticle()));

    findAct = new QAction(QIcon(":/resources/edit-find.png"), tr("Rechercher"), this);
    findAct->setShortcut(QKeySequence::Find);
    findAct->setStatusTip(tr("Recherche dans l'article actuel"));
    connect(findAct, SIGNAL(triggered()), this, SLOT(showFindWidget()));

    findInAct = new QAction(QIcon(":/resources/edit-find-project.png"), tr("Rechercher dans..."), this);
    findInAct->setShortcut(tr("Ctrl+Shift+F"));
    findInAct->setStatusTip(tr("Recherche dans tous le corpus"));
    connect(findInAct, SIGNAL(triggered()), this, SLOT(searchDialogSlot()));

    deleteAct = new QAction(QIcon(":/resources/edit-delete.png"), tr("Purger..."), this);
    deleteAct->setShortcut(tr("Alt+Del"));
    deleteAct->setStatusTip(tr("Purger le corpus"));
    connect(deleteAct, SIGNAL(triggered()), this, SLOT(deleteDialogSlot()));

    settingsAct = new QAction(QIcon(":/resources/configure.png"), tr("Configuration"), this);
    //settingsAct->setShortcut(tr(""));
    settingsAct->setStatusTip(tr("Configurer Stories Crawler"));
    connect(settingsAct, SIGNAL(triggered()), this, SLOT(settingsDialogSlot()));

    exitAct = new QAction(QIcon(":/resources/application-exit.png"), tr("&Quitter"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Quitter Stories Crawler"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutQtAct = new QAction(QIcon(":/resources/qt.png"), tr("À propos &Qt"), this);
    aboutQtAct->setStatusTip(tr("À propos de Qt"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    aboutAct = new QAction(QIcon(":/resources/storiescrawler.png"), tr("&À propos"), this);
    aboutAct->setStatusTip(tr("À propos Stories Crawler"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Fichier"));
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("Éditio&n"));
    //editMenu->addAction(viewHtmlAct);
    editMenu->addAction(genStatiAct);
    editMenu->addAction(deleteAct);
    editMenu->addSeparator();
    editMenu->addAction(findAct);
    editMenu->addAction(findInAct);    
    editMenu->addSeparator();
    editMenu->addAction(settingsAct);

    downloadMenu = menuBar()->addMenu(tr("Téléchar&ger"));
    downloadMenu->addAction(fetchSelAct);
    downloadMenu->addAction(fetchLatestAct);
    downloadMenu->addAction(fetchAllAct);
    downloadMenu->addAction(fetchCorAct);
    downloadMenu->addAction(abortFetchAct);

    goToMenu = menuBar()->addMenu(tr("A&ller"));
    goToMenu->addAction(goToPrevArtAct);
    goToMenu->addAction(goToNextArtAct);
    goToMenu->addSeparator();
    goToMenu->addAction(goToPrevCatAct);
    goToMenu->addAction(goToNextCatAct);

    articleMenu = menuBar()->addMenu(tr("Arti&cle"));
    articleMenu->addAction(openBrowserAct);
    articleMenu->addAction(copyLinkAct);
    articleMenu->addSeparator();
    articleMenu->addAction(openFileAct);
    articleMenu->addAction(openLocationAct);
    articleMenu->addAction(deleteArtAct);

    helpMenu = menuBar()->addMenu(tr("Aid&e"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
    toolBar = addToolBar(tr("Édition"));
    toolBar->setMovable(false);
    toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolBar->addAction(fetchSelAct);
    toolBar->addAction(fetchLatestAct);
    toolBar->addAction(fetchAllAct);
    toolBar->addAction(abortFetchAct);
}

void MainWindow::createStatusBar()
{
    setStatusBarTextSlot(tr("Prêt"), 2000);
    timeLabel = new QLabel(tr("Temps: 00:00:00"));

    statusBar()->addPermanentWidget(timeLabel);
}

void MainWindow::setupViews()
{
    splitter = new QSplitter();
    verticalSplitter = new QSplitter();
    verticalSplitter->setOrientation(Qt::Vertical);

    sitesWidget = new QTreeWidget();
    titlesView = new QTreeView();
    articlesWebView = new QWebView();
    articlesWebView->page()->setLinkDelegationPolicy(QWebPage::DontDelegateLinks);
    if(settings.enablePlugins)
        articlesWebView->page()->settings()->setAttribute(QWebSettings::PluginsEnabled, true);

    searchLabel = new QLabel(tr("Chercher:"));
    searchEdit = new QLineEdit();

    fromDateEdit = new QDateEdit;
    fromDateEdit->setDisplayFormat("yyyy/MM/dd");
    fromDateEdit->setMaximumDate(QDate::currentDate());
    fromDateLabel = new QLabel(tr("Afficher de:"));
    fromDateLabel->setBuddy(fromDateEdit);

    toDateEdit = new QDateEdit;
    toDateEdit->setDate(QDate::currentDate());
    toDateEdit->setDisplayFormat("yyyy/MM/dd");
    toDateEdit->setMaximumDate(QDate::currentDate());
    toDateLabel = new QLabel(tr("à:"));
    toDateLabel->setBuddy(toDateEdit);

    findWidget = new FindWidget(this);
    findWidget->hide();

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(searchLabel);
    hLayout->addWidget(searchEdit);
    hLayout->addWidget(fromDateLabel);
    hLayout->addWidget(fromDateEdit);
    hLayout->addWidget(toDateLabel);
    hLayout->addWidget(toDateEdit);

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addLayout(hLayout);
    vLayout->addWidget(titlesView);

    QWidget *widget = new QWidget;
    widget->setLayout(vLayout);

    QVBoxLayout *vLayout2 = new QVBoxLayout();
    vLayout2->addWidget(articlesWebView);
    vLayout2->addWidget(findWidget);

    QWidget *widget2 = new QWidget;
    widget2->setLayout(vLayout2);

    verticalSplitter->addWidget(widget);
    verticalSplitter->addWidget(widget2);
    splitter->addWidget(sitesWidget);
    splitter->addWidget(verticalSplitter);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    setCentralWidget(splitter);

    if(settings.rtl)
        qApp->setLayoutDirection(Qt::RightToLeft);

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    qApp->setApplicationName(tr("storiescrawler"));
    qApp->setApplicationVersion(tr("0.1"));
    setWindowIcon(QIcon(":/resources/storiescrawler.png"));
    setWindowTitle(tr("Stories Crawler"));
}

void MainWindow::setupSites()
{
    QStringList labels;
    labels << tr("Sites") << tr("Url") << tr("Id") << tr("Total") << tr("Path");
    sitesWidget->setHeaderLabels(labels);
    sitesWidget->setContextMenuPolicy(Qt::CustomContextMenu);
}

void MainWindow::setupTitles()
{
    model = new QStandardItemModel(0, 5, this);
    model->setHeaderData(TITLES_NAME, Qt::Horizontal, QObject::tr("Titre"));
    model->setHeaderData(TITLES_DATE, Qt::Horizontal, QObject::tr("Date"));
    model->setHeaderData(TITLES_URL, Qt::Horizontal, QObject::tr("Link"));
    model->setHeaderData(TITLES_ID, Qt::Horizontal, QObject::tr("Id"));
    model->setHeaderData(TITLES_STATUS, Qt::Horizontal, QObject::tr("Status"));

    proxyModel = new ProxyModel;
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSourceModel(model);

    titlesView->setModel(proxyModel);
    titlesView->setRootIsDecorated(false);
    titlesView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    titlesView->setSortingEnabled(true);
    titlesView->setWordWrap(false);
    titlesView->setSelectionBehavior(QAbstractItemView::SelectRows);
    titlesView->setAlternatingRowColors(true);
    titlesView->setContextMenuPolicy(Qt::CustomContextMenu);

    titlesView->sortByColumn(TITLES_DATE, Qt::DescendingOrder);
    model->sort(TITLES_DATE, Qt::DescendingOrder);
}

void MainWindow::setupConnections()
{
    //titlesListing = new TitlesListing;
    data = new Data;
    httpHandler = new HttpHandler;
    corpus = new Corpus;
    fetch = new Fetch;
    //Settings settings;

    connect(data, SIGNAL(newSeed(QString, QString, QString, QString, QString, QString)),
            this, SLOT(seedReceived(QString, QString, QString, QString, QString, QString)));

    connect(data, SIGNAL(newItem(QStandardItemModel*)),
            this, SLOT(itemReceived(QStandardItemModel*)));

    connect(sitesWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
            this, SLOT(itemActivated(QTreeWidgetItem *)));

    connect(sitesWidget, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(sitesContextMenu(const QPoint &)));

    connect(titlesView, SIGNAL(clicked(const QModelIndex &)),
            this, SLOT(activated(const QModelIndex &)));

    connect(titlesView, SIGNAL(doubleClicked(const QModelIndex &)),
            this, SLOT(openBrowser(const QModelIndex &)));

    //connect exactly once
    //disconnect(titlesView, SIGNAL(customContextMenuRequested(const QPoint &)),
            //this, SLOT(titlesContextMenu(const QPoint &)));

    connect(titlesView, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(titlesContextMenu(const QPoint &)));

    connect(titlesView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(selectedArtChanged(const QItemSelection)));

    connect(httpHandler, SIGNAL(newFormatedArticle(QString)),
            this, SLOT(formatedArticleReceived(QString)));

    connect(fetch, SIGNAL(updateWidget()), this, SLOT(afterFetch()));

    connect(fetch, SIGNAL(setStatusBarText(QString, int)),
            this , SLOT(setStatusBarTextSlot(QString,int)));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));

    connect(searchEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(searchHandler(QString)));

    connect(fromDateEdit, SIGNAL(dateChanged(QDate)), this, SLOT(dateFilterChanged()));

    connect(toDateEdit, SIGNAL(dateChanged(QDate)), this, SLOT(dateFilterChanged()));
}

void MainWindow::sitesContextMenu(const QPoint &position)
{
    if (!sitesWidget->indexAt(position).isValid())
        return;

    itemActivated(sitesWidget->itemAt(position));

    QMenu menu;

    if(sitesWidget->itemAt(position)->data(SITES_ID, 0).toString().isEmpty()) // Parent
    {
        menu.addAction(fetchSiteAct);
        menu.addSeparator();
        menu.addAction(openHomepageAct);
    }
    else
        menu.addAction(fetchSelAct);

    menu.exec(sitesWidget->mapToGlobal(position));

}

void MainWindow::titlesContextMenu(const QPoint &position)
{
    if (!titlesView->indexAt(position).isValid())
        return;

    //activated(titlesView->indexAt(position));

    QMenu menu;
    menu.addAction(openBrowserAct);
    menu.addAction(copyLinkAct);
    menu.addSeparator();
    menu.addAction(openFileAct);
    menu.addAction(openLocationAct);
    menu.addAction(deleteArtAct);
    menu.exec(titlesView->mapToGlobal(position));
}

void MainWindow::initializeVar()
{
    currentCat = -1;
    sec = min = hou = currentTime = 0;
}

void MainWindow::seedReceived(QString category, QString catUrl, QString id, QString site, QString siteUrl, QString path)
{
    if (!site.isEmpty()) // Parent
    {
        QString iconName = ":/resources/" + siteUrl.section(".", 1, 1) + ".png";
        parentItem = new QTreeWidgetItem(sitesWidget);
        parentItem->setIcon(SITES_NAME, QIcon(iconName));
        parentItem->setText(SITES_NAME, site);
        parentItem->setText(SITES_URL, siteUrl);
        parentItem->setExpanded(true);
    }

    QTreeWidgetItem *childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(SITES_NAME, category);
    childItem->setText(SITES_URL, catUrl);
    childItem->setText(SITES_ID, id);
    childItem->setText(SITES_PATH, path);
    parentItem->insertChild(0, childItem);
}

void MainWindow::itemReceived(QStandardItemModel *model)
{
    model->sort(TITLES_DATE, Qt::DescendingOrder);
    modelList.append(model);

    currentCat++;
    updateSiteCount(currentCat);
}

void MainWindow::updateSiteCount(int cat)
{
    // Calculate articles number in every category
    QTreeWidgetItemIterator it(sitesWidget, QTreeWidgetItemIterator::NoChildren);
    it += cat;
    if(*it)
    {
        int count = modelList.at(cat)->rowCount();
        (*it)->setData(SITES_TOTAL, Qt::DisplayRole, count);
    }
}

void MainWindow::checkEmptyModels()
{
    //qDebug() << "check empty models";

    QTreeWidgetItemIterator it(sitesWidget, QTreeWidgetItemIterator::NoChildren);
    while (*it)
    {
        if((*it)->text(SITES_TOTAL).isEmpty())
        {
            model = new QStandardItemModel(0, 5, this);
            model->setHeaderData(TITLES_NAME, Qt::Horizontal, QObject::tr("Titre"));
            model->setHeaderData(TITLES_DATE, Qt::Horizontal, QObject::tr("Date"));
            model->setHeaderData(TITLES_URL, Qt::Horizontal, QObject::tr("Link"));
            model->setHeaderData(TITLES_ID, Qt::Horizontal, QObject::tr("Id"));
            model->setHeaderData(TITLES_STATUS, Qt::Horizontal, QObject::tr("Status"));

            modelList.append(model);
            (*it)->setData(SITES_TOTAL, Qt::DisplayRole, 0);

            // Mark the change of data status
            fetch->setDataChanged(true);
        }

        ++it;
    }
}

void MainWindow::itemActivated(QTreeWidgetItem * item)
{    
    int currentId = item->text(SITES_ID).toInt();

    if (item->text(SITES_ID).isEmpty()) // Parent
    {

       /* QTreeWidgetItem *next = sitesWidget->itemBelow(item);
        int nextId = next->text(2).toInt();
        int lastId = item->childCount() + nextId;

        qDebug() << nextId << lastId;

        //qDebug() << item->children;
        QStandardItemModel *models = new QStandardItemModel();

        for(int i=nextId; i<=lastId; ++i)
        {
            for(int j=0; j<modelList.at(i)->rowCount(); ++j)
            {
                //for(int k=0; k<modelList.at(i)->columnCount(); ++k)
                QStandardItemModel *tmpModel = modelList.at(i);
                models->appendRow(tmpModel->takeRow(j));
            }
            //models->insertRows(0, modelList.at(i)->rowCount());

        }
        qDebug() << models->rowCount();
        proxyModel->setSourceModel(models);
        titlesView->setModel(proxyModel);*/

        fetchSelAct->setEnabled(false);
        //return;
    }

    else if (modelList.size() > currentId)
    {
        proxyModel->setSourceModel(modelList.at(currentId));
        titlesView->setModel(proxyModel);

        //if(end && !fetch->isReadingArticles()) // TEST
        if(!fetch->isReadingTitles() && !fetch->isReadingArticles())
            fetchSelAct->setEnabled(true);

        // Disable article actions when changing model
        setArticleActEnabled(false);
    }
    else
        titlesView->setModel(model); // Show empty model
}

void MainWindow::fetchSelected()
{
    beforeFetch();
    fetch->fetchSelected();
}

void MainWindow::fetchLatest()
{
    beforeFetch();
    fetch->fetchLatest();
}

void MainWindow::fetchAll()
{
    beforeFetch();
    fetch->fetchAll();
}

void MainWindow::fetchSite()
{
    beforeFetch();
    fetch->fetchSite();
}

void MainWindow::fetchCorrupt()
{
    beforeFetch();
    fetch->fetchCorrupt();
}

void MainWindow::beforeFetch()
{
    fetch->setSitesWidget(sitesWidget);
    fetch->setModelList(modelList);

    enableActions(false);

    timer->stop();
    timer->start(1000);
    timeLabel->setText(tr("Temps: 00:00:00"));
}

void MainWindow::afterFetch()
{
    timer->stop();

    enableActions(true);

    initializeVar();

    if(settings.exitAfterFetch && !fetch->isReadingCorrupt())
        QApplication::exit();
}

void MainWindow::enableActions(bool enabled)
{
    abortFetchAct->setEnabled(!enabled);
    fetchLatestAct->setEnabled(enabled);
    fetchAllAct->setEnabled(enabled);
    fetchSelAct->setEnabled(enabled);
    fetchSiteAct->setEnabled(enabled);
    fetchCorAct->setEnabled(enabled);
    deleteAct->setEnabled(enabled);
    titlesView->setSortingEnabled(enabled);
}

void MainWindow::setStatusBarTextSlot(QString status, int timeout = 0)
{
    statusBar()->showMessage(status, timeout);
}

// Read formated Article from local/net source
void MainWindow::activated(const QModelIndex & index)
{
    QString path = sitesWidget->currentItem()->text(SITES_PATH);
    QString id = proxyModel->index(index.row(), TITLES_ID).data().toString();
    QString article = data->readArticle(id, path);
    QString url = proxyModel->index(index.row(), TITLES_URL).data().toString();

    // Read Article from local source when it is present
    if(article.size() < 50 || settings.viewInHTML)
    {
        // If we are fetching or viewHtmlAct is not checked view empty text
        if(!fetch->isReadingArticles())
        {
            //if(viewHtmlAct->isChecked())
            //if(settings.viewInHTML)
            //{
                if(QUrl(url).isValid())
                {
                    httpHandler->abort();
                    httpHandler->get(QString(url), true);
                    setStatusBarTextSlot(tr("Lecture d'article"));
                }
                //return;
            //}
        }

        //articlesWebView->setHtml("");
    }

    else
    {
        QString lang;

        if(url.contains(REUTERS_EN) || url.contains(AFP_EN))
            lang = "lang=\"en\" xml:lang=\"en\"";
        else if(url.contains(REUTERS_FR) || url.contains(AFP_FR))
            lang = "lang=\"fr\" xml:lang=\"fr\"";
        else
            lang = "lang=\"ar\" xml:lang=\"ar\" dir=\"rtl\">";

        //qDebug() << lang << url;


        QString html = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\"" + lang +
        "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" /><style type=\"text/css\">body{" +
        "font-style:" + settings.fontStyle + ";}</style>" +
        article + "</body></html>";

        //qDebug() << html;

        /*QString html = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\"" + lang +
        "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />" +
        article + "</body></html>";*/

        //qDebug() << articlesWebView->page()->settings()->fontFamily(QWebSettings::StandardFont);
        //qDebug() << articlesWebView->page()->settings()->fontSize(QWebSettings::DefaultFontSize);

        articlesWebView->page()->settings()->setFontFamily(QWebSettings::StandardFont, settings.fontFamily);
        articlesWebView->page()->settings()->setFontSize(QWebSettings::DefaultFontSize, settings.fontSize.toInt());

        articlesWebView->setHtml(html);
    }
}

// Open default web browser for titles/sites
void MainWindow::openBrowser(const QModelIndex & index)
{
    QString url;

    if(sitesWidget->hasFocus()) // To find signal source
        url = sitesWidget->currentItem()->text(SITES_URL);
    else
        url = proxyModel->index(index.row(), TITLES_URL).data().toString();

    QDesktopServices::openUrl(QString(url));
}

void MainWindow::formatedArticleReceived(QString article)
{
    QModelIndex index = titlesView->currentIndex();
    QString url = proxyModel->index(index.row(), TITLES_URL).data().toString();
    articlesWebView->setHtml(article, url);
    setStatusBarTextSlot(tr("Terminer"), 2000);
}

void MainWindow::abortFetch()
{
    fetch->abortFetch();
    afterFetch();
}

void MainWindow::updateTime()
{
    currentTime++;
    if(settings.maxFetchTime != 0 && settings.maxFetchTime < currentTime)
    {
        abortFetch();
        return;
    }

    sec++;
    if (sec > 59)
    {
        min++;
        if (min > 59)
        {
            hou++;
            min=0;
        }
        sec=0;
    }

    QString s = QString::number(sec).rightJustified(2, '0', true);
    QString m = QString::number(min).rightJustified(2, '0', true);
    QString h = QString::number(hou).rightJustified(2, '0', true);

    timeLabel->setText(tr("Temps: ") + h + ":" + m + ":" + s);
}

void MainWindow::setSiteColor(QTreeWidgetItem *item, QColor color)
{
    if(item)
    {
        QBrush brush(color);
        item->setForeground( SITES_NAME, brush );
        item->setForeground( SITES_TOTAL, brush );
    }
    else
        qDebug() << "item out of range in setSiteColor()";
}

void MainWindow::setAricleColor(QStandardItemModel *model, int row, QColor color, ArticleStatus Status)
{
    QBrush brush(color);
    model->item(row, TITLES_NAME)->setForeground(brush);
    model->item(row, TITLES_DATE)->setForeground(brush);
    model->setData(model->index(row, TITLES_STATUS), Status);
}

QTreeWidgetItem *MainWindow::getItemFromCat(int category)
{
    QTreeWidgetItemIterator it(sitesWidget, QTreeWidgetItemIterator::NoChildren);
    it += category;
    return (*it);
}

void MainWindow::genStatistics()
{
    if (modelList.isEmpty())
        return;

    QString stati, str1, str2;
    int articlesHandled = 0;
    int charsNbr = 0;
    int charsNbrPerCat = 0;
    int wordsNbr = 0;
    int wordsNbrPerCat = 0;
    int totalWords = 0;
    int totalArticles = 0;

    QTreeWidgetItem *item = new QTreeWidgetItem;
    QTreeWidgetItemIterator it(sitesWidget, QTreeWidgetItemIterator::NoChildren);

    // If there are still category to handler
    while ((*it))
    {
        item = (*it);

        setSiteColor(item, Qt::red);

        //int currentId = item->text(SITES_ID).toInt();
        currentCat = item->text(SITES_ID).toInt();

        // If articles titles are available in the current category
        if (modelList.size() > currentCat)
        {
            setStatusBarTextSlot(tr("Traitement du %1 catégorie").arg(currentCat+1));

            QString path = item->text(SITES_PATH);

            // Handler articles row by row
            for (int i=0; i < modelList.at(currentCat)->rowCount(); i++)
            {
                QString id = modelList.at(currentCat)->index(i, TITLES_ID).data().toString();
                QString article = data->readArticle(id, path);

                if(!article.isEmpty())
                {
                    corpus->extractWords(article);
                    wordsNbr = corpus->getWordsCount();
                    charsNbr = corpus->getCharsCount();

                    wordsNbrPerCat += wordsNbr;
                    charsNbrPerCat += charsNbr;
                    articlesHandled++;

                    stati += str1.setNum(wordsNbr) + " words and " + str2.setNum(charsNbr)
                             + " chars in article id " + id + "\n";
                }

                QApplication::processEvents();
            }

            if(articlesHandled > 0)
            {
                stati += str1.setNum(wordsNbrPerCat) + " words and " + str2.setNum(charsNbrPerCat)
                         + " chars in category ";
                stati += str1.setNum(currentCat+1) + " with " + str2.setNum(articlesHandled) + " articles\n";
                stati += "moy words :" + str1.setNum(wordsNbrPerCat/articlesHandled) + " moy chars :"
                         + str2.setNum(charsNbrPerCat/articlesHandled) + "\n\n";
                //qDebug() << stati;
            }

            totalWords += wordsNbrPerCat;
            totalArticles += articlesHandled;

            wordsNbrPerCat = 0;
            charsNbrPerCat = 0;
            articlesHandled = 0;
        }

        setSiteColor(item, QApplication::palette().color(QPalette::Text));

        // Go to next category
        ++it;
    }

    if(totalWords > 0)
    {
        stati += "total words :" + str1.setNum(totalWords);
        stati += "\ntotal articles :" + str1.setNum(totalArticles) + '\n';
    }

    // Save statistics in "statistics.txt" file
    data->saveStatistics(stati);

    QString statiFile = QDir::currentPath()+"/statistics.txt";

    if(QFile(statiFile).exists())
        QDesktopServices::openUrl(QUrl::fromLocalFile(statiFile));

    setStatusBarTextSlot(tr("Terminer"), 2000);
}

void MainWindow::goToNextCat()
{
    sitesWidget->setFocus();
    QModelIndex current = sitesWidget->currentIndex();
    QModelIndex next = sitesWidget->indexBelow(current);

    if(next.isValid())
        sitesWidget->setCurrentIndex(next);

    else
        sitesWidget->setCurrentItem(sitesWidget->topLevelItem(0));

    itemActivated(sitesWidget->currentItem());
}

void MainWindow::goToPrevCat()
{
    sitesWidget->setFocus();
    QModelIndex current = sitesWidget->currentIndex();
    QModelIndex prev = sitesWidget->indexAbove(current);

    if(prev.isValid())
        sitesWidget->setCurrentIndex(prev);

    else
    {
        int index = sitesWidget->topLevelItemCount()-1;
        QTreeWidgetItem *item = sitesWidget->topLevelItem(index);
        sitesWidget->setCurrentItem(item->child(item->childCount()-1));
    }

    itemActivated(sitesWidget->currentItem());
}

void MainWindow::goToNextArt()
{
    titlesView->setFocus();
    QModelIndex current = titlesView->currentIndex();
    QModelIndex next = current.sibling(current.row()+1, current.column());

    if(next.isValid())
    {
        titlesView->setCurrentIndex(next);
        activated(next);
    }
}

void MainWindow::goToPrevArt()
{
    titlesView->setFocus();
    QModelIndex current = titlesView->currentIndex();
    QModelIndex prev = current.sibling(current.row()-1, current.column());

    if(prev.row() < 0)
    {
        titlesView->setCurrentIndex(current);
        //activated(current);
        return;
    }

    if(prev.isValid())
    {
        titlesView->setCurrentIndex(prev);
        activated(prev);
    }
}

void MainWindow::searchHandler(QString text)
{
    QRegExp regExp(text, Qt::CaseInsensitive, QRegExp::PatternSyntax(2));

    proxyModel->setFilterRegExp(regExp);
}

void MainWindow::dateFilterChanged()
{
    proxyModel->setFilterMaximumDate(toDateEdit->date());
    proxyModel->setFilterMinimumDate(fromDateEdit->date());
}

void MainWindow::openBrowserSlot()
{
    QModelIndex index = titlesView->currentIndex();
    openBrowser(index);
}

void MainWindow::openHomepage()
{
    QModelIndex index = sitesWidget->currentIndex();
    openBrowser(index);
}

void MainWindow::copyLink()
{
    QModelIndex index = titlesView->currentIndex();
    QString url = proxyModel->index(index.row(), TITLES_URL).data().toString();
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText( url, QClipboard::Clipboard );
}

void MainWindow::openFile()
{
    QModelIndex index = titlesView->currentIndex();
    QString path = sitesWidget->currentItem()->text(SITES_PATH);
    QString id = proxyModel->index(index.row(), TITLES_ID).data().toString();
    QString fullPath = QDir::currentPath() + "/data/" + path + "/" + id + ".txt";
    qDebug() << fullPath;

    if(QFile(fullPath).exists())
        QDesktopServices::openUrl(QUrl::fromLocalFile(fullPath));

    else
        QMessageBox::critical(this, tr("Fichier introuvable"),
                           tr("Le fichier <br /><b>%1</b><br /> n'existe pas.").arg(fullPath));
}

void MainWindow::openLocation()
{
    QString path = sitesWidget->currentItem()->text(SITES_PATH);
    QString fullPath = QDir::currentPath() + "/data/" + path;
    qDebug() << fullPath;

    if(QFile(fullPath).exists())
        QDesktopServices::openUrl(QUrl::fromLocalFile(fullPath));

    else
        QMessageBox::critical(this, tr("Dossier introuvable"),
                           tr("Le dossier <br /><b>%1</b><br /> n'existe pas.").arg(fullPath));
}

void MainWindow::deleteArticle()
{
    int ret = QMessageBox::warning(this, tr("Supprimer"), tr("Voulez-vous vraiment supprimer l'article<br /><b>%1"
                                                             //"sélectionné et effacer son contenu ?"),
                                                             "</b> ?").arg(proxyModel->index(titlesView->currentIndex().row(),
                                                                                         TITLES_NAME).data().toString()),
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::No);

    if (ret == QMessageBox::No)
        return;

    QModelIndex index = titlesView->currentIndex();
    QString path = sitesWidget->currentItem()->text(SITES_PATH);
    QString id = proxyModel->index(index.row(), TITLES_ID).data().toString();
    QString fullPath = QDir::currentPath() + "/data/" + path + "/" + id + ".txt";
    qDebug() << fullPath;

    //int category = sitesWidget->currentItem()->text(SITES_ID).toInt();

    //if(modelList.at(category)->removeRow(index.row()))
    if(proxyModel->removeRow(index.row()))
    {
        int count = sitesWidget->currentItem()->data(SITES_TOTAL, Qt::DisplayRole).toInt() - 1;
        sitesWidget->currentItem()->setData(SITES_TOTAL, Qt::DisplayRole, count);
        fetch->setDataChanged(true);
    }

    if(QFile(fullPath).exists())
        QFile::remove(fullPath);

    else
        QMessageBox::critical(this, tr("Fichier introuvable"),
                           tr("Le fichier <br /><b>%1</b><br /> n'existe pas.").arg(fullPath));
}

void MainWindow::selectedArtChanged(const QItemSelection &selected)
{
    QModelIndexList indexes = selected.indexes();

    if (!indexes.isEmpty())
         setArticleActEnabled(true);

    else
        setArticleActEnabled(false);
}

void MainWindow::setArticleActEnabled(bool enable)
{
    openBrowserAct->setEnabled(enable);
    copyLinkAct->setEnabled(enable);
    openFileAct->setEnabled(enable);
    openLocationAct->setEnabled(enable);
    deleteArtAct->setEnabled(enable);
}

void MainWindow::showFindWidget()
{
    findWidget->setWebView(articlesWebView);
    findWidget->show();
}

void MainWindow::searchDialogSlot()
{
    findDialog = new FindDialog();
    findDialog->setModelList(modelList);
    findDialog->show();
}

void MainWindow::deleteDialogSlot()
{
    deleteDialog = new DeleteDialog();
    deleteDialog->setModelList(modelList);

    connect(deleteDialog, SIGNAL(updateCount(int)), this, SLOT(updateSiteCount(int)));
    connect(deleteDialog, SIGNAL(dataChanged(bool)), fetch, SLOT(setDataChanged(bool)));

    if(deleteDialog->exec())
    {
        qDebug() << "dataChanged after deleteDialog exce()" << fetch->isDataChanged();

        if(fetch->isDataChanged())
        {
            setStatusBarTextSlot(tr("Enregistrement du corpus..."));
            data->saveData(modelList);
            fetch->setDataChanged(false);
        }
    }
}

void MainWindow::settingsDialogSlot()
{
    settingsDialog = new SettingsDialog();
    if(settingsDialog->exec())
    {
        //writeSettings();
        settings.loadSettings();
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("À propos"),
                       tr("<h2>Stories Crawler %1</h2>"
                          "<p>Ce programme télécharge et affiche le contenu de <br />"
                          "plusieurs sites web pour crée un corpus linguistique.</p>"
                          "Copyright &copy; 2010 <br /><br />"
                          "Amine Roukh, Abdelkadir Sadouki."
                          "<br />Étudiants en l'université Abdelhamid Ibn Badis"
                          "<br />Encadré par M.Abdelrezzak Brahmi."
                          ).arg(qApp->applicationVersion()));
}

void MainWindow::closeEvent( QCloseEvent *event )
{
    if(fetch->isReadingTitles() || fetch->isReadingArticles()) // If we are fetching
    {
        int ret = QMessageBox::warning(this, tr("Confirmation"), tr("Voulez-vous vraiment quitter Stories Crawler ?"),
                                       QMessageBox::Cancel, QMessageBox::Close);

        if (ret == QMessageBox::Cancel)
        {
            event->ignore();
            return;
        }
        abortFetch();
    }

    if(fetch->isDataChanged())
    {
        qDebug() << "saving corpus data...";
        setStatusBarTextSlot(tr("Enregistrement du corpus..."));
        data->saveData(modelList);
    }
    else
        qDebug() << "corpus data not changed";

    writeSettings();
    event->accept();
}

void MainWindow::readSettings()
{
    QSettings settings("PFE", "storiescrawler");
    
    splitter->restoreState(settings.value("Gui/Splitter").toByteArray());
    verticalSplitter->restoreState(settings.value("Gui/VerticalSplitter").toByteArray());
    titlesView->header()->restoreState(settings.value("Gui/TitlesViewheader").toByteArray());
    //findWidget->restoreGeometry(settings.value("Gui/FindWidget").toByteArray());
    sitesWidget->header()->restoreState(settings.value("Gui/SitesWidgetheader").toByteArray());
    resize(settings.value("Gui/Size", QSize(700,540)).toSize());
    move(settings.value("Gui/Pos", QPoint(200, 200)).toPoint());
    //viewHtmlAct->setChecked(settings.value("ViewInHtmlOld", true).toBool());

    sitesWidget->hideColumn(SITES_URL); // Hide Url column
    sitesWidget->hideColumn(SITES_ID); // Hide Id column
    sitesWidget->hideColumn(SITES_PATH); // Hide Path column
    titlesView->hideColumn(TITLES_URL); // Hide Link column
    titlesView->hideColumn(TITLES_ID); // Hide Id column
    titlesView->hideColumn(TITLES_STATUS); // Hide Status column
    sitesWidget->header()->resizeSection( SITES_NAME, 130 );
    sitesWidget->header()->resizeSection( SITES_TOTAL, 10 );
    titlesView->header()->resizeSection( TITLES_NAME, 300 );
    titlesView->header()->resizeSection( TITLES_DATE, 70 );

    // Restore last category and article
    if(settings.value("SaveCatArt", true).toBool())
    {
        QTreeWidgetItemIterator it(sitesWidget, QTreeWidgetItemIterator::NoChildren);
        it += settings.value("CurrentCategory", 0).toInt();
        if((*it))
        {
            sitesWidget->setCurrentItem((*it));
            itemActivated((*it));
        }

        QString articleId = settings.value("CurrentArticle", "0").toString();

        for(int i=0; i < proxyModel->rowCount(); ++i)
        {
            if(proxyModel->index(i, TITLES_ID).data().toString() == articleId)
            {
                QModelIndex index = proxyModel->index(i, 0);
                titlesView->setCurrentIndex(index);
                activated(index);
                break;
            }
        }
    }

    if(settings.value("Startup/FetchNothingOnStartUp", true).toBool())
        return;
    if(settings.value("Startup/FetchAllOnStartUp", false).toBool())
        fetchAll();
    else if(settings.value("Startup/FetchLatestOnStartUp", false).toBool())
        fetchLatest();
    else if(settings.value("Startup/FetchCorruptOnStartUp", false).toBool())
        fetchCorrupt();
}

void MainWindow::writeSettings()
{
    QSettings settings("PFE", "storiescrawler");
    
    settings.setValue("Gui/Splitter", splitter->saveState());
    settings.setValue("Gui/VerticalSplitter", verticalSplitter->saveState());
    settings.setValue("Gui/TitlesViewheader", titlesView->header()->saveState());
    //settings.setValue("Gui/FindWidget", findWidget->saveGeometry());
    settings.setValue("Gui/sitesWidgetheader", sitesWidget->header()->saveState());
    settings.setValue("Gui/pos", pos());
    settings.setValue("Gui/size", size());
    //settings.setValue("ViewInHtmlOld", viewHtmlAct->isChecked());

    if(settings.value("SaveCatArt", true).toBool())
    {
        if(sitesWidget->currentItem()) // Fix crash on exit when seeds.xml does not exist
            settings.setValue("CurrentCategory", sitesWidget->currentItem()->text(SITES_ID));
        QString id = proxyModel->index(titlesView->currentIndex().row(), TITLES_ID).data().toString();
        settings.setValue("CurrentArticle", id);
    }
}
