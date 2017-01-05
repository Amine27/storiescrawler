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


#ifndef HTTPHANDLER_H
#define HTTPHANDLER_H

#include "settings.h"

#include <QObject>
#include <QTextBrowser>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QTimer>

class HttpHandler : public QObject
{
    Q_OBJECT

public:
    HttpHandler(QObject *parent = 0);
    void get(QUrl url, bool formated);
    void abort();

signals:
    void newArticleData(QString article);
    void newFormatedArticle(QString article);
    void networkError(QString);

private slots:
    void replyFinished(QNetworkReply*);
    void errorSlot(QNetworkReply::NetworkError error);
    void updateTime();

private:
    void parseHtml(QString out);
    void articleHtml(QString out);

private:
    //int connectionId;
    int retriesNbr;
    bool m_formated;
    bool aborted;
    bool error;
    bool isRunning;
    //bool timeout;
    QString m_currentUrl;

    QTimer *timer;

    QNetworkAccessManager* m_manager;
    QNetworkReply* networkReply;
    QNetworkRequest request;

    Settings settings;
};

#endif // HTTPHANDLER_H
