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


#ifndef FETCH_H
#define FETCH_H

#include "titleslisting.h"
#include "constants.h"
#include "data.h"
#include "titleslisting.h"
#include "httphandler.h"
#include "settings.h"

#include <QObject>
#include <QTreeWidget>

class Fetch : public QObject
{
    Q_OBJECT

public:
    Fetch();
    void fetch(FetchNbr nbr);
    void setSitesWidget(QTreeWidget *sitesWidget);
    void setModelList(QList<QStandardItemModel*> modelList);
    void readCorrupt();
    void fetchLatest();
    void fetchAll();
    void fetchSelected();
    void fetchSite();
    void fetchCorrupt();
    void abortFetch();
    bool isReadingArticles() const;
    bool isReadingTitles() const;
    bool isReadingCorrupt() const;
    bool isDataChanged() const;

public slots:
    void titlesReceived(QString titleString, QString pubDateString, QString linkString);
    void fetchFinished();
    void articleReceived(QString articleContent);
    void htmlErrorReceived(QString str);
    void setDataChanged(bool changed);

signals:
    void updateWidget();
    void setStatusBarText(QString status, int timeout = 0);

private:
    void beforeFetch();
    void afterFetch();
    void readArticles();
    void fetchArticles();
    void initializeVar();
    void setSiteColor(QTreeWidgetItem *item, QColor color);
    void setAricleColor(QStandardItemModel *model, int row, QColor color, ArticleStatus Status);
    QTreeWidgetItem *getItemFromCat(int category);

private:
    QTreeWidget *m_sitesWidget;
    QTreeWidgetItem *currentItem;

    QList<QStandardItemModel*> m_modelList;
    QStandardItemModel *model;

    bool end;
    bool aborted;
    bool isReadingArt;
    bool isReadingTil;
    bool m_fetchSelected;
    bool m_fetchSite;
    bool m_corrupt;
    bool dataChanged;
    int currentCat;
    int curArtCat;
    int currentUrlRow;
    int lastCatId;
    FetchNbr m_FetchNbr;

    int fetchCatNbr;
    int fetchAllNbr;

    Data *data;
    TitlesListing *titlesListing;
    HttpHandler *httpHandler;
    Settings settings;
};

#endif // FETCH_H
