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


#include "fetch.h"

Fetch::Fetch()
{
    model = new QStandardItemModel(0, 6, this);
    model->setHeaderData(TITLES_NAME, Qt::Horizontal, QObject::tr("Titre"));
    model->setHeaderData(TITLES_DATE, Qt::Horizontal, QObject::tr("Date"));
    model->setHeaderData(TITLES_URL, Qt::Horizontal, QObject::tr("Link"));
    model->setHeaderData(TITLES_ID, Qt::Horizontal, QObject::tr("Id"));
    model->setHeaderData(TITLES_STATUS, Qt::Horizontal, QObject::tr("Status"));

    initializeVar();

    data = new Data;
    titlesListing = new TitlesListing;
    httpHandler = new HttpHandler;

    connect(titlesListing, SIGNAL(newTitlesData(QString, QString, QString)),
             this, SLOT(titlesReceived(QString, QString, QString)));

    connect(titlesListing, SIGNAL(finish()), this, SLOT(fetchFinished()));

    connect(httpHandler, SIGNAL(newArticleData(QString)),
            this, SLOT(articleReceived(QString)));

    connect(httpHandler, SIGNAL(networkError(QString)),
            this, SLOT(htmlErrorReceived(QString)));

    fetchAllNbr = 0;
    fetchCatNbr = 0;

    dataChanged = false;
}

void Fetch::setSitesWidget(QTreeWidget *sitesWidget)
{
    m_sitesWidget = sitesWidget;
}

void Fetch::setModelList(QList<QStandardItemModel*> modelList)
{
    m_modelList = modelList;
}

void Fetch::fetch(FetchNbr nbr)
{
    QTreeWidgetItemIterator it(m_sitesWidget, QTreeWidgetItemIterator::NoChildren);

    currentCat++;
    it += currentCat;

    // If there are still category to read
    if ((*it))
    {
        currentItem = (*it);
        // Change current title color to blue
        //setSiteColor(currentItem, Qt::blue);
        setSiteColor(currentItem, settings.fetchTitlesColor);

        QString url = (*it)->text(SITES_URL);
        QString nbrStr;
        int begin = 0;

        if(url.contains(ECHOROUKONLINE))
        {
            url = url.section('.', 0, 2) + ".";
            url += nbrStr.setNum(1) + ".rss";

            begin = 1;
        }

        else if(url.contains(ALARABIYA))
        {
            QString last = '&' + url.section('&', 1, 2);
            QString first = url.section('=', 0, 0) + '=';
            url = first + nbrStr.setNum(0) + last;

            begin = 0;
        }

        else if(url.contains(REUTERS))
        {
            QDate date = QDate::currentDate();
            url = QString( url + date.toString("MMddyyyy") );

            begin = 0;
        }

        else if(url.contains(ELKHABAR))
        {
            QDate date = QDate::currentDate();
            QString prevDay = QDate::fromString(date.toString("yyyyMMdd"), "yyyyMMdd").addDays(-1).toString("yyyyMMdd");
            url = QString( url + prevDay);

            begin = 0;
        }

        else if(url.contains(AFP))
        {
            begin = 0;
        }

        else if(url.contains(XINHUA))
        {
            begin = 1;
        }

        else if(url.contains(BBC))
        {
            begin = 1;
        }

        isReadingTil = true;

        titlesListing->fetch(url, begin, nbr);

        emit setStatusBarText(tr("Téléchargement des titres du %1 catégorie").arg(currentCat+1));
        end = false;
    }
    else
    {
        currentCat = -1;
        end = true;
        //data->saveData(m_modelList);
    }
}

