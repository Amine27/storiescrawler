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


#ifndef TITLESLISTING_H
#define TITLESLISTING_H

#include "settings.h"

#include <QObject>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QXmlStreamReader>
#include <QDebug>
#include <QDate>
#include <QTimer>

enum FetchNbr
{
    Latest = 0,
    All
};

class TitlesListing : public QObject
{
    Q_OBJECT

public:
    TitlesListing(QObject *parent = 0);
    void fetch(QUrl url, int begin, FetchNbr nbr);
    void abort();
    FetchNbr getFetchNbr() const;

signals:
    void newTitlesData(QString titleString, QString pubDateString, QString linkString);
    void finish();

public slots:
    void replyFinished(QNetworkReply*);
    void errorSlot(QNetworkReply::NetworkError error);
    void updateTime();

private:
    void parseXml(QString out);
    void parseHtml(QString out);
    int getArticleNbr(QString out) const;

private:
    int rssNbr;
    int httpNbr;
    int lastPageTry;
    int retriesNbr;
    bool validRss;
    bool validHtml;
    bool aborted;
    bool isRunning;
    bool timeout;
    bool error;
    QString m_currentUrl;
    FetchNbr m_FetchNbr;

    QTimer *timer;

    QXmlStreamReader xml;
    QNetworkAccessManager *m_manager;
    QNetworkReply *networkReply;
    QNetworkRequest request;

    Settings settings;
};

#endif // TITLESLISTING_H
