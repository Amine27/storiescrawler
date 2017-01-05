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


#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QCheckBox>
#include <QGroupBox>
#include <QComboBox>
#include <QRadioButton>
#include <QDialogButtonBox>
#include <QFontComboBox>
#include <QLabel>
#include <QColorDialog>
#include <QToolButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QSettings>
#include <QTextCodec>
#include <QDebug>

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog();

public slots:
    void accept();
    void findStyles(const QFont &font);
    void findSizes(const QFont &font);
    void colorDialog();

private:
    void findCodecs();

private:
    void readSettings();
    void writeSettings();

private:
    QTabWidget *tabWidget;

    QWidget *generalPage;
    QWidget *appearancePage;
    QWidget *crawlingPage;
    QWidget *connectionPage;

    QGroupBox *startGroupBox;
    QGroupBox *articlesViewGroupBox;

    QGroupBox *appDirectionGroupBox;
    QGroupBox *fontGroupBox;
    QGroupBox *colorGroupBox;

    QGroupBox *maxGroupBox;
    QGroupBox *fetchOptionsGroupBox;

    QGroupBox *proxyGroupBox;
    QGroupBox *fromGroupBox;

    QRadioButton *fetchNothingRadioBtn;
    QRadioButton *fetchAllRadioBtn;
    QRadioButton *fetchLatestRadioBtn;
    QRadioButton *fetchCorruptRadioBtn;
    QRadioButton *viewInHTML;
    QRadioButton *viewInText;

    QCheckBox *saveCatArt;
    QCheckBox *exitAfterFetch;
    QCheckBox *enablePluginsCheckBox;
    QCheckBox *connectIntrnetCheckBox;

    QRadioButton *ltrRadioBtn;
    QRadioButton *rtlRadioBtn;

    QFontComboBox *fontComboBox;
    QComboBox *styleComboBox;
    QComboBox *sizeComboBox;

    QComboBox *languageComboBox;

    QToolButton *notFetchedBtn;
    QToolButton *fetchArticlesBtn;
    QToolButton *fetchTitlesBtn;
    QColor notFetchedColor;
    QColor fetchArticlesColor;
    QColor fetchTitlesColor;

    QSpinBox *maxCatSpinBox;
    QSpinBox *maxAllSpinBox;
    QSpinBox *maxFetchTimeSpinBox;
    QSpinBox *timeoutSpinBox;
    QSpinBox *maxRetriesSpinBox;

    QComboBox *fetchSelectedComboBox;
    QComboBox *fetchSiteComboBox;
    QComboBox *encodingComboBox;

    QComboBox *proxyTypeComboBox;
    QSpinBox *proxyPortSpinBox;
    QLineEdit *proxyHostNameLineEdit;
    QLineEdit *proxyUserNameLineEdit;
    QLineEdit *proxyPasswordLineEdit;

    QLineEdit *fromLineEdit;

    QComboBox *userAgentComboBox;
};

#endif // SETTINGSDIALOG_H
