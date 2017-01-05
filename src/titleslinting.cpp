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


#include "titleslisting.h"
#include "constants.h"

TitlesListing::TitlesListing(QObject *parent)
        : QObject(parent)
{
    isRunning = false;
    lastPageTry = 0;
    retriesNbr = 0;

    m_manager = new QNetworkAccessManager(this);

    connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    settings.loadSettings();

    if (settings.proxyEnabled)
    {
        QNetworkProxy proxy;

        if (settings.proxyType == 0)
            proxy = QNetworkProxy::Socks5Proxy;
        else
            proxy = QNetworkProxy::HttpProxy;

        proxy.setHostName(settings.proxyHostName);
        proxy.setPort(settings.proxyPort);
        proxy.setUser(settings.proxyUserName);
        proxy.setPassword(settings.proxyPassword);

        m_manager->setProxy(proxy);

        qDebug() << "proxy enabled host" << m_manager->proxy().hostName();
    }

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
}

void TitlesListing::fetch(QUrl url, int begin, FetchNbr nbr)
{
    xml.clear();
    aborted = false;
    validRss = false;
    validHtml = false;
    isRunning = true;
    timeout = false;
    error = false;
    m_FetchNbr = nbr;
    httpNbr = rssNbr = begin;
    m_currentUrl = url.toString();

    //QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", settings.userAgent);
    if(settings.useFromEmail)
        request.setRawHeader("From", settings.emailAddress);
    request.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");

    networkReply = m_manager->get(request);
    connect(networkReply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(errorSlot(QNetworkReply::NetworkError)));

    qDebug() << "list:" << m_currentUrl;

    //timer->stop();
    timer->start(settings.timeout);
}

void TitlesListing::replyFinished(QNetworkReply* nReply)
{
    QString data = nReply->readAll();

    isRunning = false;
    networkReply->deleteLater(); // FIXME: Memory leak - FIXED
    nReply->deleteLater();

    timer->stop();

    //qDebug() << "pass";

    if(timeout || error)
    {
        //qDebug() << "pass";
        fetch(m_currentUrl, httpNbr, m_FetchNbr);
        return;
    }

    if(!aborted)
    {
        if(m_currentUrl.contains(".rss"))
            parseXml(data);
        else
            parseHtml(data);
    }
    
    if(m_FetchNbr == All && (validHtml || validRss) && !aborted) // !aborded <- TEST
    {
        if(m_currentUrl.contains(ECHOROUKONLINE))
        {
            QString nbrStr;

            rssNbr++;
            QString first = m_currentUrl.section('.', 0, 2) + ".";
            m_currentUrl = first + nbrStr.setNum(rssNbr) + ".rss";

            fetch(m_currentUrl, rssNbr, m_FetchNbr);
        }

        else if(m_currentUrl.contains(ALARABIYA))
        {
            QString nbrStr;

            httpNbr += 20;
            QString last = '&' + m_currentUrl.section('&', 1, 2);
            QString first = m_currentUrl.section('=', 0, 0) + '=';
            m_currentUrl = first + nbrStr.setNum(httpNbr) + last;

            fetch(m_currentUrl, httpNbr, m_FetchNbr);
        }

        else if(m_currentUrl.contains(REUTERS))
        {
            QString nextDate = QDate::fromString(m_currentUrl.right(8), "MMddyyyy").addDays(-1).toString("MMddyyyy");
            m_currentUrl.chop(8);
            m_currentUrl += nextDate;

            fetch(m_currentUrl, httpNbr, m_FetchNbr);
        }

        else if(m_currentUrl.contains(ELKHABAR))
        {
            QString date = m_currentUrl.right(8);
            int currentKey = m_currentUrl.section('&', 1, 1).remove("key=").toInt();
            currentKey++;

            if(currentKey > httpNbr)
            {
                date = QDate::fromString(date, "yyyyMMdd").addDays(-1).toString("yyyyMMdd");
                currentKey = 0;
            }

            QString str;
            QString first = m_currentUrl.section('=', 0, 1) + "=";
            m_currentUrl.chop(8);
            m_currentUrl = first + str.setNum(currentKey) + "&date_insert=" + date;

            //qDebug() << m_currentUrl;

            fetch(m_currentUrl, httpNbr, m_FetchNbr);
        }

        /*else if(m_currentUrl.contains(AFP)) // maktoob.com
        {
            QString nbrStr;

            httpNbr++;
            QString first = m_currentUrl.section('=', 0, 2) + "=";
            m_currentUrl = first + nbrStr.setNum(httpNbr);

            fetch(m_currentUrl, httpNbr, m_FetchNbr);
        }*/

        else if(m_currentUrl.contains(AFP))
        {
            QString nbrStr;

            httpNbr++;
            QString first = m_currentUrl.section('=', 0, 0) + '=';
            m_currentUrl = first + nbrStr.setNum(httpNbr);

            fetch(m_currentUrl, httpNbr, m_FetchNbr);
        }

        else if(m_currentUrl.contains(XINHUA))
        {
            QString nbrStr;

            httpNbr++;
            QString first = m_currentUrl.section('=', 0, 0) + '=';
            QString last = '&' + m_currentUrl.section('&', 1, 2);
            m_currentUrl = first + nbrStr.setNum(httpNbr) + last;

            fetch(m_currentUrl, httpNbr, m_FetchNbr);
        }

        else if(m_currentUrl.contains(BBC))
        {
            QString nbrStr;

            httpNbr++;
            QString first = m_currentUrl.section('=', 0, 3) + '=';
            m_currentUrl = first + nbrStr.setNum(httpNbr);

            fetch(m_currentUrl, httpNbr, m_FetchNbr);
        }

    }
    else
        emit finish();
}

