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


#ifndef DELETEDIALOG_H
#define DELETEDIALOG_H

#include "data.h"

#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDirIterator>
#include <QMessageBox>
#include <QDateEdit>
#include <QProgressDialog>
#include <QGroupBox>

class DeleteDialog : public QDialog
{
    Q_OBJECT

public:
    DeleteDialog();
    void setModelList(QList<QStandardItemModel*> modelList);

public slots:
    void seedReceived(QString category, QString catUrl, QString id, QString site, QString siteUrl, QString path);
    void curIndexChanged(int index);
    void deleteSlot();

signals:
    void updateCount(int cat);
    void dataChanged(bool changed);

private:
    void setupSites();
    void updateModelList();
    void deleteFiles(const QFileInfoList &files);

private:
    QLabel *sitesLabel;
    QLabel *catLabel;
    QLabel *warningLabel;
    QLabel *fromLabel;
    QLabel *toLabel;
    QComboBox *sitesComboBox;
    QComboBox *catComboBox;
    QPushButton *closeButton;
    QPushButton *deleteButton;
    QDateEdit *fromDateEdit;
    QDateEdit *toDateEdit;
    QGroupBox *filterDateGroupBox;

    QHBoxLayout *buttonLayout;
    QVBoxLayout *mainLayout;

    QList<QStandardItemModel*> catList;
    QList<QStandardItemModel*> m_modelList;

    QDir currentDir;

    int currentSite;
    int currentCat;
    int curArtCat;

    QStringList pathsList;
    QList<QStringList> rowstoRemove;

    Data *data;
};

#endif // DELETEDIALOG_H
