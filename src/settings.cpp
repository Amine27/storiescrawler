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


#include "settings.h"

Settings::Settings()
{
    //loadSettings();
}

void Settings::loadSettings()
{
    QSettings settings("PFE", "storiescrawler");

    saveCatArt = settings.value("SaveCatArt", true).toBool();
    exitAfterFetch = settings.value("ExitAfterFetch", false).toBool();
    enablePlugins = settings.value("EnablePlugins", true).toBool();

    viewInHTML = settings.value("ViewInHTML", false).toBool();
    viewInText = settings.value("ViewInText", true).toBool();

    ltr = settings.value("Gui/LTR", true).toBool();
    rtl = settings.value("Gui/RTL", false).toBool();

    if(settings.value("Appearance/ColorGroupBox", false).toBool())
    {
        notFetchedColor = QColor(settings.value("Appearance/notFetchedColor", Qt::gray).toString());
        fetchArticlesColor = QColor(settings.value("Appearance/FetchArticlesColor", Qt::red).toString());
        fetchTitlesColor = QColor(settings.value("Appearance/FetchTitlesColor", Qt::blue).toString());
    }
    else
    {
        notFetchedColor = Qt::gray;
        fetchArticlesColor = Qt::red;
        fetchTitlesColor = Qt::blue;
    }

    fontSize = settings.value("Appearance/FontSize", "16").toString();
    fontFamily = qVariantValue<QFont>(settings.value("Appearance/FontFamily", "Arial")).family();
    fontStyle = settings.value("Appearance/FontStyle", "Normal").toString();
    if(fontStyle == "Bold")
        fontStyle = "normal; font-weight: bold";
    else if(fontStyle == "Bold Italic")
        fontStyle = "italic; font-weight: bold";
    else if(fontStyle == "Bold Oblique")
        fontStyle = "oblique; font-weight: bold";
    else if(fontStyle == "Light")
        fontStyle = "normal; font-weight: lighter";


    QStringList langList;
    langList /*<< "Arabic (ar)"*/ << "English (en)" << "French (fr)";
    language = langList.at(settings.value("Appearance/Language", 1).toInt());
    language = ":/translations/storiescrawler_" + language.section('(', 1, 1); language.chop(1);
    //qDebug() << language;

    maxFetchCat = settings.value("Crawling/MaxFetchCat", 59).toInt();
    maxFetchAll = settings.value("Crawling/MaxFetchAll", 299).toInt();
    maxFetchTime = settings.value("Crawling/MaxFetchTime", 0).toInt();
    timeout = settings.value("Crawling/Timeout", 300).toInt()*1000;
    maxRetries = settings.value("Crawling/MaxRetries", 3).toInt();

    fetchSelectedOptions = settings.value("Crawling/FetchSelectedOptions", 0).toInt();
    fetchSiteOptions = settings.value("Crawling/FetchSiteOptions", 0).toInt();

    int mib = settings.value("Crawling/MibCodec", 106).toInt(); // UTF-8 by default (106 from IANA character-sets)
    codec = QTextCodec::codecForMib(mib);
    //qDebug() << mib << codec->name();

    proxyEnabled = settings.value("Proxy/Enabled", false).toBool();
    proxyType = settings.value("Proxy/Type", 0).toInt();
    proxyHostName = settings.value("Proxy/Host").toString();
    proxyPort = settings.value("Proxy/Prot", 1080).toInt();
    proxyUserName = settings.value("Proxy/UserName").toString();
    proxyPassword = settings.value("Proxy/Password").toString();

    useFromEmail = settings.value("Connection/UseFromEmail", false).toBool();
    emailAddress = QString(settings.value("Connection/EmailAddress").toString()).toAscii();

    QStringList userAgentList = settings.value("Connection/UserAgentList").toStringList();
    int index = settings.value("Connection/UserAgent", 0).toInt();
    if(!userAgentList.isEmpty() && userAgentList.count() > index)
    {
        userAgent = QString(userAgentList.at(index)).toAscii();
    }
    else
        userAgent = "Mozilla/4.0 (compatible; AppleWebKit/532.4) StoriesCrawler/0.1";
}
