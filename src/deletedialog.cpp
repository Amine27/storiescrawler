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


#include "deletedialog.h"
#include "constants.h"

DeleteDialog::DeleteDialog()
{
    sitesLabel = new QLabel(tr("Site:"));
    sitesComboBox = new QComboBox();
    sitesComboBox->addItem("Tous");
    connect(sitesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(curIndexChanged(int)));

    catLabel = new QLabel(tr("Catégorie:"));
    catComboBox = new QComboBox();
    catComboBox->setEnabled(false);
    catComboBox->addItem(tr("Tous"));

    fromDateEdit = new QDateEdit;
    fromDateEdit->setDate(QDate::currentDate());
    //fromDateEdit->setDate(QDate(2010, 04, 27));
    fromDateEdit->setMaximumDate(QDate::currentDate());
    fromDateEdit->setDisplayFormat("yyyy/MM/dd");
    fromLabel = new QLabel(tr("entre:"));
    fromLabel->setBuddy(fromDateEdit);

    toDateEdit = new QDateEdit;
    toDateEdit->setDate(QDate::currentDate());
    //toDateEdit->setDate(QDate(2010, 04, 27));
    toDateEdit->setMaximumDate(QDate::currentDate());
    toDateEdit->setDisplayFormat("yyyy/MM/dd");
    toLabel = new QLabel(tr("et:"));
    toLabel->setBuddy(toDateEdit);

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(fromLabel);
    hLayout->addWidget(fromDateEdit);
    hLayout->addWidget(toLabel);
    hLayout->addWidget(toDateEdit);

    filterDateGroupBox = new QGroupBox(tr("Filtrage par date"));
    filterDateGroupBox->setCheckable(true);
    filterDateGroupBox->setChecked(false);
    filterDateGroupBox->setLayout(hLayout);

    warningLabel = new QLabel(tr("<b>Attention</b>, les articles supprimés sont définitivement perdus."));

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(sitesLabel, 0, 0);
    gridLayout->addWidget(sitesComboBox, 0, 1, 1, 2);
    gridLayout->addWidget(catLabel, 1, 0);
    gridLayout->addWidget(catComboBox, 1, 1, 1, 2);
    //gridLayout->addWidget(fromLabel, 2, 0, 1, 1);
    //gridLayout->addWidget(fromDateEdit, 2, 1, 1, 1);
    //gridLayout->addWidget(toLabel, 2, 2, 1, 1);
    //gridLayout->addWidget(toDateEdit, 2, 3, 1, 1);
    //gridLayout->addWidget(warningLabel, 3, 0, 3, 0);

    deleteButton = new QPushButton(QIcon(":/resources/edit-delete.png"), tr("&Supprimer"), this);
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteSlot()));

    closeButton = new QPushButton(QIcon(":/resources/dialog-close.png"), tr("&Fermer"), this);
    closeButton->setFocus();
    connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));

    buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(closeButton);
    buttonLayout->setAlignment(Qt::AlignRight);

    mainLayout = new QVBoxLayout;
    mainLayout->addLayout(gridLayout);
    mainLayout->addWidget(filterDateGroupBox);
    mainLayout->addWidget(warningLabel);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
    setWindowTitle(tr("Purger le corpus"));
    setWindowIcon(QIcon(":/resources/storiescrawler.png"));
    setLayoutDirection(Qt::LeftToRight);

    setupSites();
}

void DeleteDialog::setupSites()
{
    currentSite = -1;
    currentCat = 0;

    data = new Data;

    connect(data, SIGNAL(newSeed(QString, QString, QString, QString, QString, QString)),
            this, SLOT(seedReceived(QString, QString, QString, QString, QString, QString)));

    data->readSeeds();
}

void DeleteDialog::seedReceived(QString category, QString catUrl, QString id, QString site, QString siteUrl, QString path)
{
    Q_UNUSED(catUrl);

    if (!site.isEmpty()) // Parent
    {
        sitesComboBox->addItem(site);
        currentSite++;

        QStandardItemModel *list = new QStandardItemModel(0, 4);
        int row = list->rowCount();
        list->insertRow(row);
        list->setData(list->index(row, 0), tr("Tous"));

        catList.append(list);
    }

    int row = catList.at(currentSite)->rowCount();
    catList.at(currentSite)->insertRow(row);
    catList.at(currentSite)->setData(catList.at(currentSite)->index(row, 0), category);
    catList.at(currentSite)->setData(catList.at(currentSite)->index(row, 1), id);
    catList.at(currentSite)->setData(catList.at(currentSite)->index(row, 2), siteUrl);
    catList.at(currentSite)->setData(catList.at(currentSite)->index(row, 3), path);

    pathsList << path;
}

