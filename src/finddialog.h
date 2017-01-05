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


#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include "data.h"
#include "constants.h"

#include <QDialog>
#include <QTreeWidgetItem>
#include <QGroupBox>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDirIterator>
#include <QProgressDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QDateEdit>
#include <QCheckBox>
#include <QMessageBox>
#include <QFileDialog>

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    FindDialog();
    void setModelList(QList<QStandardItemModel*> modelList);

public slots:
    void openFileOfItem(QTreeWidgetItem *item, int column);
    void seedReceived(QString category, QString catUrl, QString id, QString site, QString siteUrl, QString path);
    void curIndexChanged(int index);
    void textChanged(QString str);
    void find();
    void saveAs();

private:
    void setupSites();
    QFileInfoList findFiles(const QFileInfoList &files, const QString &text, QStringList dates);
    void showFiles(const QFileInfoList &files);

private:
    QGroupBox *findGroupBox;
    QGroupBox *resultsGroupBox;
    QGroupBox *filterDateGroupBox;
    QComboBox *textComboBox;
    QLabel *textLabel;
    QLabel *sitesLabel;
    QLabel *catLabel;
    QLabel *filesFoundLabel;
    QLabel *fromLabel;
    QLabel *toLabel;
    QComboBox *sitesComboBox;
    QComboBox *catComboBox;
    QPushButton *findButton;
    QPushButton *saveAsButton;
    QPushButton *closeButton;
    QDateEdit *fromDateEdit;
    QDateEdit *toDateEdit;

    QTreeWidget *filesWidget;

    QHBoxLayout *buttonLayout;
    QVBoxLayout *mainLayout;

    QList<QStandardItemModel*> catList;
    QList<QStandardItemModel*> m_modelList;

    QDir currentDir;

    int currentSite;
    int currentCat;

    QStringList pathsList;
    QStringList dateList;

    Data *data;
};

#endif // FINDDIALOG_H
