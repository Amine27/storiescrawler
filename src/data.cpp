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


#include "data.h"
#include "constants.h"

Data::Data(QObject *parent)
        : QObject(parent)
{
    maxDate = QDate(1970, 01, 01);
    minDate = QDate::currentDate();

    settings.loadSettings();
}

void Data::readSeeds()
{
    QFile file;
    file.setFileName(QDir::currentPath()+"/seeds.xml");

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << file.fileName() << file.errorString();
        return;
    }

    xmlReader.setDevice(&file);

    QString site, siteUrl, category, catUrl, id, path;

    while (!xmlReader.atEnd())
    {
        xmlReader.readNext();

        if (xmlReader.isStartElement())
        {
            if (xmlReader.name() == "site")
            {
                site = xmlReader.attributes().value("name").toString();
                siteUrl = xmlReader.attributes().value("url").toString();
            }

            if (xmlReader.name() == "category")
            {
                category = xmlReader.attributes().value("name").toString();
                id = xmlReader.attributes().value("id").toString();
                path = xmlReader.attributes().value("path").toString();
            }
            else if (xmlReader.name() == "url")
                catUrl = xmlReader.readElementText();
        }
        else if (xmlReader.isEndElement() && !catUrl.isEmpty())
        {
            emit newSeed(category, catUrl, id, site, siteUrl, path);

            site.clear();
            siteUrl.clear();
            category.clear();
            catUrl.clear();
            id.clear();
            path.clear();
        }
    }

    if (xmlReader.hasError())
    {
        qDebug() << QString(tr("Cannot read file %1:\n%2.").arg(file.fileName()).arg(xmlReader.errorString()));
        return;
    }

    file.close();
}

void Data::saveData(QList<QStandardItemModel*> modelList)
{
    QFile file;
    file.setFileName(QDir::currentPath()+"/data.xml");
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << file.fileName() << file.errorString();
        return;
    }

    xmlWriter.setDevice(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("data");

    for (int i = 0; i < modelList.size(); ++i)
    {
        xmlWriter.writeStartElement("category");
        QString str;
        xmlWriter.writeAttribute("id", str.setNum(i));


        for (int j = 0; j < modelList.at(i)->rowCount(); ++j)
        {
            xmlWriter.writeStartElement("item");

            xmlWriter.writeAttribute("id", modelList.at(i)->index(j, TITLES_ID).data().toString());

            xmlWriter.writeTextElement("title", modelList.at(i)->index(j, TITLES_NAME).data().toString());
            xmlWriter.writeTextElement("date", modelList.at(i)->index(j, TITLES_DATE).data().toString());
            xmlWriter.writeTextElement("url", modelList.at(i)->index(j, TITLES_URL).data().toString());
            xmlWriter.writeTextElement("status", modelList.at(i)->index(j, TITLES_STATUS).data().toString());

            xmlWriter.writeEndElement();

            setMinMaxDate(modelList.at(i)->index(j, TITLES_DATE).data().toString());
        }

        xmlWriter.writeEndElement();
    }

    xmlWriter.writeEndDocument();

    file.close();
}

