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


#include "httphandler.h"
#include "constants.h"

HttpHandler::HttpHandler(QObject *parent)
        : QObject(parent)
{
    isRunning = false;
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

void HttpHandler::get(QUrl url, bool formated)
{
    aborted = false;
    isRunning = true;
    error = false;
    //timeout = false;
    m_formated = formated;
    m_currentUrl = url.toString();
    url = QString(m_currentUrl);

    //QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", settings.userAgent);
    if(settings.useFromEmail)
        request.setRawHeader("From", settings.emailAddress);
    request.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");

    networkReply = m_manager->get(request);
    connect(networkReply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(errorSlot(QNetworkReply::NetworkError)));

    timer->start(settings.timeout);

    qDebug() << "Proccess:" << url.toString();

}

void HttpHandler::replyFinished(QNetworkReply* nReply)
{
    timer->stop();

    QString data;

    if(m_currentUrl.contains(BBC_OLD))
    {
        QByteArray ba = nReply->readAll();
        QTextCodec *codec = QTextCodec::codecForName("windows-1256");
        QTextDecoder *decoder = codec->makeDecoder();
        data = decoder->toUnicode(ba);

        qDebug() << "httphandler pass by bbc_old encode fun";
    }
    else
        data = nReply->readAll();

    //qDebug() << data;

    isRunning = false;    
    networkReply->deleteLater();
    nReply->deleteLater();

    if(!aborted && !error)
        parseHtml(data);
}

// Parse HTML
void HttpHandler::parseHtml(QString out)
{
    QString finalText, text, searchstring, bout = out;
    int pos1=0, pos2=0;

    //qDebug() << bout;

    if(m_currentUrl.contains(ECHOROUKONLINE))
    {
        if(m_formated) // View in HTML
            searchstring = "<div id=\"article_holder\">";

        else
            searchstring = "<p class=\"abstract\">";

        pos1 = bout.indexOf(searchstring,pos2);
        pos2 = bout.indexOf("<p class=\"nbreads\">",pos1);
    }

    else if(m_currentUrl.contains(ALARABIYA))
    {
        if(m_formated)
        {
            searchstring = "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\" height=\"0\" width=\"330\" dir=\"ltr\">";
            //searchstring = "<div style=\"float:right;width:352px;background-image: url";
            pos1 = bout.indexOf(searchstring,pos2);
            pos2 = bout.lastIndexOf("<a href=\"#000\">",pos1);
        }

        else
        {
            searchstring = "<Mainbody xmlns=\"\">";
            pos1 = bout.indexOf(searchstring,pos2);

            if(bout.contains("<p align=\"right\">"))
                pos2 = bout.lastIndexOf("<p align=\"right\">");

            else
                pos2 = bout.lastIndexOf("<a href=\"#000\">");
        }
    }

    else if(m_currentUrl.contains(REUTERS))
    {
        if(m_formated)
        {
            searchstring = "<div class=\"content\">";
            pos1 = bout.indexOf(searchstring,pos2);
            pos2 = bout.indexOf("drawControls();",pos1);
        }

        else
        {
            searchstring = "<span id=\"midArticle_start\"></span>";
            pos1 = bout.indexOf(searchstring,pos2);
            pos2 = bout.indexOf("<p id=\"copyrightNotice\" class=\"copyright\">",pos1);
        }
    }

    else if(m_currentUrl.contains(ELKHABAR))
    {
        if(m_formated)
            searchstring = "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"15\">";

        else
            searchstring = "<p align=\"justify\"><b><font size=\"4\"><font color=\"#000000\">";

        pos1 = bout.indexOf(searchstring,pos2);
        pos2 = bout.indexOf("<table width=\"100%\" border=\"0\" align=\"right\" cellpadding=\"15\" cellspacing=\"0\">",pos1);
    }

    else if(m_currentUrl.contains(AFP))
    {
        if(m_formated)
        {
            searchstring = "<div class=\"article-header-title\">";
            pos1 = bout.indexOf(searchstring,pos2);
            pos2 = bout.indexOf("height=\"60\" border=\"0\" alt=\"\"></a></noscript>");
        }

        else
        {
            //searchstring = "<p class=\"rteright\">";
            //pos1 = bout.indexOf(searchstring,pos2);
            searchstring = "<div class=\"article-main-text\">"; // TEST
            pos1 = bout.indexOf(searchstring,pos2)-searchstring.size();
            pos2 = bout.indexOf("height=\"60\" border=\"0\" alt=\"\"></a></noscript>");
        }
    }

    else if(m_currentUrl.contains(XINHUA))
    {        
        if(m_formated)
        {
            searchstring = "<table width=\"615\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\">";
            pos1 = bout.indexOf(searchstring,pos2);
            pos2 = bout.indexOf("<table width=\"94%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#F0F7FD\" class=\"huang13\">",pos1);
        }

        else
        {
            searchstring = "<!-- begin_ct -->";
            pos1 = bout.indexOf(searchstring,pos2);
            pos2 = bout.indexOf("<!-- end_ct -->",pos1);
        }
    }

    else if(m_currentUrl.contains(BBC_OLD))
    {
        if(m_formated)
        {
            searchstring = "<!-- S BO -->";
            pos1 = bout.indexOf(searchstring,pos2);
            pos2 = bout.lastIndexOf("<!-- E BO -->");
        }

        else
        {
            searchstring = "<!-- E IIMA -->";
            pos1 = bout.indexOf(searchstring,pos2);
            pos2 = bout.lastIndexOf("<!-- E BO -->");
        }
    }

    else if(m_currentUrl.contains(BBC))
    {
        if(m_formated)
        {
            searchstring = "<div class=\" g-w16 g-first\">";
            pos1 = bout.indexOf(searchstring,pos2);
            pos2 = bout.lastIndexOf("<div class=\"g-container story-body\">");
        }

        else
        {
            searchstring = "<p class=\"ingress\">";
            pos1 = bout.indexOf(searchstring,pos2)-searchstring.size();
            pos2 = bout.lastIndexOf("<div class=\"g-container story-body\">");
        }
    }

    text = bout.remove(pos2,out.size());
    text = text.remove(0,pos1+searchstring.size());

    if(m_formated)
    {
        articleHtml(text);
        return;
    }

    QTextBrowser *textBrowser = new QTextBrowser;
    textBrowser->setText(text);

    finalText = textBrowser->toPlainText(); // Remove HTML tags
    finalText.replace( QRegExp("<[^>]*>"), "" );
    //qDebug() << finalText;

    emit newArticleData(finalText);
}

// Emit formated article
void HttpHandler::articleHtml(QString out)
{
    QFile file;
    file.setFileName(QDir::currentPath()+"/style.css");

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << file.fileName() << file.errorString();
        return;
    }

    QTextStream in(&file);
    QString css = in.readAll();

    QString lang;

    if(m_currentUrl.contains(REUTERS_EN) || m_currentUrl.contains(AFP_EN))
        lang = "lang=\"en\" xml:lang=\"en\"";
    else if(m_currentUrl.contains(REUTERS_FR) || m_currentUrl.contains(AFP_FR))
        lang = "lang=\"fr\" xml:lang=\"fr\"";
    else
        lang = "lang=\"ar\" xml:lang=\"ar\" dir=\"rtl\">";

    //qDebug() << lang;

    QString html = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"
    "<html xmlns=\"http://www.w3.org/1999/xhtml\"" + lang +
    "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" /><style type=\"text/css\">";

    if(m_currentUrl.contains(ECHOROUKONLINE))
    {
        css = css.section("/* echoroukonline.com */", 1, 1);
        html +=  css + "</style></head><body><div id=\"article_holder\">" + out + "</body></html>";
    }

    else if(m_currentUrl.contains(ALARABIYA))
    {
        if(out.contains("</pbody></p></td></tr></table>"))
            out = out.remove(out.lastIndexOf("</pbody></p></td></tr></table>"), out.size());
        else
            out = out.remove(out.lastIndexOf("<a href=\"#000\">"), out.size());

        out.replace( QRegExp("<tr><td><center><a href=\"#000\">*</center></td></tr>"), "" );

        //out = out.remove(QString("<tr><td><center><a href=\"#000\"><img border=\"0\" src=\"http://media.alarabiya.net/img/totop.gif\" title=\"???? ??????\" width=\"35\" height=\"18\" alt=\"???? ??????\" vspace=\"15\"></img></a></center></td></tr>"));

        css = css.section("/* alarabiya.net */", 1, 1);
        html +=  css + "</style></head><body><table border=\"0\" cellspacing=\"0\" cellpadding=\"0\" height=\"0\" width=\"330\" dir=\"ltr\"><tr>"
                 + out + "</pbody></p></td></tr></table></body></html>";
    }

    else if(m_currentUrl.contains(REUTERS))
    {
        // Get image link
        QString bout = out;
        QString img;

        QString searchstring = "launchArticleSlideshow();\"><";
        int pos1 = bout.indexOf(searchstring,0);
        int pos2 = bout.indexOf("alt=\"Photo\"",pos1);
        QString text = bout.remove(pos2,bout.size());
        text = bout.remove(0,pos1+searchstring.size());

        img = "<div id=\"articlePhoto\"><img src=\"http://ara.reuters.com" + text.section("\"", 1, 1) + "\" border=\"0\" alt=\"Photo\"/></div>";
        //qDebug () << img;

        if(img.contains("resources/r/")) // If this article contain image
            out.replace("<span id=\"midArticle_start\"></span>", "<span id=\"midArticle_start\"></span>" + img);

        //out = out.remove(out.indexOf("<input name=\"CurrentSize\" id=\"CurrentSize\" type=\"hidden\" />"), 693); // 703
        out = out.remove(out.indexOf("<input name=\"CurrentSize\" id=\"CurrentSize\" type=\"hidden\" />"),
                                   (out.indexOf("<div id=\"resizeableText\">"))-out.indexOf("<input name=\"CurrentSize\" id=\"CurrentSize\" type=\"hidden\" />"));

        out = out.remove(out.indexOf("<p id=\"copyrightNotice\" class=\"copyright\">",50), out.size());

        //if(out.contains("ableText\">"))
            //out.replace("ableText\">", "");

        css = css.section("/* reuters.com */", 1, 1);
        html +=  css + "</style></head><body>" + out + "</div></div></body></html>";
    }

    else if(m_currentUrl.contains(ELKHABAR))
    {
        css = css.section("/* elkhabar.com */", 1, 1);
        html +=  css + "</style></head><body><table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"15\">"
                 + out + "</body></html>";
    }

    else if(m_currentUrl.contains(AFP))
    {
        /*QString flashString = "<div class=\"article-header-video\">";

        if(out.contains(flashString))
        {
            out = out.remove(out.indexOf(flashString),
                               (out.indexOf("</object></div>")+30)-out.indexOf(flashString));
        }*/

        css = css.section("/* france24.com */", 1, 1);
        html +=  css + "</style></head><div id=\"content-area\"><div class=\"node article article-photo\">"
                 "<div class=\"article-header\"><div class=\"article-header-infos\"><div class=\"article-header-title\">"
                 + out + "</body></html>";
    }

    else if(m_currentUrl.contains(XINHUA))
    {
        QString jsString = "<table width=\"105\" border=\"0\" align=\"right\" cellpadding=\"0\" cellspacing=\"0\" class=\"taille\">";

        out = out.remove(out.indexOf(jsString)-11,
                           (out.indexOf("height=\"11\" border=\"0\" /></a></td>")+85)-out.indexOf(jsString));

        if(out.contains("<div width=\"100%\">")) // Remove next pages
        {
            out = out.remove(out.indexOf("<div width=\"100%\">"),
                             (out.indexOf("</script>")+9)-out.indexOf("<div width=\"100%\">"));
        }

        css = css.section("/* news.cn */", 1, 1);
        html +=  css + "</style></head><body><table width=\"615\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\">"
                 + out + "</body></html>";
    }

    else if(m_currentUrl.contains(BBC_OLD))
    {
        css = css.section("/* bbc.co.uk/hi */", 1, 1);
        html +=  css + "</style></head><body><div class=\"storybody\">"
                 + out + "</div></body></html>";
    }

    else if(m_currentUrl.contains(BBC))
    {
        QString flashString = "<div class=\"module align-center module-emp448\">";

        if(out.contains(flashString))
        {
            out = out.remove(out.indexOf(flashString),
                               (out.indexOf("<p class=\"ingress\"></p>"))-out.indexOf(flashString));
        }

        css = css.section("/* bbc.co.uk */", 1, 1);
        html +=  css + "</style></head><body><div class=\" g-w16 g-first\">"
                 + out + "</div></body></html>";
    }

    //qDebug() << html;

    file.close();
    emit newFormatedArticle(html);
}

void HttpHandler::abort()
{
    aborted = true;

    //if(networkReply->isRunning())
    if(isRunning)
        networkReply->abort();
}

void HttpHandler::errorSlot(QNetworkReply::NetworkError errorEnum)
{
    if(aborted)
        return;

    error = true;
    Q_UNUSED(errorEnum);
    QNetworkReply *r = qobject_cast<QNetworkReply*>(sender());

    qDebug() << "HttpHandler error:" << r->errorString();

    retriesNbr++;

    //qDebug() << retriesNbr << settings.maxRetries;

    if(retriesNbr <= settings.maxRetries)
    {
        qDebug() << "HttpHandler retries" << retriesNbr;
        get(m_currentUrl, m_formated);
        return;
    }

    retriesNbr = 0;
    error = false; // To pass

    //emit networkError(r->errorString());
}

void HttpHandler::updateTime()
{
    timer->stop();
    qDebug() << "HttpHandler timeout" << timer->interval()/1000 << "secs";
    //timeout = true;
    abort();
    get(m_currentUrl, m_formated);
}