void TitlesListing::parseXml(QString out)
{
    xml.addData(out);

    QString currentTag, linkString, titleString, pubDateString;

    while (!xml.atEnd())
    {
        xml.readNext();

        if (xml.isStartElement())
        {
            if (xml.name() == "item")
            {
                linkString = xml.attributes().value("rss:about").toString();
                validRss = true;
            }

            currentTag = xml.name().toString();
        }

        else if (xml.isEndElement())
        {
            if (xml.name() == "item")
            {
                emit newTitlesData(titleString, pubDateString, linkString);

                titleString.clear();
                linkString.clear();
                pubDateString.clear();
            }

        }
        else if (xml.isCharacters() && !xml.isWhitespace() && validRss)
        {
            if (currentTag == "title")
                titleString += xml.text().toString();
            else if (currentTag == "link")
            {
                linkString += xml.text().toString();
                //pathString = linkString.section( '/', -2, -2);
            }
            else if (currentTag == "pubDate")
                pubDateString += xml.text().toString();
        }
    }

    if (xml.error() && xml.error() != QXmlStreamReader::PrematureEndOfDocumentError)
    {
        qWarning() << "XML ERROR:" << xml.lineNumber() << ": " << xml.errorString();
        //abort();
    }

    if (!validRss)
    {
        qWarning() << "Not a valid RSS link";
        //emit finish();
    }
}

