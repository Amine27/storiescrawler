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


#include "findwidget.h"

FindWidget::FindWidget(QWidget *parent)
    : QWidget(parent)
{
    appPalette = qApp->palette();

    installEventFilter(this);

    toolClose = setupToolButton(QLatin1String(""), QLatin1String(":/resources/dialog-close.png"));
    connect(toolClose, SIGNAL(clicked()), this, SLOT(closeSlot()));

    editFind = new QLineEdit(this);
    editFind->setMinimumSize(QSize(150, 0));
    connect(editFind, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    connect(editFind, SIGNAL(returnPressed()), this, SLOT(findNext()));
    connect(editFind, SIGNAL(textChanged(QString)), this, SLOT(updateButtons()));

    toolNext = setupToolButton(tr("Suivant"), QLatin1String(":/resources/go-down-search.png"));
    connect(toolNext, SIGNAL(clicked()), this, SLOT(findNext()));

    toolPrevious = setupToolButton(tr("Précédent"), QLatin1String(":/resources/go-up-search.png"));
    connect(toolPrevious, SIGNAL(clicked()), this, SLOT(findPrevious()));

    labelWrapped = new QLabel(this);
    labelWrapped->setScaledContents(true);
    //labelWrapped->setTextFormat(Qt::RichText);
    labelWrapped->setMinimumSize(QSize(0, 20));
    labelWrapped->setMaximumSize(QSize(150, 20));
    labelWrapped->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);
    labelWrapped->setText(tr("Recherche à partir de début"));
    labelWrapped->hide();

    QHBoxLayout *hboxLayout = new QHBoxLayout(this);
    hboxLayout->addWidget(toolClose);
    hboxLayout->addWidget(editFind);
    hboxLayout->addWidget(toolNext);
    hboxLayout->addWidget(toolPrevious);
    hboxLayout->addWidget(labelWrapped);

    QSpacerItem *spacerItem = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout->addItem(spacerItem);
    setMinimumWidth(minimumSizeHint().width());

    updateButtons();
}

void FindWidget::show()
{
    QWidget::show();
    editFind->selectAll();
    editFind->setFocus(Qt::ShortcutFocusReason);
}

void FindWidget::setPalette(bool found)
{
    QPalette palette = editFind->palette();
    palette.setColor(QPalette::Active, QPalette::Base, found ? QColor(186, 249, 206)
        : QColor(247, 200, 230));
    editFind->setPalette(palette);
}

void FindWidget::setTextWrappedVisible(bool visible)
{
    labelWrapped->setVisible(visible);
}

void FindWidget::updateButtons()
{
    bool enable = !editFind->text().isEmpty();
    toolNext->setEnabled(enable);
    toolPrevious->setEnabled(enable);
}

void FindWidget::textChanged(const QString &text)
{
    Q_UNUSED(text);

    find(m_webView, true);
}

bool FindWidget::eventFilter(QObject *object, QEvent *e)
{
    if (e->type() == QEvent::KeyPress)
    {
        if ((static_cast<QKeyEvent*>(e))->key() == Qt::Key_Escape)
        {
            hide();
        }
    }

    return QWidget::eventFilter(object, e);
}

QToolButton* FindWidget::setupToolButton(const QString &text, const QString &icon)
{
    QToolButton *toolButton = new QToolButton(this);

    toolButton->setText(text);
    toolButton->setAutoRaise(true);
    toolButton->setIcon(QIcon(icon));
    toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    return toolButton;
}

void FindWidget::setWebView(QWebView *webView)
{
    m_webView = webView;
}

void FindWidget::findNext()
{
    find(m_webView, true);
}

void FindWidget::findPrevious()
{
    find(m_webView, false);
}

void FindWidget::closeSlot()
{
    hide();
}

void FindWidget::find(QWebView *webView, bool forward)
{
    QString ttf = editFind->text();
    bool found = false;

    if (ttf.isEmpty())
    {
        editFind->setPalette(appPalette);
        return;
    }

    found = findInWebPage(webView, forward);

    if (!found && ttf.isEmpty())
        found = true;   // text not found, no need to mark the line edit red...

    if (!isVisible())
        show();

    setPalette(found);
}

bool FindWidget::findInWebPage(QWebView *webView, bool forward)
{
    QString ttf = editFind->text();

    if (webView)
    {
        bool found = false;
        QWebPage::FindFlags options;
        if (!ttf.isEmpty())
        {
            if (!forward)
                options |= QWebPage::FindBackward;

            found = webView->findText(ttf, options);
            setTextWrappedVisible(false);

            if (!found)
            {
                options |= QWebPage::FindWrapsAroundDocument;
                found = webView->findText(ttf, options);

                if (found)
                    setTextWrappedVisible(true);
            }
        }
        // force highlighting of all other matches, also when empty (clear)
        #if QT_VERSION >= 0x040600
        options = QWebPage::HighlightAllOccurrences;
        #endif
        webView->findText(QLatin1String(""), options);
        webView->findText(ttf, options);
        return found;
    }

    return false;
}