void Fetch::titlesReceived(QString titleString, QString pubDateString, QString linkString)
{
    if(titlesListing->getFetchNbr() == All)
    {
        qDebug() << "titlesReceived" << fetchCatNbr << settings.maxFetchCat << currentCat;

        if(settings.maxFetchAll != 299 && fetchAllNbr+1 > settings.maxFetchAll)
        {
            qDebug() << "max all fetch";
            titlesListing->abort();
            fetchFinished();
            return;
        }

        if(settings.maxFetchCat != 59 && fetchCatNbr+1 > settings.maxFetchCat)
        {
            qDebug() << "max fetch in cat";
            titlesListing->abort();
            fetchFinished();
            return;
        }
    }

    if(linkString.contains(ECHOROUKONLINE))
    {
        pubDateString.remove(0, 5);
        pubDateString.chop(6);

        QStringList monthsName;
        monthsName << "Jan" << "Feb" << "Mar" <<  "Apr" <<  "May" << "Jun" << "Jul" << "Aug" << "Sep" << "Oct" <<  "Nov" << "Dec";

        int ind = 1, i;
        QString str;
        foreach( QString month, monthsName )
        {
            if (( i = pubDateString.indexOf( month, 0, Qt::CaseInsensitive )) != -1 )
                pubDateString.replace( i, 3, str.setNum(ind));
            ind++;
        }

        // FIXME: Show in RTL bug = FIXED
        pubDateString = QDateTime::fromString(pubDateString, "dd M yyyy hh:mm:ss").toString("yyyy/MM/dd hh:mm");
    }

    else if(linkString.contains(REUTERS))
    {
        pubDateString = QDateTime::fromString(pubDateString, "MMddyyyy").toString("yyyy/MM/dd");
    }

    else if(linkString.contains(AFP))
    {
        pubDateString = QDateTime::fromString(pubDateString, "dd/MM/yyyy").toString("yyyy/MM/dd");
    }

    else if(linkString.contains(XINHUA))
    {
        pubDateString = QDateTime::fromString(pubDateString, "yyyy.MM.dd hh:mm:ss").toString("yyyy/MM/dd hh:mm");
    }

    // If model for current category exist update it if needed
    if (m_modelList.size() > currentCat)
    {
        // Check if item exist in the model before insert it
        if (m_modelList.at(currentCat)->findItems(titleString).isEmpty())
        {
            QString str;
            int row = m_modelList.at(currentCat)->rowCount();
            int titleId = row;

            while(!m_modelList.at(currentCat)->findItems(str.setNum(titleId), Qt::MatchExactly, TITLES_ID).isEmpty())
                titleId++;


            m_modelList.at(currentCat)->insertRow(row);
            m_modelList.at(currentCat)->setData(m_modelList.at(currentCat)->index(row, TITLES_NAME), titleString);
            m_modelList.at(currentCat)->setData(m_modelList.at(currentCat)->index(row, TITLES_DATE), pubDateString);
            m_modelList.at(currentCat)->setData(m_modelList.at(currentCat)->index(row, TITLES_URL), linkString);
            m_modelList.at(currentCat)->setData(m_modelList.at(currentCat)->index(row, TITLES_ID), titleId);

            //setAricleColor(m_modelList.at(currentCat), row, Qt::gray, NotFetched); // Not fetched yet by default
            setAricleColor(m_modelList.at(currentCat), row, settings.notFetchedColor, NotFetched); // Not fetched yet by default

            // Update Articles number
            currentItem->setData(SITES_TOTAL, Qt::DisplayRole, row+1);

            fetchCatNbr++;
            fetchAllNbr++;
        }
    }
    // Else create new one
    else
    {
        int row = model->rowCount();
        model->insertRow(row);
        model->setData(model->index(row, TITLES_NAME), titleString);
        model->setData(model->index(row, TITLES_DATE), pubDateString);
        model->setData(model->index(row, TITLES_URL), linkString);
        model->setData(model->index(row, TITLES_ID), row);

        //setAricleColor(model, row, Qt::gray, NotFetched);
        setAricleColor(model, row, settings.notFetchedColor, NotFetched);

        currentItem->setData(SITES_TOTAL, Qt::DisplayRole, row+1);

        fetchCatNbr++;
        fetchAllNbr++;
    }
}

void Fetch::fetchFinished()
{
    if(titlesListing->getFetchNbr() == All)
    {
        if(settings.maxFetchAll != 299 && fetchAllNbr+1 > settings.maxFetchAll)
        {
            titlesListing->abort();
            end = true;
        }

        // If last category
        if(settings.maxFetchCat != 59 && m_modelList.size() <= currentCat+1)
        {
            qDebug() << "last cat";
            titlesListing->abort();
            //return;
        }
        else
            fetchCatNbr = 0;
    }

    // if model dosen't exist insert it
    if (m_modelList.size() <= currentCat)
        m_modelList.append(model);

    // Reset fetched title color to default (black)
    setSiteColor(currentItem, QApplication::palette().color(QPalette::Text));

    isReadingTil = false;

    if (!end && !aborted && !m_fetchSelected)
    {
        QTreeWidgetItem *next = m_sitesWidget->itemBelow(currentItem);
        if(next) // Not out of sites list
        {
            if(m_fetchSite && next->text(SITES_ID).isEmpty())
            {
                qDebug() << "last category in this site";
                end = true;
            }

            else
            {
                // Fetch next category (titles)
                fetch(m_FetchNbr);

                model = new QStandardItemModel(0, 6, this);
                model->setHeaderData(TITLES_NAME, Qt::Horizontal, QObject::tr("Titre"));
                model->setHeaderData(TITLES_DATE, Qt::Horizontal, QObject::tr("Date"));
                model->setHeaderData(TITLES_URL, Qt::Horizontal, QObject::tr("Link"));
                model->setHeaderData(TITLES_ID, Qt::Horizontal, QObject::tr("Id"));
                model->setHeaderData(TITLES_STATUS, Qt::Horizontal, QObject::tr("Status"));
            }
        }
    }
    // Start reading articles
    if (!isReadingArt)
    {
        // Change current title color to red (if it is not blue!)
        //if(getItemFromCat(curArtCat)->foreground(SITES_NAME).color() != Qt::blue)
            //setSiteColor(getItemFromCat(curArtCat), Qt::red);
        if(getItemFromCat(curArtCat)->foreground(SITES_NAME).color() != settings.fetchTitlesColor)
            setSiteColor(getItemFromCat(curArtCat), settings.fetchArticlesColor);

        readArticles();
    }

    if (end && !isReadingArt)
        afterFetch();
}

