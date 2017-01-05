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


#include "finddialog.h"
#include "settings.h"

FindDialog::FindDialog()
{
    textLabel = new QLabel(tr("Text:"));

    textComboBox = new QComboBox();
    textComboBox->setEditable(true);
    connect(textComboBox, SIGNAL(editTextChanged(QString)), this, SLOT(textChanged(QString)));

    sitesLabel = new QLabel(tr("Site:"));
    sitesComboBox = new QComboBox();
    sitesComboBox->addItem("Tous");
    connect(sitesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(curIndexChanged(int)));

    catLabel = new QLabel(tr("Catégorie:"));
    catComboBox = new QComboBox();
    catComboBox->setEnabled(false);
    catComboBox->addItem(tr("Tous"));

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(textLabel, 0, 0);
    gridLayout->addWidget(textComboBox, 0, 1, 1, 2);
    gridLayout->addWidget(sitesLabel, 1, 0);
    gridLayout->addWidget(sitesComboBox, 1, 1, 1, 2);
    gridLayout->addWidget(catLabel, 2, 0);
    gridLayout->addWidget(catComboBox, 2, 1, 1, 2);

    setupSites();

    findGroupBox = new QGroupBox(tr("Rechercher"), this);
    findGroupBox->setLayout(gridLayout);

    fromDateEdit = new QDateEdit;
    fromDateEdit->setDate(QDate::currentDate());
    fromDateEdit->setMaximumDate(QDate::currentDate());
    fromDateEdit->setDisplayFormat("yyyy/MM/dd");
    fromLabel = new QLabel(tr("entre:"));
    fromLabel->setBuddy(fromDateEdit);

    toDateEdit = new QDateEdit;
    toDateEdit->setDate(QDate::currentDate());
    toDateEdit->setMaximumDate(QDate::currentDate());
    toDateEdit->setDisplayFormat("yyyy/MM/dd");
    toLabel = new QLabel(tr("et:"));
    toLabel->setBuddy(toDateEdit);

    QHBoxLayout *filterDateLayout = new QHBoxLayout;
    filterDateLayout->addWidget(fromLabel);
    filterDateLayout->addWidget(fromDateEdit);
    filterDateLayout->addWidget(toLabel);
    filterDateLayout->addWidget(toDateEdit);
    filterDateLayout->addStretch();

    filterDateGroupBox = new QGroupBox(tr("Articles créés:"), this);
    filterDateGroupBox->setCheckable(true);
    filterDateGroupBox->setChecked(false);
    filterDateGroupBox->setLayout(filterDateLayout);

    QStringList labels;
    labels << tr("Titre") << tr("Fichier") << tr("Date") << tr("Chemin");

    filesWidget = new QTreeWidget;
    filesWidget->setRootIsDecorated(false);
    filesWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    filesWidget->setSortingEnabled(true);
    filesWidget->sortByColumn(1);
    filesWidget->setWordWrap(false);
    filesWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    filesWidget->setAlternatingRowColors(true);
    filesWidget->setHeaderLabels(labels);
    //filesWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(filesWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(openFileOfItem(QTreeWidgetItem*, int)));

    filesFoundLabel = new QLabel;

    QVBoxLayout *filesLayout = new QVBoxLayout;
    filesLayout->addWidget(filesWidget);
    filesLayout->addWidget(filesFoundLabel);

    resultsGroupBox = new QGroupBox(tr("Résultat"), this);
    resultsGroupBox->setLayout(filesLayout);

    findButton = new QPushButton(QIcon(":/resources/edit-find.png"), tr("&Rechercher"), this);
    findButton->setEnabled(false);
    connect(findButton, SIGNAL(clicked()), this, SLOT(find()));

    saveAsButton = new QPushButton(QIcon(":/resources/document-save-as.png"), tr("&Enregistrer sous"), this);
    saveAsButton->setEnabled(false);
    connect(saveAsButton, SIGNAL(clicked()), this, SLOT(saveAs()));

    closeButton = new QPushButton(QIcon(":/resources/dialog-close.png"), tr("&Fermer"), this);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));

    buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(findButton);
    buttonLayout->addWidget(saveAsButton);
    buttonLayout->addWidget(closeButton);
    buttonLayout->setAlignment(Qt::AlignRight);

    mainLayout = new QVBoxLayout;
    mainLayout->addWidget(findGroupBox);
    mainLayout->addWidget(filterDateGroupBox);
    mainLayout->addWidget(resultsGroupBox);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
    setWindowTitle(tr("Rechercher dans le corpus"));
    setWindowIcon(QIcon(":/resources/storiescrawler.png"));
    setFixedSize(500, 450);
    setLayoutDirection(Qt::LeftToRight);
}