void DeleteDialog::setModelList(QList<QStandardItemModel*> modelList)
{
    m_modelList = modelList;
}

void DeleteDialog::curIndexChanged(int index)
{
    if(index == 0)
    {
        catComboBox->setCurrentIndex(0);
        catComboBox->setEnabled(false);
        return;
    }

    catComboBox->setEnabled(true);
    catComboBox->setModel(catList.at(index-1));
}

void DeleteDialog::deleteSlot()
{
    int ret = QMessageBox::warning(this, tr("Supprimer"), tr("Voulez-vous vraiment supprimer le(s)<br />"
                                                             "site(s) sélectionné(s) et effacer son contenu ?"),
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::No);

    if (ret == QMessageBox::No)
        return;

    QString path = "data/";
    curArtCat = 0;
    currentSite = sitesComboBox->currentIndex()-1;
    currentCat = catComboBox->currentIndex();

    int fromCategory = 0;
    int toCategory = m_modelList.size()-1;

    // If a site is selected (not "Tous" label)
    if(currentSite+1 != 0)
    {
        path += catList.at(currentSite)->index(1, 3).data().toString().section('/', 0, 0);

        fromCategory = catList.at(currentSite)->index(1, 1).data().toInt();
        toCategory = fromCategory+catComboBox->count()-2;

        if(currentCat == 0)
        {
            curArtCat = catList.at(currentSite)->index(1, 1).data().toInt();
        }
        else
        {
            curArtCat = catList.at(currentSite)->index(currentCat, 1).data().toInt();
            path = "data/" + catList.at(currentSite)->index(currentCat, 3).data().toString();

            fromCategory = toCategory = catList.at(currentSite)->index(currentCat, 1).data().toInt();
        }
    }

    qDebug() << path << fromCategory << toCategory;

    currentDir = QDir(path);
    QFileInfoList files;
    //QStringList dates;
    QString str, str2;
    QStringList idArticles;

    for(int i=fromCategory; i <= toCategory; ++i)
    {
        QString curPath = pathsList.at(i);

        for(int j=0; j<m_modelList.at(i)->rowCount(); ++j)
        {
            bool status = m_modelList.at(i)->index(j, TITLES_STATUS).data().toBool();
            QString articleId = m_modelList.at(i)->index(j, TITLES_ID).data().toString();
            QDate curDate = m_modelList.at(i)->index(j, TITLES_DATE).data().toDate();

            if(filterDateGroupBox->isChecked())
            {
                if(curDate >= fromDateEdit->dateTime().date() && curDate <= toDateEdit->dateTime().date()
                    && status)
                {
                    //dates << curDate.toString("yyyy/MM/dd");

                    files += QFileInfo(QDir::currentPath() + "/data/" + curPath +"/"+ articleId + ".txt");
                    //qDebug() << curDate << i << files.last().absoluteFilePath();
                    //qDebug() << curDate << fromDateEdit->dateTime().date() << toDateEdit->dateTime().date();

                    idArticles << str.setNum(i) /*<< str2.setNum(j)*/ << articleId;
                    rowstoRemove << idArticles;
                }

            }
            else if(status)
            {
                //dates << curDate.toString("yyyy/MM/dd");

                files += QFileInfo(QDir::currentPath() + "/data/" + curPath +"/"+ articleId + ".txt");
                //qDebug() << i << files.last().absoluteFilePath();
                //qDebug() << m_modelList.at(i)->removeRow(j);

                idArticles << str.setNum(i) /*<< str2.setNum(j)*/ << articleId;
                rowstoRemove << idArticles;
            }

            idArticles.clear();
        }
    }

    /*if(currentSite+1 != 0)
    {
        //QString dirName = catList.at(currentSite)->index(1, 2).data().toString();
        //path += dirName.section(".", 1, 1);

        path += catList.at(currentSite)->index(1, 3).data().toString().section('/', 0, 0);

        if(currentCat == 0)
        {
            curArtCat = catList.at(currentSite)->index(1, 1).data().toInt();
        }
        else
        {
            curArtCat = catList.at(currentSite)->index(currentCat, 1).data().toInt();
            //path += "/" + m_modelList.at(curArtCat)->index(0, 3).data().toString();

            path = "data/" + catList.at(currentSite)->index(currentCat, 3).data().toString();
        }
    }

    qDebug() << path;

    currentDir = QDir(path);

    QFileInfoList files;
    int filesCount = -1;

    QDirIterator it(currentDir.absolutePath(), QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    files = currentDir.entryInfoList(QStringList("*"), QDir::Files | QDir::NoSymLinks);

    while (it.hasNext())
    {
        qDebug() << it.filePath();// << files.size();
        currentDir = QDir(it.next());
        files += currentDir.entryInfoList(QStringList("*"), QDir::Files | QDir::NoSymLinks);
        filesCount--; // Don't evalute count.txt file in every dir
    };

    filesCount += files.size();
    qDebug() << filesCount << path;
    */

    if(!files.isEmpty())
    {
        deleteFiles(files);
        updateModelList();
        warningLabel->setText(tr("%1 article(s) supprimé(s)").arg((files.size() > 0) ? files.size() : 0));
    }

}

