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


#ifndef DATA_H
#define DATA_H

#include "settings.h"

#include <QObject>
#include <QList>
#include <QStandardItemModel>
#include <QXmlStreamReader>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QPalette>
#include <QApplication>
#include <QDate>

enum ArticleStatus
{
    NotFetched = 0,
    Fetched
};

class Data : public QObject
{
    Q_OBJECT

public:
    Data(QObject *widget = 0);
    void readSeeds();
    void loadData();
    void saveData(QList<QStandardItemModel*> modelList);
    void saveArticle(QString title, QString content, QString id, QString path);
    QString readArticle(QString id, QString path);
    int readRowCount(QString path);
    void saveRowCount(int count, QString path);
    void saveStatistics(QString stati);

    QDate dataMaximumDate() const { return maxDate; }
    QDate dataMinimumDate() const { return (minDate == QDate::currentDate()) ?
                                    QDate::currentDate().addMonths(-12) : minDate ;}

signals:
    void newSeed(QString category, QString catUrl, QString id, QString site, QString siteUrl, QString path);
    void newItem(QStandardItemModel *model);

private:
    void setMinMaxDate(QString date);

private:
    QXmlStreamWriter xmlWriter;
    QXmlStreamReader xmlReader;

    QStandardItemModel *model;

    QDate minDate;
    QDate maxDate;

    Settings settings;
};

#endif // DATA_H