// Manage current row and article before fetch
void Fetch::readArticles()
{

    // If there are still category and articles to read
    if (curArtCat < m_modelList.size() && currentUrlRow < m_modelList.at(curArtCat)->rowCount())
    {
        //if(getItemFromCat(curArtCat)->foreground(SITES_NAME).color() != Qt::blue)
            //setSiteColor(getItemFromCat(curArtCat), Qt::red);
        if(getItemFromCat(curArtCat)->foreground(SITES_NAME).color() != settings.fetchTitlesColor)
            setSiteColor(getItemFromCat(curArtCat), settings.fetchArticlesColor);

        fetchArticles();
    }
    // If we are at the last article go to next category if it's exist
    else if (currentUrlRow >= m_modelList.at(curArtCat)->rowCount())
    {
        isReadingArt = false;

        //if(getItemFromCat(curArtCat)->foreground(SITES_NAME).color() != Qt::blue)
        if(getItemFromCat(curArtCat)->foreground(SITES_NAME).color() != settings.fetchTitlesColor)
            setSiteColor(getItemFromCat(curArtCat), QApplication::palette().color(QPalette::Text));

        curArtCat++;

        // If it is the last category, or last caterory for current site,
        // or fetch selected; than finish, else read saved rowCount
        if (curArtCat >= m_modelList.size() || (curArtCat > lastCatId && m_fetchSite) || m_fetchSelected)
        {
            qDebug() << "2- last Cat or m_fetchSelected:" << m_fetchSelected
                    << "or m_fetchSite:" << m_fetchSite;
            curArtCat--; // Fix crash caused by out of range in afterFetch() setColor fun
            afterFetch();
        }
        else
        {
            QString path = getItemFromCat(curArtCat)->text(SITES_PATH);
            currentUrlRow = data->readRowCount(path);
            qDebug() << currentUrlRow << "/" << m_modelList.at(curArtCat)->rowCount() << "articles from category:" << curArtCat+1;

            // Update current curArtCat to currentCat-1
            if(curArtCat < currentCat)
            {
                readArticles();
            }

            // or readArticles if last caterory and we are not fetching titles
            else if((curArtCat == currentCat) && (currentCat == m_modelList.size()-1) && !isReadingTil)
            {
                qDebug() << curArtCat << currentCat << m_modelList.size() << isReadingTil;
                qDebug() << "pass";
                readArticles();
            }
        }
    }
}