void FindDialog::setupSites()
{
    currentSite = -1;
    currentCat = 0;

    data = new Data;

    connect(data, SIGNAL(newSeed(QString, QString, QString, QString, QString, QString)),
            this, SLOT(seedReceived(QString, QString, QString, QString, QString, QString)));

    data->readSeeds();
}

void FindDialog::seedReceived(QString category, QString catUrl, QString id, QString site, QString siteUrl, QString path)
{
    Q_UNUSED(catUrl);
    //Q_UNUSED(id);
    Q_UNUSED(siteUrl);

    if (!site.isEmpty()) // Parent
    {
        sitesComboBox->addItem(site);
        currentSite++;

        QStandardItemModel *list = new QStandardItemModel(0, 3);
        int row = list->rowCount();
        list->insertRow(row);
        list->setData(list->index(row, 0), tr("Tous"));

        catList.append(list);
    }

    int row = catList.at(currentSite)->rowCount();
    catList.at(currentSite)->insertRow(row);
    catList.at(currentSite)->setData(catList.at(currentSite)->index(row, 0), category);
    catList.at(currentSite)->setData(catList.at(currentSite)->index(row, 1), id);
    catList.at(currentSite)->setData(catList.at(currentSite)->index(row, 2), path);

    pathsList << path;
}

void FindDialog::setModelList(QList<QStandardItemModel*> modelList)
{
    m_modelList = modelList;
}

void FindDialog::curIndexChanged(int index)
{
    if(index == 0)
    {
        catComboBox->setEnabled(false);
        return;
    }

    catComboBox->setEnabled(true);
    catComboBox->setModel(catList.at(index-1));
}

void FindDialog::textChanged(QString str)
{
    if(!str.isEmpty())
        findButton->setEnabled(true);

    else
        findButton->setEnabled(false);
}

void FindDialog::find()
{
    QString path = "data/";
    //int curArtCat=0;
    currentSite = sitesComboBox->currentIndex()-1;
    currentCat = catComboBox->currentIndex();

    int fromCategory = 0;
    int toCategory = m_modelList.size()-1;

    // If a site is selected (not "Tous" label)
    if(currentSite+1 != 0)
    {
        path += catList.at(currentSite)->index(1, 2).data().toString().section('/', 0, 0);

        fromCategory = catList.at(currentSite)->index(1, 1).data().toInt();
        toCategory = fromCategory+catComboBox->count()-2;

        // If a category is selected (not "Tous" label)
        if(currentCat != 0)
        {
            path = "data/" + catList.at(currentSite)->index(currentCat, 2).data().toString();

            fromCategory = toCategory = catList.at(currentSite)->index(currentCat, 1).data().toInt();
        }
    }

    qDebug() << path << fromCategory << toCategory;

    currentDir = QDir(path);
    QFileInfoList files;
    QStringList dates;

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
                    dates << curDate.toString("yyyy/MM/dd");

                    files += QFileInfo(QDir::currentPath() + "/data/" + curPath +"/"+ articleId + ".txt");
                    //qDebug() << curDate << i << files.last().absoluteFilePath();
                    //qDebug() << curDate << fromDateEdit->dateTime().date() << toDateEdit->dateTime().date();
                }

            }
            else if(status)
            {
                dates << curDate.toString("yyyy/MM/dd");

                files += QFileInfo(QDir::currentPath() + "/data/" + curPath +"/"+ articleId + ".txt");
                //qDebug() << i << files.last().absoluteFilePath();
            }
        }
    }

    QString text = textComboBox->currentText();

    /*QDirIterator it(currentDir.absolutePath(), QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    files += currentDir.entryInfoList(QStringList("*"), QDir::Files | QDir::NoSymLinks);

    while (it.hasNext())
    {
        //qDebug() << it.filePath() << files.size();
        currentDir = QDir(it.next());
        files += currentDir.entryInfoList(QStringList("*"), QDir::Files | QDir::NoSymLinks);
    };

    qDebug() << currentDir.absolutePath() << files.size() << path;*/

    files = findFiles(files, text, dates);

    qDebug() << files.size();

    showFiles(files);
}