void Data::loadData()
{
    QFile file;
    file.setFileName(QDir::currentPath()+"/data.xml");
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << file.fileName() << file.errorString();
        return;
    }

    xmlReader.setDevice(&file);

    QString categoryId, itemId, date, title, url, /*path,*/ pervId, status;

    model = new QStandardItemModel(0, 5, this);
    model->setHeaderData(TITLES_NAME, Qt::Horizontal, QObject::tr("Titre"));
    model->setHeaderData(TITLES_DATE, Qt::Horizontal, QObject::tr("Date"));
    model->setHeaderData(TITLES_URL, Qt::Horizontal, QObject::tr("Link"));
    model->setHeaderData(TITLES_ID, Qt::Horizontal, QObject::tr("Id"));
    model->setHeaderData(TITLES_STATUS, Qt::Horizontal, QObject::tr("Status"));

    while (!xmlReader.atEnd())
    {
        xmlReader.readNext();

        if (xmlReader.isStartElement())
        {
            if (xmlReader.name() == "category")
                categoryId = xmlReader.attributes().value("id").toString();
            else if (xmlReader.name() == "item")
                itemId = xmlReader.attributes().value("id").toString();
            if (xmlReader.name() == "date")
                date = xmlReader.readElementText();
            else if (xmlReader.name() == "title")
                title = xmlReader.readElementText();
            else if (xmlReader.name() == "url")
                url = xmlReader.readElementText();
            else if (xmlReader.name() == "status")
                status = xmlReader.readElementText();
        }
        else if (xmlReader.isEndElement())
        {
            if (pervId != itemId)
            {
                int row = model->rowCount();
                model->insertRow(row);
                model->setData(model->index(row, TITLES_NAME), title);
                model->setData(model->index(row, TITLES_DATE), date);
                model->setData(model->index(row, TITLES_URL), url);
                model->setData(model->index(row, TITLES_ID), itemId);
                model->setData(model->index(row, TITLES_STATUS), status);

                //QBrush color = status.toInt() == NotFetched ? QBrush(Qt::gray) : QBrush(QApplication::palette().color(QPalette::Text));

                QBrush color = status.toInt() == NotFetched ? QBrush(settings.notFetchedColor) :
                               QBrush(QApplication::palette().color(QPalette::Text));

                model->item(row, 0)->setForeground(color);
                model->item(row, 1)->setForeground(color);

                pervId = itemId;

                setMinMaxDate(date);
            }

            if (xmlReader.name() == "category")
            {
                emit newItem(model);

                model = new QStandardItemModel(0, 5, this);
                model->setHeaderData(TITLES_NAME, Qt::Horizontal, QObject::tr("Titre"));
                model->setHeaderData(TITLES_DATE, Qt::Horizontal, QObject::tr("Date"));
                model->setHeaderData(TITLES_URL, Qt::Horizontal, QObject::tr("Link"));
                model->setHeaderData(TITLES_ID, Qt::Horizontal, QObject::tr("Id"));
                model->setHeaderData(TITLES_STATUS, Qt::Horizontal, QObject::tr("Status"));
            }
        }
    }

    if (xmlReader.hasError())
    {
        qDebug() << file.fileName() << xmlReader.errorString();
        return;
    }

    file.close();
}

void Data::saveArticle(QString title, QString content, QString id, QString path)
{
    QFile file;
    QDir dir;
    path = QDir::currentPath()+"/data/"+path;

    if (!dir.exists(path))
    {
        if (!dir.mkpath(path))
        {
            qDebug() << "error while creating '" << path << "' directory";
            return;
        }
    }

    file.setFileName(path+"/"+id+".txt");

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << file.fileName() << file.errorString();
        return;
    }

    QTextStream out(&file);
    //out.setCodec("UTF-8");
    out.setAutoDetectUnicode(false);
    out.setCodec(settings.codec);
    out << title << "\n" << content;

    file.close();
}

QString Data::readArticle(QString id, QString path)
{
    QFile file;
    path = QDir::currentPath()+"/data/"+path;

    file.setFileName(path+"/"+id+".txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << file.fileName() << file.errorString();
        return "";
    }

    //qDebug() << id;

    QTextStream in(&file);
    //in.setCodec("UTF-8");
    in.setAutoDetectUnicode(false);
    in.setCodec(settings.codec);
    in.readLine(); // Skip title line
    file.close();

    //qDebug() << settings.codec->name();

    return in.readAll();
}

int Data::readRowCount(QString path)
{
    QFile file;
    path = QDir::currentPath()+"/data/" + path + "/count.txt";
    file.setFileName(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << file.fileName() << file.errorString();
        return 0;
    }

    QByteArray line = file.readLine();
    file.close();

    return line.toInt();
}

void Data::saveRowCount(int count, QString path)
{
    QFile file;
    path = QDir::currentPath()+"/data/"+path+"/count.txt";
    file.setFileName(path);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << file.fileName() << file.errorString();
        return;
    }

    QTextStream out(&file);
    out << count;

    qDebug() << "save current article id: " << count;
    file.close();
}

void Data::saveStatistics(QString stati)
{
    QFile file;
    file.setFileName( QDir::currentPath() + "/statistics.txt");

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << file.fileName() << file.errorString();
        return;
    }

    QTextStream out(&file);
    out << stati;

    qDebug() << "save global statistics";
    file.close();
}

// Save the max/min date
void Data::setMinMaxDate(QString date)
{
    QDate curDate = QDate::fromString(date, "yyyy/MM/dd");
    if(curDate.isValid())
    {
        maxDate = (maxDate < curDate) ? curDate : maxDate;
        minDate = (minDate > curDate) ? curDate : minDate;
    }
}
