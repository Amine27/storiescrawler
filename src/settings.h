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


#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QColor>
#include <QFont>
#include <QTextCodec>
#include <QDebug>

class Settings
{

public:
    Settings();
    void loadSettings();

public:
    /*bool fetchNothing;
    bool fetchAll;
    bool fetchLatest;
    bool fetchCorrupt;*/

    bool saveCatArt;
    bool exitAfterFetch;
    bool enablePlugins;

    bool viewInHTML;
    bool viewInText;

    bool ltr;
    bool rtl;

    //bool customColor;
    QColor notFetchedColor;
    QColor fetchArticlesColor;
    QColor fetchTitlesColor;

    QString fontFamily;
    QString fontStyle;
    QString fontSize;

    QString language;

    int maxFetchCat;
    int maxFetchAll;
    int maxFetchTime;
    int maxRetries;
    int timeout;

    int fetchSelectedOptions;
    int fetchSiteOptions;

    QTextCodec *codec;

    bool proxyEnabled;
    int proxyType;
    int proxyPort;
    QString proxyHostName;
    QString proxyUserName;
    QString proxyPassword;

    bool useFromEmail;
    QByteArray emailAddress;

    QByteArray userAgent;
};

#endif // SETTINGS_H
