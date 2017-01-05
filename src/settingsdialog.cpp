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


#include "settingsdialog.h"

SettingsDialog::SettingsDialog()
{
    tabWidget = new QTabWidget;

    fetchNothingRadioBtn = new QRadioButton(tr("Ne rien télécharger"));
    fetchAllRadioBtn = new QRadioButton(tr("Télécharger tous les articles au démarrage"));
    fetchLatestRadioBtn = new QRadioButton(tr("Télécharger les derniers articles au démarrage"));
    fetchCorruptRadioBtn = new QRadioButton(tr("Télécharger les articles corrompus au démarrage"));

    QVBoxLayout *startLayout = new QVBoxLayout;
    startLayout->addWidget(fetchNothingRadioBtn);
    startLayout->addWidget(fetchAllRadioBtn);
    startLayout->addWidget(fetchLatestRadioBtn);
    startLayout->addWidget(fetchCorruptRadioBtn);

    startGroupBox = new QGroupBox(tr("Démarrage"));
    startGroupBox->setLayout(startLayout);

    viewInHTML = new QRadioButton(tr("Toujours afficher les articles en HTML (depuis le site)"));
    viewInText = new QRadioButton(tr("Afficher les articles en HTML s'ils ne sont pas encore téléchargés"));

    QVBoxLayout *viewLayout = new QVBoxLayout;
    viewLayout->addWidget(viewInText);
    viewLayout->addWidget(viewInHTML);

    articlesViewGroupBox = new QGroupBox(tr("Affichage des articles"));
    articlesViewGroupBox->setLayout(viewLayout);

    saveCatArt = new QCheckBox(tr("Sauvegarder le dernier article et la dernière catégorie avant de fermer"));
    exitAfterFetch = new QCheckBox(tr("Fermer Stories Crawler lorsque les téléchargements sont terminés"));
    enablePluginsCheckBox = new QCheckBox(tr("Activer les plugins (Flash...)"));

    QVBoxLayout *generalLayout = new QVBoxLayout;
    generalLayout->addWidget(startGroupBox);
    generalLayout->addWidget(articlesViewGroupBox);
    generalLayout->addWidget(saveCatArt);
    generalLayout->addWidget(exitAfterFetch);
    generalLayout->addWidget(enablePluginsCheckBox);
    generalLayout->addStretch();

    generalPage = new QWidget;
    generalPage->setLayout(generalLayout);

    //------------------------------

    ltrRadioBtn = new QRadioButton(tr("de gauche à droite"));
    rtlRadioBtn = new QRadioButton(tr("de droite à gauche"));

    QVBoxLayout *appearanceVLayout = new QVBoxLayout;
    appearanceVLayout->addWidget(rtlRadioBtn);
    appearanceVLayout->addWidget(ltrRadioBtn);

    appDirectionGroupBox = new QGroupBox(tr("Direction de l'application"));
    appDirectionGroupBox->setLayout(appearanceVLayout);

    fontComboBox = new QFontComboBox;
    sizeComboBox = new QComboBox;
    styleComboBox = new QComboBox;

    QFormLayout *fontLayout = new QFormLayout;
    fontLayout->addRow(tr("Famille:"), fontComboBox);
    fontLayout->addRow(tr("Taille:"), sizeComboBox);
    fontLayout->addRow(tr("Style:"), styleComboBox);

    fontGroupBox = new QGroupBox(tr("Police des articles"));
    fontGroupBox->setLayout(fontLayout);

    findStyles(fontComboBox->currentFont());
    findSizes(fontComboBox->currentFont());

    connect(fontComboBox, SIGNAL(currentFontChanged(QFont)), this, SLOT(findStyles(QFont)));
    connect(fontComboBox, SIGNAL(currentFontChanged(QFont)), this, SLOT(findSizes(QFont)));

    notFetchedBtn = new QToolButton();
    fetchArticlesBtn = new QToolButton();
    fetchTitlesBtn = new QToolButton();

    QFormLayout *colorLayout = new QFormLayout;
    colorLayout->addRow(tr("Article non télécharger:"), notFetchedBtn);
    colorLayout->addRow(tr("Téléchargement des sites:"), fetchTitlesBtn);
    colorLayout->addRow(tr("Téléchargement des articles:"), fetchArticlesBtn);

    connect(notFetchedBtn, SIGNAL(clicked()), this, SLOT(colorDialog()));
    connect(fetchTitlesBtn, SIGNAL(clicked()), this, SLOT(colorDialog()));
    connect(fetchArticlesBtn, SIGNAL(clicked()), this, SLOT(colorDialog()));

    colorGroupBox = new QGroupBox(tr("Utiliser des couleurs personnalisées"));
    colorGroupBox->setCheckable(true);
    //colorGroupBox->setChecked(false);
    colorGroupBox->setLayout(colorLayout);

    QStringList langList;
    langList /*<< "Arabic (ar)"*/ << "English (en)" << "French (fr)";

    languageComboBox = new QComboBox;
    languageComboBox->addItems(langList);
    QLabel *languageLabel = new QLabel(tr("Langue:"));

    QHBoxLayout *languageLayout = new QHBoxLayout;
    languageLayout->addWidget(languageLabel);
    languageLayout->addWidget(languageComboBox);
    languageLayout->addStretch();

    QVBoxLayout *appearanceLayout = new QVBoxLayout;
    appearanceLayout->addWidget(appDirectionGroupBox);
    appearanceLayout->addWidget(fontGroupBox);
    appearanceLayout->addWidget(colorGroupBox);
    appearanceLayout->addLayout(languageLayout);
    appearanceLayout->addStretch();

    appearancePage = new QWidget;
    appearancePage->setLayout(appearanceLayout);

    // -------------------------------

    maxCatSpinBox = new QSpinBox;
    maxCatSpinBox->setMaximum(50000);
    maxCatSpinBox->setMinimum(59);
    maxCatSpinBox->setSpecialValueText(tr("illimité"));

    maxAllSpinBox = new QSpinBox;
    maxAllSpinBox->setMaximum(1000000);
    maxAllSpinBox->setMinimum(299);
    maxAllSpinBox->setSpecialValueText(tr("illimité"));

    maxFetchTimeSpinBox = new QSpinBox;
    maxFetchTimeSpinBox->setMaximum(1000000);
    maxFetchTimeSpinBox->setSpecialValueText(tr("illimité"));

    timeoutSpinBox = new QSpinBox;
    timeoutSpinBox->setMaximum(3600);
    timeoutSpinBox->setMinimum(90);

    maxRetriesSpinBox = new QSpinBox;
    maxRetriesSpinBox->setMaximum(30);
    maxRetriesSpinBox->setMinimum(0);

    QFormLayout *maxLayout = new QFormLayout;
    maxLayout->addRow(tr("Maximum des titres à télécharger dans chaque catégorie"), maxCatSpinBox);
    maxLayout->addRow(tr("Maximum des titres à télécharger"), maxAllSpinBox);
    maxLayout->addRow(tr("Arrêter le téléchargement après (seconds)"), maxFetchTimeSpinBox);
    maxLayout->addRow(tr("Timeout de connexion (seconds)"), timeoutSpinBox);
    maxLayout->addRow(tr("Nombre de tentatives pour un URL échoué"), maxRetriesSpinBox);

    maxGroupBox = new QGroupBox(tr("Limite de téléchargement"));
    maxGroupBox->setLayout(maxLayout);

    QStringList items;
    items << tr("Tous") << tr("Les derniers");
    fetchSelectedComboBox = new QComboBox;
    fetchSelectedComboBox->addItems(items);
    fetchSiteComboBox = new QComboBox;
    fetchSiteComboBox->addItems(items);

    QFormLayout *fetchOptionsLayout = new QFormLayout;
    fetchOptionsLayout->addRow(tr("\"Télécharger le sélectionné\" télécharge les articles"),
                               fetchSelectedComboBox);
    fetchOptionsLayout->addRow(tr("\"Télécharger le site\" télécharge les articles"),
                               fetchSiteComboBox);

    fetchOptionsGroupBox = new QGroupBox(tr("Options de téléchargement"));
    fetchOptionsGroupBox->setLayout(fetchOptionsLayout);

    encodingComboBox = new QComboBox;
    findCodecs();

    QFormLayout *encodeLayout = new QFormLayout;
    encodeLayout->addRow(tr("Encodage du corpus"), encodingComboBox);

    QVBoxLayout *crawlingLayout = new QVBoxLayout;
    crawlingLayout->addWidget(maxGroupBox);
    crawlingLayout->addWidget(fetchOptionsGroupBox);
    crawlingLayout->addLayout(encodeLayout);
    crawlingLayout->addStretch();

    crawlingPage = new QWidget;
    crawlingPage->setLayout(crawlingLayout);

    // -------------------------------

    proxyTypeComboBox = new QComboBox;
    proxyTypeComboBox->addItem("Socks5");
    proxyTypeComboBox->addItem("Http");

    proxyPortSpinBox = new QSpinBox;
    proxyPortSpinBox->setMaximum(10000);

    proxyHostNameLineEdit = new QLineEdit;
    proxyUserNameLineEdit = new QLineEdit;
    proxyPasswordLineEdit = new QLineEdit;
    proxyPasswordLineEdit->setEchoMode(QLineEdit::Password);

    QFormLayout *proxyLayout = new QFormLayout;
    proxyLayout->addRow(tr("Type"), proxyTypeComboBox);
    proxyLayout->addRow(tr("Hôte"), proxyHostNameLineEdit);
    proxyLayout->addRow(tr("Port"), proxyPortSpinBox);
    proxyLayout->addRow(tr("Nom d'utilisateur"), proxyUserNameLineEdit);
    proxyLayout->addRow(tr("Mot de passe"), proxyPasswordLineEdit);

    proxyGroupBox = new QGroupBox(tr("Activer le proxy"));
    proxyGroupBox->setCheckable(true);
    proxyGroupBox->setLayout(proxyLayout);

    fromLineEdit = new QLineEdit;

    QFormLayout *fromLayout = new QFormLayout;
    fromLayout->addRow(tr("L'adresse e-mail dans la requête HTTP"), fromLineEdit);

    fromGroupBox = new QGroupBox(tr("Adresse e-mail"));
    fromGroupBox->setCheckable(true);
    fromGroupBox->setLayout(fromLayout);

    QStringList userAgentList;
    userAgentList << "Mozilla/5.0 (U; Linux) AppleWebKit/532.4 (KHTML, like Gecko) StoriesCrawler/0.1"
            << "Mozilla/5.0 (U; Linux i686;) Gecko/20100423 Ubuntu/10.04 (lucid) Firefox/3.6.3"
            << "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)"
            << "Opera/9.62 (X11; Linux i686; U; fr, en_US) Presto/2.1.1";

    userAgentComboBox = new QComboBox;
    userAgentComboBox->setEditable(true);
    userAgentComboBox->setMaxCount(10);
    //userAgentComboBox->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    userAgentComboBox->addItems(userAgentList);

    QLabel *userAgentLabel = new QLabel(tr("Identification du Stories Crawler"));
    userAgentLabel->setBuddy(userAgentComboBox);

    QVBoxLayout *connectionLayout = new QVBoxLayout;
    connectionLayout->addWidget(proxyGroupBox);
    connectionLayout->addWidget(fromGroupBox);
    connectionLayout->addWidget(userAgentLabel);
    connectionLayout->addWidget(userAgentComboBox);
    connectionLayout->addStretch();

    connectionPage = new QWidget;
    connectionPage->setLayout(connectionLayout);

    //---------------------------------

    tabWidget->addTab(generalPage, tr("Général"));
    tabWidget->addTab(appearancePage, tr("Apparence"));
    tabWidget->addTab(crawlingPage, tr("Crawling"));
    tabWidget->addTab(connectionPage ,tr("Connexion"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                          | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    readSettings();

    setLayoutDirection(Qt::LeftToRight);
    setWindowTitle(tr("Préférences"));
    setWindowIcon(QIcon(":/resources/storiescrawler.png"));
}

void SettingsDialog::findStyles(const QFont &font)
{
    QFontDatabase fontDatabase;
    QString currentItem = styleComboBox->currentText();
    styleComboBox->clear();

    QString style;
    foreach (style, fontDatabase.styles(font.family()))
        styleComboBox->addItem(style);

    int styleIndex = styleComboBox->findText(currentItem);

    if (styleIndex == -1)
        styleComboBox->setCurrentIndex(0);
    else
        styleComboBox->setCurrentIndex(styleIndex);
}

void SettingsDialog::findSizes(const QFont &font)
{
    QFontDatabase fontDatabase;
    QString currentSize = sizeComboBox->currentText();
    sizeComboBox->blockSignals(true);
    sizeComboBox->clear();

    int size;
    if(fontDatabase.isSmoothlyScalable(font.family(), fontDatabase.styleString(font)))
    {
        foreach(size, QFontDatabase::standardSizes())
        {
            sizeComboBox->addItem(QVariant(size).toString());
            sizeComboBox->setEditable(true);
        }
    }

    else
    {
        foreach(size, fontDatabase.smoothSizes(font.family(), fontDatabase.styleString(font)))
        {
            sizeComboBox->addItem(QVariant(size).toString());
            sizeComboBox->setEditable(false);
        }
    }

    sizeComboBox->blockSignals(false);

    int sizeIndex = sizeComboBox->findText(currentSize);

    if(sizeIndex == -1)
        sizeComboBox->setCurrentIndex(qMax(0, sizeComboBox->count() / 3));
    else
        sizeComboBox->setCurrentIndex(sizeIndex);
}

void SettingsDialog::colorDialog()
{
    QToolButton *toolBtn = qobject_cast<QToolButton *>(sender());

    QColor oldColor;

    if(toolBtn == notFetchedBtn)
        oldColor = notFetchedColor;
    else if(toolBtn == fetchArticlesBtn)
        oldColor = fetchArticlesColor;
    else if(toolBtn == fetchTitlesBtn)
        oldColor = fetchTitlesColor;

    QColor newColor = QColorDialog::getColor(oldColor);
    if (!newColor.isValid())
        return;

    if(toolBtn == notFetchedBtn)
        notFetchedColor = newColor;
    else if(toolBtn == fetchArticlesBtn)
        fetchArticlesColor = newColor;
    else if(toolBtn == fetchTitlesBtn)
        fetchTitlesColor = newColor;

    QPixmap pixmap(QSize(32,32));
    pixmap.fill(newColor);
    toolBtn->setIcon(pixmap);

    //toolBtn->setStyleSheet("background-color: "+col.name());
}

void SettingsDialog::findCodecs()
{
    QMap<QString, QTextCodec *> codecMap;
    QRegExp iso8859RegExp("ISO[- ]8859-([0-9]+).*");

    foreach (int mib, QTextCodec::availableMibs())
    {
        QTextCodec *codec = QTextCodec::codecForMib(mib);

        QString sortKey = codec->name().toUpper();
        int rank;

        if (sortKey.startsWith("UTF-8"))
            rank = 1;

        else if (sortKey.startsWith("UTF-16"))
            rank = 2;

        else if (iso8859RegExp.exactMatch(sortKey))
        {
            if (iso8859RegExp.cap(1).size() == 1)
                rank = 3;
            else
                rank = 4;
        }
        else
            rank = 5;

        sortKey.prepend(QChar('0' + rank));

        codecMap.insert(sortKey, codec);
    }

    encodingComboBox->clear();

    foreach (QTextCodec *codec, codecMap.values())
        encodingComboBox->addItem(codec->name(), codec->mibEnum());

    //qDebug() << encodingComboBox->count();
}

void SettingsDialog::accept()
{
    if(fromGroupBox->isChecked())
    {
        if(!fromLineEdit->text().contains(QRegExp("^[A-Z0-9._%\\-]+@(?:[A-Z0-9\\-]+\\.)+[A-Z]{2,4}$",
                                                  Qt::CaseInsensitive)))
        {
            QMessageBox::critical(this, tr("Erreur"), tr("L'adresse e-mail que vous avez saisie est invalide : ")
                                  +fromLineEdit->text(), QMessageBox::Ok);
            return;
        }
    }

    writeSettings();

    QMessageBox::information(this, tr("Information"), tr("Certain modifications nécessite le redémarrage de "
                                                         "l'application pour être correctement prise en compte."),
                             QMessageBox::Ok);

    QDialog::accept();
}

void SettingsDialog::readSettings()
{
    QSettings settings("PFE", "storiescrawler");

    fetchNothingRadioBtn->setChecked(settings.value("Startup/FetchNothingOnStartUp", true).toBool());
    fetchAllRadioBtn->setChecked(settings.value("Startup/FetchAllOnStartUp", false).toBool());
    fetchLatestRadioBtn->setChecked(settings.value("Startup/FetchLatestOnStartUp", false).toBool());
    fetchCorruptRadioBtn->setChecked(settings.value("Startup/FetchCorruptOnStartUp", false).toBool());

    viewInHTML->setChecked(settings.value("ViewInHTML", false).toBool());
    viewInText->setChecked(settings.value("ViewInText", true).toBool());

    saveCatArt->setChecked(settings.value("SaveCatArt", true).toBool());
    exitAfterFetch->setChecked(settings.value("ExitAfterFetch", false).toBool());
    enablePluginsCheckBox->setChecked(settings.value("EnablePlugins", true).toBool());

    ltrRadioBtn->setChecked(settings.value("Gui/LTR", true).toBool());
    rtlRadioBtn->setChecked(settings.value("Gui/RTL", false).toBool());

    QString size = settings.value("Appearance/FontSize", sizeComboBox->currentText()).toString();
    QString style = settings.value("Appearance/FontStyle", styleComboBox->currentText()).toString();
    sizeComboBox->setCurrentIndex(sizeComboBox->findText(size));
    styleComboBox->setCurrentIndex(styleComboBox->findText(style));
    fontComboBox->setCurrentFont(qVariantValue<QFont>(settings.value("Appearance/FontFamily")));

    colorGroupBox->setChecked(settings.value("Appearance/ColorGroupBox", false).toBool());
    /*notFetchedBtn->setStyleSheet("background-color: "
                                 +settings.value("Appearance/NotFetchedColor", Qt::gray).toString());
    fetchArticlesBtn->setStyleSheet("background-color: "
                                    +settings.value("Appearance/FetchArticlesColor", Qt::red).toString());
    fetchTitlesBtn->setStyleSheet("background-color: "
                                  +settings.value("Appearance/FetchTitlesColor", Qt::blue).toString());
                                  */
    QPixmap pixmap(QSize(32,32));
    notFetchedColor = settings.value("Appearance/NotFetchedColor", Qt::gray).toString();
    pixmap.fill(notFetchedColor);
    notFetchedBtn->setIcon(pixmap);

    fetchArticlesColor = settings.value("Appearance/FetchArticlesColor", Qt::red).toString();
    pixmap.fill(fetchArticlesColor);
    fetchArticlesBtn->setIcon(pixmap);

    fetchTitlesColor = settings.value("Appearance/FetchTitlesColor", Qt::blue).toString();
    pixmap.fill(fetchTitlesColor);
    fetchTitlesBtn->setIcon(pixmap);

    languageComboBox->setCurrentIndex(settings.value("Appearance/Language", 1).toInt());

    maxCatSpinBox->setValue(settings.value("Crawling/MaxFetchCat", 59).toInt());
    maxAllSpinBox->setValue(settings.value("Crawling/MaxFetchAll", 299).toInt());
    maxFetchTimeSpinBox->setValue(settings.value("Crawling/MaxFetchTime", 0).toInt());
    timeoutSpinBox->setValue(settings.value("Crawling/Timeout", 300).toInt());
    maxRetriesSpinBox->setValue(settings.value("Crawling/MaxRetries", 3).toInt());

    fetchSelectedComboBox->setCurrentIndex(settings.value("Crawling/FetchSelectedOptions", 0).toInt());
    fetchSiteComboBox->setCurrentIndex(settings.value("Crawling/FetchSiteOptions", 0).toInt());

    encodingComboBox->setCurrentIndex(settings.value("Crawling/Codec", 0).toInt());

    QStringList userAgentList = settings.value("Connection/UserAgentList").toStringList();
    if(!userAgentList.isEmpty())
    {
        userAgentComboBox->clear();
        userAgentComboBox->addItems(userAgentList);
    }

    proxyGroupBox->setChecked(settings.value("Proxy/Enabled", false).toBool());
    proxyTypeComboBox->setCurrentIndex(settings.value("Proxy/Type", 0).toInt());
    proxyHostNameLineEdit->setText(settings.value("Proxy/Host").toString());
    proxyPortSpinBox->setValue(settings.value("Proxy/Prot", 1080).toInt());
    proxyUserNameLineEdit->setText(settings.value("Proxy/UserName").toString());
    proxyPasswordLineEdit->setText(settings.value("Proxy/Password").toString());

    fromGroupBox->setChecked(settings.value("Connection/UseFromEmail", false).toBool());
    fromLineEdit->setText(settings.value("Connection/EmailAddress").toString());

    userAgentComboBox->setCurrentIndex(settings.value("Connection/UserAgent", 0).toInt());
}

void SettingsDialog::writeSettings()
{
    QSettings settings("PFE", "storiescrawler");

    settings.setValue("Startup/FetchNothingOnStartUp", fetchNothingRadioBtn->isChecked());
    settings.setValue("Startup/FetchAllOnStartUp", fetchAllRadioBtn->isChecked());
    settings.setValue("Startup/FetchLatestOnStartUp", fetchLatestRadioBtn->isChecked());
    settings.setValue("Startup/FetchCorruptOnStartUp", fetchCorruptRadioBtn->isChecked());

    settings.setValue("SaveCatArt", saveCatArt->isChecked());
    settings.setValue("ExitAfterFetch",exitAfterFetch->isChecked());
    settings.setValue("EnablePlugins", enablePluginsCheckBox->isChecked());

    settings.setValue("ViewInHTML", viewInHTML->isChecked());
    settings.setValue("ViewInText", viewInText->isChecked());

    settings.setValue("Gui/LTR", ltrRadioBtn->isChecked());
    settings.setValue("Gui/RTL", rtlRadioBtn->isChecked());

    settings.setValue("Appearance/FontFamily", fontComboBox->currentFont());
    settings.setValue("Appearance/FontSize", sizeComboBox->currentText());
    settings.setValue("Appearance/FontStyle", styleComboBox->currentText());

    settings.setValue("Appearance/ColorGroupBox", colorGroupBox->isChecked());
    /*settings.setValue("Appearance/NotFetchedColor", notFetchedBtn->styleSheet().section(" ", 1, 1));
    settings.setValue("Appearance/FetchArticlesColor", fetchArticlesBtn->styleSheet().section(" ", 1, 1));
    settings.setValue("Appearance/FetchTitlesColor", fetchTitlesBtn->styleSheet().section(" ", 1, 1));*/

    settings.setValue("Appearance/NotFetchedColor", notFetchedColor.name());
    settings.setValue("Appearance/FetchArticlesColor", fetchArticlesColor.name());
    settings.setValue("Appearance/FetchTitlesColor", fetchTitlesColor.name());

    settings.setValue("Appearance/Language", languageComboBox->currentIndex());

    settings.setValue("Crawling/MaxFetchCat", maxCatSpinBox->value());
    settings.setValue("Crawling/MaxFetchAll", maxAllSpinBox->value());
    settings.setValue("Crawling/MaxFetchTime", maxFetchTimeSpinBox->value());  
    settings.setValue("Crawling/Timeout", timeoutSpinBox->value());
    settings.setValue("Crawling/MaxRetries", maxRetriesSpinBox->value());

    settings.setValue("Crawling/FetchSelectedOptions", fetchSelectedComboBox->currentIndex());
    settings.setValue("Crawling/FetchSiteOptions", fetchSiteComboBox->currentIndex());

    int mib = encodingComboBox->itemData(encodingComboBox->currentIndex()).toInt();
    settings.setValue("Crawling/MibCodec", mib);
    settings.setValue("Crawling/Codec", encodingComboBox->currentIndex());

    settings.setValue("Proxy/Enabled", proxyGroupBox->isChecked());
    settings.setValue("Proxy/Type", proxyTypeComboBox->currentIndex());
    settings.setValue("Proxy/Host", proxyHostNameLineEdit->text());
    settings.setValue("Proxy/Prot", proxyPortSpinBox->value());
    settings.setValue("Proxy/UserName", proxyUserNameLineEdit->text());
    settings.setValue("Proxy/Password", proxyPasswordLineEdit->text());

    settings.setValue("Connection/UseFromEmail", fromGroupBox->isChecked());
    settings.setValue("Connection/EmailAddress", fromLineEdit->text());

    QStringList userAgentList;
    for(int i = 0; i < userAgentComboBox->count(); ++i)
            userAgentList.append(userAgentComboBox->itemText(i));

    settings.setValue("Connection/UserAgentList", userAgentList);
    settings.setValue("Connection/UserAgent", userAgentComboBox->currentIndex());
}