void TitlesListing::parseHtml(QString out)
{
    QString text, searchstring, bout;
    int pos1=0, pos2=0, articleNbr;

    // Read articles number in this page
    articleNbr = getArticleNbr(out);

    if(m_currentUrl.contains(ALARABIYA))
    {
        searchstring = "class=\"BK_G\">";

        for(int i=0; i < articleNbr; ++i)
        {
            bout = out;
            pos1 = bout.indexOf(searchstring,pos2);
            pos2 = bout.indexOf("</a>",pos1);
            text = bout.remove(pos2,out.size());
            text = text.remove(0,pos1-36);

            QString title = text.section('>', 1, 1).replace("&quot;", "\"");
            QString link = "http://www.alarabiya.net" + text.section('\'', 1, 1);
            QString date = text.section('/', 2, 4);
            //QString path = m_currentUrl.section('=', 3, 3);

            //qDebug() << date;

            emit newTitlesData(title, date, link);
        }

        // If last page
        if(articleNbr < 20)
        {
            qDebug() << articleNbr << "last title";
            validHtml = false;
        }
        else
            validHtml = true;
    }

    else if(m_currentUrl.contains(REUTERS))
    {
        QString date = m_currentUrl.right(8);
        QString linkStart;

        searchstring = "<div class=\"headlineMed standalone\">";
        //linkStart = "http://ara.reuters.com";

        if(m_currentUrl.contains(REUTERS_EN))
        {
            searchstring = "<div class=\"feature\"><h2>";
            linkStart = "http://fr.reuters.com";
        }
        else if(m_currentUrl.contains(REUTERS_FR))
            linkStart = "http://fr.reuters.com";
        else if(m_currentUrl.contains(REUTERS_ARA))
            linkStart = "http://ara.reuters.com";


        for(int i=0; i < articleNbr; ++i)
        {
            bout = out;
            pos1 = bout.indexOf(searchstring,pos2);
            pos2 = bout.indexOf("<span class=\"timestamp\">",pos1);
            text = bout.remove(pos2,out.size());
            text = text.remove(0,pos1+searchstring.size());

            QString title = text.section('>', 1, 1); title.chop(3);
            QString link = linkStart + text.section('\"', 1, 1) + "?sp=true";
            //QString path = link.section('/', 4, 4);

            //qDebug() << date;

            emit newTitlesData(title, date, link);
        }

        // If last page
        /*if(articleNbr < 1)
        {
            qDebug() << articleNbr << "last title";
            validHtml = false;
        }
        else
            validHtml = true;
        */

        if(articleNbr < 1)
        {
            if(lastPageTry < 10)
            {
                lastPageTry++;
                validHtml = true;
                qDebug() << articleNbr << "last title retry" << lastPageTry;
            }
            else
            {
                qDebug() << articleNbr << "last title page";
                lastPageTry = 0;
                validHtml = false;
            }
        }
        else
        {
            lastPageTry = 0;
            validHtml = true;
        }
    }

    else if(m_currentUrl.contains(ELKHABAR))
    {
        // Page numbers in this date
        httpNbr = out.count("<A");

        searchstring = "<b><a href=";

        for(int i=0; i < articleNbr; ++i)
        {
            bout = out;
            pos1 = bout.indexOf(searchstring,pos2);
            pos2 = bout.indexOf(" ]</font>",pos1);
            text = bout.remove(pos2,out.size());
            text = text.remove(0,pos1+searchstring.size());
            text = text.remove("<font size=\"5\">");

            QString title = text.section('>', 1, 1);
            title = title.remove("</font"); title = title.remove("<br");
            QString link = "http://www.elkhabar.com" + text.section('\"', 1, 1);
            link = link.section('&', 0, 1);
            if(!link.startsWith("..")) // test
                { link.replace("..", ""); link.replace("index.php", ""); } // test
            link += "&date_insert=" + m_currentUrl.right(8);
            QString date = text.section(" [ ", 1, 1); date = date.replace('-', '/');
            //QString path = link.section("idc=", 1, 1); path = path.section('&', 0, 0);

            //qDebug() << path;

            emit newTitlesData(title, date, link);
        }

        // If last page
        /*if(articleNbr < 1)
        {
            qDebug() << articleNbr << "last title";
            validHtml = false;
        }
        else
            validHtml = true;
        */
        if(articleNbr < 1)
        {
            if(lastPageTry < 10)
            {
                lastPageTry++;
                validHtml = true;
                qDebug() << articleNbr << "last title retry" << lastPageTry;
            }
            else
            {
                qDebug() << articleNbr << "last title page";
                lastPageTry = 0;
                validHtml = false;
            }
        }
        else
        {
            lastPageTry = 0;
            validHtml = true;
        }
    }

    /*else if(m_currentUrl.contains(AFP)) // maktoob.com
    {
        searchstring = "<a href=\"http://news.maktoob.com/article/";

        for(int i=0; i < articleNbr; ++i)
        {
            bout = out;
            pos1 = bout.indexOf(searchstring,pos2);
            pos2 = bout.indexOf("</span></span>",pos1);
            text = bout.remove(pos2,out.size());
            text = text.remove(0,pos1);

            QString title = text.section('>', 1, 1).replace("&quot;", "\""); title.chop(3);
            QString link = text.section('\"', 1, 1);
            QString date = text.section('>', 5, 5); date = date.section(' ', 0, 0);
            QString path = m_currentUrl.section("tax=", 1, 1); path = path.section('&', 0, 0);

            emit newTitlesData(title, date, link, path);
        }

        // If last page
        if(articleNbr < 20)
        {
            qDebug() << articleNbr << "last title";
            validHtml = false;
        }
        else
            validHtml = true;
    }*/

    else if(m_currentUrl.contains(AFP))
    {
        searchstring = "<dt class=\"title\">";

        for(int i=0; i < articleNbr; ++i)
        {
            bout = out;
            pos1 = bout.indexOf(searchstring,pos2);
            pos2 = bout.indexOf("</dd>",pos1);
            text = bout.remove(pos2,out.size());
            text = text.remove(0,pos1+searchstring.size());

            QString title = text.section('>', 1, 1); title.chop(3);
            title.replace("&quot;", "\""); title.replace("&#039;", "'");
            //QString link = "http://www.france24.com" + text.section('\"', 1, 1);
            QString link = text.section('\"', 1, 1);
            QString date = text.section("LE ", 1, 1); date = date.trimmed();
            date = date.section(' ', 0, 0);
            //QString path = m_currentUrl.section('/', 6, 6); path = path.section('?', 0, 0);

            emit newTitlesData(title, date, link);
        }

        // If last page
        if(articleNbr < 1)
        {
            qDebug() << articleNbr << "last title";
            validHtml = false;
        }
        else
            validHtml = true;
    }

    else if(m_currentUrl.contains(XINHUA))
    {
        searchstring = "<td valign=top align=right>";

        for(int i=0; i < articleNbr; ++i)
        {
            bout = out;
            pos1 = bout.indexOf(searchstring,pos2);
            pos2 = bout.indexOf("</span></font></td>",pos1);
            text = bout.remove(pos2,out.size());
            text = text.remove(0,pos1+searchstring.size());

            QString title = text.section('>', 1, 1); title.chop(4);
            QString link = text.section('\"', 1, 1);
            QString date = text.section('>', 4, 4);
            //QString path = m_currentUrl.section("=", 3, 3);

            emit newTitlesData(title, date, link);
        }

        // If last page
        if(articleNbr < 1)
        {
            qDebug() << articleNbr << "last title";
            validHtml = false;
        }
        else
            validHtml = true;
    }

    else if(m_currentUrl.contains(BBC))
    {
        searchstring = "<h3><a href=";

        for(int i=0; i < articleNbr; ++i)
        {
            bout = out;
            pos1 = bout.indexOf(searchstring,pos2);
            pos2 = bout.indexOf("<li class=\"date\">",pos1);
            text = bout.remove(pos2+35,out.size());
            text = text.remove(0,pos1);

            QString title = text.section('>', 2, 2); title.chop(3); title = title.trimmed();
            title.replace("&quot;", "\"");
            QString link = text.section('\"', 1, 1);
            QString date = text.section('>', 9, 9); date.chop(4);
            //QString path = m_currentUrl.section("q=", 1, 1); path = path.section('&', 0, 0);

            emit newTitlesData(title, date, link);
        }

        // If last page
        if(articleNbr < 1)
        {
            qDebug() << articleNbr << "last title";
            validHtml = false;
        }
        else
            validHtml = true;
    }
}