void Fetch::articleReceived(QString articleContent)
{
    // If article is empty duo to connexion error re-download it
    if(articleContent.size() < 50)
    {
        //qDebug() << "Content is empty, re-download url: " << modelList.at(curArtCat)->index(currentUrlRow, 2).data().toString();
        //readArticles();
        qDebug() << "Content is empty!!";
        //return;
    }
    else
    {
        // Change article status to Fetched and update it's color
        setAricleColor(m_modelList.at(curArtCat), currentUrlRow, QApplication::palette().color(QPalette::Text), Fetched);
    }

    QString title = m_modelList.at(curArtCat)->index(currentUrlRow, TITLES_NAME).data().toString();
    QString path = getItemFromCat(curArtCat)->text(SITES_PATH);
    QString id = m_modelList.at(curArtCat)->index(currentUrlRow, TITLES_ID).data().toString();

    data->saveArticle(title, articleContent, id, path);

    // Continue fetch corrupt articles if the fun is called
    if(m_corrupt)
    {
        readCorrupt();
    }

    else
    {
        currentUrlRow++;
        qDebug() << "receive article:" << currentUrlRow << "/" << m_modelList.at(curArtCat)->rowCount();

        // Save rowCount to continue donwloading in the future
        QString path = getItemFromCat(curArtCat)->text(SITES_PATH);
        data->saveRowCount(currentUrlRow, path);

        // If we are not at the end of the category
        if (currentUrlRow < m_modelList.at(curArtCat)->rowCount())
        {
            readArticles();
        }
        // Else If it's not the last category, and not "get selected category articles"
        else
        {
            //if(getItemFromCat(curArtCat)->foreground(SITES_NAME).color() != Qt::blue)
            if(getItemFromCat(curArtCat)->foreground(SITES_NAME).color() != settings.fetchTitlesColor)
                setSiteColor(getItemFromCat(curArtCat), QApplication::palette().color(QPalette::Text));

            curArtCat++;

            qDebug() << "Finishing the caterogry " << curArtCat; //<< " in: " << timeLabel->text().right(8);

            // If it is the last category, or last caterory for current site,
            // or fetch selected; than finish, else read saved rowCount
            if (curArtCat >= m_modelList.size() || (curArtCat > lastCatId && m_fetchSite) || m_fetchSelected)
            {
                qDebug() << "1- last Cat or m_fetchSelected:" << m_fetchSelected
                        << "or m_fetchSite:" << m_fetchSite;
                curArtCat--;
                afterFetch();
            }
            else
            {
                QString path = getItemFromCat(curArtCat)->text(SITES_PATH);
                currentUrlRow = data->readRowCount(path);

                readArticles();
            }
        }
    }
}

void Fetch::readCorrupt()
{
    bool finished = false;

    do
    {
        currentUrlRow++; // Here to avoid infinit loop on "Content is Empty!!" case

        if (currentUrlRow >= m_modelList.at(curArtCat)->rowCount())
        {
            setSiteColor(getItemFromCat(curArtCat), QApplication::palette().color(QPalette::Text));

            curArtCat++;

            if(getItemFromCat(curArtCat))
                //setSiteColor(getItemFromCat(curArtCat), Qt::red);
                setSiteColor(getItemFromCat(curArtCat), settings.fetchArticlesColor);

            // If it is the last category than finish, else read saved rowCount
            if (curArtCat >= m_modelList.size())
            {
                qDebug() << "last Cat in fetch corrupt";
                finished = true;
            }
            else
            {
                qDebug() << "category:" << curArtCat+1;
                currentUrlRow = -1;
            }
        }

        else
        {
            QString path = getItemFromCat(curArtCat)->text(SITES_PATH);
            QString id = m_modelList.at(curArtCat)->index(currentUrlRow, TITLES_ID).data().toString();
            int status = m_modelList.at(curArtCat)->index(currentUrlRow, TITLES_STATUS).data().toInt();
            QString article = data->readArticle(id, path);

            //qDebug() << currentUrlRow;

            // If article is corrupt break from the loop
            if(article.size() < 50)
            {
                qDebug() << "read corrupt article id:" << currentUrlRow << "in category" << curArtCat+1;
                break;
            }
            else if(!status)
                setAricleColor(m_modelList.at(curArtCat), currentUrlRow, QApplication::palette().color(QPalette::Text), Fetched);
        }

        QApplication::processEvents();

    } while(!finished);

    // If we are not at the end: read corrupt article
    if(!finished)
        fetchArticles();

    else
        afterFetch();
}

// Read Article from website
void Fetch::fetchArticles()
{
    QString url = m_modelList.at(curArtCat)->index(currentUrlRow, TITLES_URL).data().toString();

    emit setStatusBarText(tr("Lecture d'article: %1/%2 du %3 catégorie").arg(currentUrlRow+1)
                          .arg(m_modelList.at(curArtCat)->rowCount()).arg(curArtCat+1));

    httpHandler->get(QString(url), false);
    isReadingArt = true;
}

void Fetch::htmlErrorReceived(QString /*str*/)
{
    //qDebug() << "htmlErrorReceived";

    if(isReadingArt && !aborted)
        articleReceived("");
}

QTreeWidgetItem *Fetch::getItemFromCat(int category)
{
    QTreeWidgetItemIterator it(m_sitesWidget, QTreeWidgetItemIterator::NoChildren);
    it += category;
    return (*it);
}

void Fetch::setSiteColor(QTreeWidgetItem *item, QColor color)
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