void DeleteDialog::deleteFiles(const QFileInfoList &files)
{
    QProgressDialog progressDialog(this);
    progressDialog.setCancelButtonText(tr("&Annuler"));
    progressDialog.setRange(0, files.size());
    progressDialog.setWindowTitle(tr("Supprimer des fichiers"));
    progressDialog.setLayoutDirection(Qt::LeftToRight);

    for (int i = 0; i < files.size(); ++i)
    {
        progressDialog.setValue(i);
        progressDialog.setLabelText(tr("Supprimer le numéro de fichier %1 de %2...")
                                    .arg(i).arg(files.size()));
        qApp->processEvents();

        if (progressDialog.wasCanceled())
            break;

        /*if(filterDateGroupBox->isChecked())
        {
            if(files.at(i).created().date() < fromDateEdit->dateTime().date() ||
               files.at(i).created().date() > toDateEdit->dateTime().date())
            {
                qDebug() << "skip date:" << files.at(i).created().date();
                continue;
            }
        }*/

        QFile file(files.at(i).absoluteFilePath());
        file.remove();
        currentDir.rmpath(files.at(i).absolutePath());
    }
}

void DeleteDialog::updateModelList()
{
    int category;
    QString articleId;

    for(int i=0; i < rowstoRemove.size(); ++i)
    {
        articleId = rowstoRemove.at(i).at(1);
        category = rowstoRemove.at(i).at(0).toInt();

        //qDebug() << articleId << category;

        for(int j=0; j < m_modelList.at(category)->rowCount(); ++j)
        {
            if(m_modelList.at(category)->index(j, TITLES_ID).data().toString() == articleId)
                m_modelList.at(category)->removeRow(j);
        }
    }

    for (int i = 0; i < m_modelList.size(); ++i)
    {
        emit updateCount(i);
    }

    emit dataChanged(true);

    rowstoRemove.clear();

    /*
    qDebug() << currentSite << currentCat << curArtCat;

    if(currentSite+1 == 0 && currentCat == 0)
    {
        //qDebug() << m_modelList.size();
        for (int i = 0; i < m_modelList.size(); ++i)
        {
            m_modelList.at(i)->removeRows(0, m_modelList.at(i)->rowCount());
            emit updateCount(i);
        }
    }

    else if(currentCat == 0)
    {
        for(int i = 0; i < catComboBox->count()-1; ++i)
        {
            //qDebug() << i+curArtCat;
            m_modelList.at(i+curArtCat)->removeRows(0, m_modelList.at(i+curArtCat)->rowCount());
            emit updateCount(i+curArtCat);
        }
    }
    else
    {
        //qDebug() << curArtCat;
        m_modelList.at(curArtCat)->removeRows(0, m_modelList.at(curArtCat)->rowCount());
        emit updateCount(curArtCat);
    }
    */
}
