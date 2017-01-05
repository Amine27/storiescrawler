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


#ifndef FINDWIDGET_H
#define FINDWIDGET_H

#include <QWidget>
#include <QApplication>
#include <QKeyEvent>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QToolButton>
#include <QWebView>
#include <QDebug>

class FindWidget : public QWidget
{
    Q_OBJECT
public:
    FindWidget(QWidget *parent = 0);

    void show();
    void find(QWebView *webView, bool forward);
    void setWebView(QWebView *webView);

private slots:
    void updateButtons();
    void textChanged(const QString &text);
    void findNext();
    void findPrevious();
    void closeSlot();

private:
    bool eventFilter(QObject *object, QEvent *e);
    bool findInWebPage(QWebView *webView, bool forward);
    QToolButton* setupToolButton(const QString &text, const QString &icon);
    void setTextWrappedVisible(bool visible);
    void setPalette(bool found);

private:
    QPalette appPalette;

    QLineEdit *editFind;
    QLabel *labelWrapped;
    QToolButton *toolClose;
    QToolButton *toolNext;
    QToolButton *toolPrevious;

    QWebView *m_webView;
};

#endif // FINDWIDGET_H