void Fetch::setAricleColor(QStandardItemModel *model, int row, QColor color, ArticleStatus Status)
{
    QBrush brush(color);
    model->item(row, TITLES_NAME)->setForeground(brush);
    model->item(row, TITLES_DATE)->setForeground(brush);
    model->setData(model->index(row, TITLES_STATUS), Status);

    // Mark the change of data status
    dataChanged = true;
}

void Fetch::fetchSelected()
{
    qDebug() << "fetchSelected";
    m_fetchSelected = true;    
    curArtCat = m_sitesWidget->currentItem()->text(SITES_ID).toInt();
    currentCat = curArtCat-1;
    beforeFetch();

//#ifdef Q_OS_LINUX
//    m_FetchNbr = Latest;
//#else
//    m_FetchNbr = All;
//#endif

    if(settings.fetchSelectedOptions == 0) // All
        m_FetchNbr = All;
    else if(settings.fetchSelectedOptions == 1) // Latest
        m_FetchNbr = Latest;

    fetch(m_FetchNbr);
}

void Fetch::fetchLatest()
{
    qDebug() << "fetchLatest";
    beforeFetch();
    m_FetchNbr = Latest;
    fetch(m_FetchNbr);
}

void Fetch::fetchAll()
{
    qDebug() << "fetchAll";
    beforeFetch();
    m_FetchNbr = All;
    fetch(m_FetchNbr);
}

void Fetch::fetchSite()
{
    qDebug() << "fetchSite";
    m_fetchSite = true;
    QTreeWidgetItem *current = m_sitesWidget->currentItem();
    QTreeWidgetItem *next = m_sitesWidget->itemBelow(current);

    currentCat = next->text(SITES_ID).toInt()-1;
    curArtCat = next->text(SITES_ID).toInt();
    lastCatId = current->childCount() + currentCat;

    beforeFetch();
//#ifdef Q_OS_LINUX
//    m_FetchNbr = Latest;
//#else
//    m_FetchNbr = All;
//#endif

    if(settings.fetchSiteOptions == 0) // All
        m_FetchNbr = All;
    else if(settings.fetchSiteOptions == 1) // Latest
        m_FetchNbr = Latest;

    fetch(m_FetchNbr);
}

void Fetch::fetchCorrupt()
{
    qDebug() << "fetchCorrupt";
    m_corrupt = true;
    beforeFetch();

    if(getItemFromCat(curArtCat))
        //setSiteColor(getItemFromCat(curArtCat), Qt::red);
        setSiteColor(getItemFromCat(curArtCat), settings.fetchArticlesColor);

    currentUrlRow = -1;
    readCorrupt();
}

void Fetch::beforeFetch()
{
    settings.loadSettings();

    // Read saved rowCount for current category to continue donwloading
    if (m_modelList.size() > curArtCat && !m_corrupt)
    {
        QString path = getItemFromCat(curArtCat)->text(SITES_PATH);
        currentUrlRow = data->readRowCount(path);
        //qDebug() << path << currentUrlRow;
        qDebug() << "continue from article id: " << currentUrlRow;
        m_modelList.at(curArtCat)->sort(TITLES_DATE, Qt::DescendingOrder); // To avoid Url(s) conflicts
    }
}

void Fetch::afterFetch()
{
    emit setStatusBarText(tr("Terminer"), 2000);

    setSiteColor(getItemFromCat(curArtCat), QApplication::palette().color(QPalette::Text));

    if(dataChanged)
    {
        data->saveData(m_modelList);
        dataChanged = false;
    }

    emit updateWidget();

    initializeVar();
}

void Fetch::abortFetch()
{
    aborted = true;
    titlesListing->abort();
    httpHandler->abort();

    qDebug() << "fetch Aborted";
    afterFetch();
}

void Fetch::initializeVar()
{
    aborted = false;
    isReadingArt = false;
    isReadingTil = false;
    m_fetchSelected = false;
    m_fetchSite = false;
    m_corrupt = false;
    currentCat = -1;
    currentUrlRow = 0;
    curArtCat = 0;
    lastCatId = 0;
}

bool Fetch::isReadingArticles() const
{
    return isReadingArt;
}

bool Fetch::isReadingTitles() const
{
    return isReadingTil;
}

bool Fetch::isReadingCorrupt() const
{
    return m_corrupt;
}

bool Fetch::isDataChanged() const
{
    return dataChanged;
}

void Fetch::setDataChanged(bool changed)
{
    //qDebug() << "Fetch::setDataChanged" << changed;
    dataChanged = changed;
}