int TitlesListing::getArticleNbr(QString out) const
{
    if(m_currentUrl.contains(ALARABIYA))
        return out.count("<div class=\"R_T\" dir=rtl>");

    if(m_currentUrl.contains(REUTERS_EN))
       return out.count("<div class=\"feature\"><h2>");

    if(m_currentUrl.contains(REUTERS))
        return out.count("<div class=\"headlineMed standalone\">");

    if(m_currentUrl.contains(ELKHABAR))
        return out.count("<td bgcolor=\"#ECF1F7\"><div align=\"center\">");

    //if(m_currentUrl.contains(AFP))
        //return out.count("<span class=\"grytxt\">");

    if(m_currentUrl.contains(AFP))
        return out.count("dt class=\"title\">");

    if(m_currentUrl.contains(XINHUA))
        return out.count("<td valign=top align=right>");

    if(m_currentUrl.contains(BBC))
        return out.count("<h3><a href=");

    return 0;
}

void TitlesListing::abort()
{
    aborted = true;

    //if(networkReply->isRunning())
    if(isRunning)
        networkReply->abort();
}

FetchNbr TitlesListing::getFetchNbr() const
{
    return m_FetchNbr;
}

void TitlesListing::errorSlot(QNetworkReply::NetworkError err)
{
    error = true;
    Q_UNUSED(err);
    QNetworkReply *r = qobject_cast<QNetworkReply*>(sender());

    qDebug() << "TitlesListing error:" << r->errorString();

    retriesNbr++;

    //qDebug() << retriesNbr << settings.maxRetries;

    if(retriesNbr <= settings.maxRetries)
    {
        qDebug() << "TitlesListing retries" << retriesNbr;
        return;
    }

    retriesNbr = 0;
    error = false; // To pass

    if(timeout)
    {
        timeout = false; // To pass
    }

    //emit networkError(r->errorString());
}

void TitlesListing::updateTime()
{
    timer->stop();
    qDebug() << "TitlesListing timeout" << timer->interval()/1000 << "secs";
    timeout = true;
    abort();
    //fetch(m_currentUrl, httpNbr, m_FetchNbr);
}