QFileInfoList FindDialog::findFiles(const QFileInfoList &files, const QString &text, QStringList dates)
{
    QProgressDialog progressDialog(this);
    progressDialog.setCancelButtonText(tr("&Annuler"));
    progressDialog.setRange(0, files.size());
    progressDialog.setWindowTitle(tr("Rechercher des fichiers"));
    progressDialog.setLayoutDirection(Qt::LeftToRight);

    QFileInfoList foundFiles;

    dateList.clear();

    Settings settings;
    settings.loadSettings();

    for (int i = 0; i < files.size(); ++i)
    {
        progressDialog.setValue(i);
        progressDialog.setLabelText(tr("Chercher le numéro de fichier %1 de %2...")
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

        if (file.open(QIODevice::ReadOnly))
        {
            QString line;
            QTextStream in(&file);
            //in.setCodec("UTF-8");
            in.setCodec(settings.codec);

            while (!in.atEnd())
            {
                if (progressDialog.wasCanceled())
                    break;
                line = in.readLine();
                if (line.contains(text))
                {
                    foundFiles << files.at(i);
                    dateList << dates.at(i);
                    //qDebug() << currentDir.absoluteFilePath(files[i]);
                    break;
                }
            }
        }
    }
    return foundFiles;
}

void FindDialog::showFiles(const QFileInfoList &files)
{
    Settings settings;
    settings.loadSettings();

    filesWidget->clear();

    for (int i = 0; i < files.size(); ++i)
    {
        QString fileNameItem = files.at(i).fileName();
        //QString fileDate = files.at(i).created().toString("yyyy/MM/dd");
        QString fileDate = dateList.at(i);
        QString filePath = files.at(i).path();
        filePath = filePath.section("data", 1, 1);
        QString fileNameTitle;
        
        QFile file(files.at(i).absoluteFilePath());

        if (file.open(QIODevice::ReadOnly))
        {
            QTextStream in(&file);
            //in.setCodec("UTF-8");
            in.setCodec(settings.codec);
            fileNameTitle = in.readLine();
        }

        QTreeWidgetItem *item = new QTreeWidgetItem(filesWidget);
        item->setText(0, fileNameTitle);
        item->setText(1, fileNameItem);
        item->setText(2, fileDate);
        item->setText(3, filePath);
    }

    filesFoundLabel->setText(tr("%1 article(s) trouvé(s)").arg(files.size()) +
                             tr(" (Double-cliquez sur un article pour l'ouvrir)"));

    if(files.size() > 0)
        saveAsButton->setEnabled(true);
    else
        saveAsButton->setEnabled(false);
}

void FindDialog::openFileOfItem(QTreeWidgetItem * item, int column)
{
    Q_UNUSED(column);

    QString fullPath = QDir::current().absolutePath()+"/data"+item->text(3)+"/"+item->text(1);

    if(QFile(fullPath).exists())
        QDesktopServices::openUrl(QUrl::fromLocalFile(fullPath));

    else
        QMessageBox::critical(this, tr("Fichier introuvable"),
                              tr("Le fichier <br /><b>%1</b><br /> n'existe pas.").arg(fullPath));

    // Detect item category id
    /*QString path = item->text(3).remove(0, 1);

    for(int i=0; i < catList.size(); ++i)
    {
        for(int j=1; j < catList.at(i)->rowCount(); ++j)
        {
            if (catList.at(i)->index(j, 2).data().toString() == path)
                qDebug() << catList.at(i)->index(j, 1).data().toInt();

            qDebug() << path << catList.at(i)->index(j, 2).data().toString();
        }

    }*/
}

void FindDialog::saveAs()
{
    QFile file;
    QDir newDir;
    QString dir = QFileDialog::getExistingDirectory(this, tr("Enregistrer sous"),
                                                    QDir::currentPath(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks
                                                    /*| QFileDialog::DontUseNativeDialog*/);

    if (dir.isEmpty())
        return;

    QTreeWidgetItemIterator it(filesWidget);
    while (*it)
    {
        if(!QDir(dir+(*it)->text(3)).exists())
            newDir.mkpath(dir+(*it)->text(3));

        QString fileName = QDir::current().absolutePath()+"/data"+(*it)->text(3)+"/"+(*it)->text(1);
        QString newName = dir+(*it)->text(3)+"/"+(*it)->text(1);

        if(!file.copy(fileName, newName))
        {
            QMessageBox::critical(this, tr("Problème d'écriture"),
                                  tr("Impossible d'écrire le fichier:\n%1.")
                                 .arg(newName));
            return;
        }

        qDebug() << fileName << newName;

        ++it;
    }

    QMessageBox::information(this, tr("Information"),
                             tr("Les résultats ont été enregistrés dans le répertoire:\n%1.")
                             .arg(dir));
}
