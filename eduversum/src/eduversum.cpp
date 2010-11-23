/*
 * mainwidow.cpp
 *
 * Copyright (c) 2009 Fabian Wuertz <xadras@sidux.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <QDir>
#include <QTreeWidget>
#include <QTextBrowser>
#include <QProcess>
#include <QSystemTrayIcon>
#include <QFrame>
#include <QDesktopServices>
#include <QUrl>

#include "eduversum.h"
#include "easyXml.h"
#include "settings.h"


//------------------------------------------------------------------------------
//-- init ----------------------------------------------------------------------
//------------------------------------------------------------------------------

Eduversum::Eduversum (QWidget* parent, Qt::WFlags flags): QWidget (parent, flags)
{
    //QTime t1 = QTime::currentTime();

    appdir = Settings::applicationPath();
    alreadyDownloadedThumbmnails = QStringList();
    pm = new PackageManager();
    loadGui();
    loadData();

    //QTime t2 = QTime::currentTime();
    //qDebug() << "Time to load: "+QString::number(t2.msec()-t1.msec()+1000*(t2.second()-t1.second()));
}


void Eduversum::loadGui()
{

    setupUi(this);
    setWindowIcon( QIcon(Settings::iconPath()+"eduversum.svgz") );

    //descriptionTextBrowser->setOpenExternalLinks(TRUE); // open links in a external browser
    connect(categoriesTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),this, SLOT( showCategoryApps() ) );
    connect(treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),this, SLOT( showApp(QTreeWidgetItem*) ) );
    connect(treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this, SLOT( execApp() ) );
    connect(searchLineEdit, SIGNAL(textChanged(const QString)), SLOT(searchApps()));
    connect(comboBox, SIGNAL(currentIndexChanged(int)),this, SLOT( showCategoryApps() ) );
    connect(execPushButton, SIGNAL(clicked()), SLOT(execApp()));
    connect(examplePushButton, SIGNAL(clicked()), SLOT(copyExample()));
    connect(installRemovePushButton, SIGNAL(clicked()), SLOT(installRemove()));
    http = new QHttp;
    connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(httpRequestFinished(int, bool)));

    // help menu
    QMenu *helpMenu = new QMenu(this);
    QAction *helpAction = new QAction(QIcon(":/res/help.svg"),tr("Eduversum Help"), this);
    connect(helpAction, SIGNAL(triggered()), this, SLOT(showHelp()));
    aboutAction = new QAction( QIcon(Settings::iconPath()+"eduversum.svgz"), tr("About Eduversum"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));
    disclaimerAction = new QAction( QPixmap( ":/res/docs.svg"), tr("Disclaimer"), this);
    connect(disclaimerAction, SIGNAL(triggered()), this, SLOT(showDisclaimer()));
    helpMenu->addAction(helpAction);
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(disclaimerAction);
    helpButton->setMenu(helpMenu);

    // tray
    trayIcon = new QSystemTrayIcon(this);
    quitAction = new QAction(QIcon(":/res/exit.svg"), tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(helpAction);
    trayIconMenu->addAction(aboutAction);
    trayIconMenu->addAction(disclaimerAction);
    trayIconMenu->addAction(quitAction);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(Settings::iconPath()+"eduversum.svgz"));
    trayIcon->setVisible(TRUE);
    trayIcon->show();
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    setHelp();
    descriptionTextBrowser->setText( "<h2>"+tr("Welcome")+"</h2>"+help );

    unsetGui();
}


void Eduversum::showAppsPage()
{
    contentStackedWidget->setCurrentIndex(0);
}


void Eduversum::unsetGui()
{
    examplePushButton->hide();
    execPushButton->hide();
    installRemovePushButton->hide();
}


void Eduversum::loadData()
{
    QString language = Settings::language();

    // load apps and apps categories
    apps = new AppLoader();
    // clear categoriesTreeWidget
    while(categoriesTreeWidget->topLevelItemCount() > 0)  {
        categoriesTreeWidget->takeTopLevelItem(0);
    }
    categoriesTreeWidget->insertTopLevelItems(0, apps->importCategories());
    categoriesTreeWidget->sortItems(0, Qt::AscendingOrder);

    // load docs
    docsTextBrowser->setText("<h2>"+tr("Teaching Aids")+"</h2>"+tr("Here you find a collection of teaching aids. To open one, double click on the specific entry."));
    QStringList teachings = QDir( appdir+"/teaching/"+language+"/").entryList( QDir::Files );
    foreach(QString teaching, teachings) {
        QTreeWidgetItem *item = new QTreeWidgetItem(docsTreeWidget);
        QMap<QString,QString> docsList = EasyXml::xmlToList(appdir+"/teaching/"+language+"/"+teaching);
        item->setText( 0,docsList.value("name") );
        item->setText( 2, docsList.value("description") );
        QString titles = docsList.value("title");
        QString paths  = docsList.value("path");
    }

    // load web links
    webTextBrowser->setText("<h2>"+tr("Weblinks")+"</h2>"+tr("Here you find a collection of links, that are relation with the topics linux and education.To visit a link, double click on the specific entry."));
    QString path = appdir+"/weblinks/"+language;
    QStringList values = QStringList() << "title" << "url" << "description";
    webTreeWidget->insertTopLevelItems(0, EasyXml::xmlsToList(path, values));
}


//------------------------------------------------------------------------------
//-- show apps -----------------------------------------------------------------
//------------------------------------------------------------------------------

void Eduversum::showCategoryApps()
{
    treeWidget->show();
    frame0->show();

    // show category descrption
    QString category    = categoriesTreeWidget->currentItem()->text(1);
    QString name        = categoriesTreeWidget->currentItem()->text(0);
    QString description = categoriesTreeWidget->currentItem()->text(2);
    descriptionTextBrowser->clear();
    descriptionTextBrowser->setHtml("<h3>"+name+"</h3>"+description);

    // add items to treeWidget
    while(treeWidget->topLevelItemCount() > 0)  {
        treeWidget->takeTopLevelItem(0);
    }
    int status = comboBox->currentIndex();
    treeWidget->insertTopLevelItems(0, apps->list(category, status) );

    unsetGui();
}


void Eduversum::searchApps()
{
    // add items to treeWidget
    while(treeWidget->topLevelItemCount() > 0)  {
        treeWidget->takeTopLevelItem(0);
    }
    treeWidget->insertTopLevelItems(0, apps->search(searchLineEdit->text()) );

    unsetGui();
}



void Eduversum::showApp(QTreeWidgetItem* item)
{

    // Show exec button
    QString status = item->text(4);
    QString exec = item->text(5);
    if( status == "notinstalled" or status == "install" or exec == "none") {
        execPushButton->hide();
    } else {
        execPushButton->show();
    }

    // Show install/remove button
    installRemovePushButton->show();
    if( status == "installed" ) {
        installRemovePushButton->setText(tr("Remove"));
        installRemovePushButton->setIcon( QIcon(":/res/remove.png") );
    } else {
        installRemovePushButton->setText(tr("Install"));
        installRemovePushButton->setIcon( QIcon(":/res/add.png") );
    }

    // Show exmaple button
    QString example = item->text(10);
    if( example == "TRUE" ) {
        examplePushButton->show();
    } else {
        examplePushButton->hide();
    }

    // Show description
    QString packageName = item->text(3);
    QString thumbnailPath = apps->thumbnailPath(packageName);
    if( thumbnailPath.isEmpty() && !alreadyDownloadedThumbmnails.contains(packageName)) {
        downloadThumbnail(packageName);
    } else {
        formatAppDescription();
    }
}

void Eduversum::formatAppDescription()
{
    QTreeWidgetItem* item = treeWidget->currentItem();
    QString description;
    QString packageName = item->text(3);

    // name
    description += "<table cellpadding=0 cellspacing=0 style='margin:8px'><tr>";
    description += "<td><img height=48 width=48 src='"+item->text(9)+"'></td>";
    description += "<td valign='middle'><h2 style='margin:8px'>"+item->text(0)+"</h2></td>";
    description += "</tr></table>";

    // description
    description += "<table cellpadding=0 cellspacing=0 style='margin:8px'><tr><td>";
    if( item->text(6).isEmpty() ) {
        QString desc = pm->getPackageDescription(packageName);
        description += desc;
        apps->setDescription(item, desc);
    } else {
        description += item->text(6);
    }
    // homepage
    if(!item->text(7).isEmpty()) {
        description += "<br><br><b>"+tr("Website")+":</b> <a href=\""+item->text(7)+"\">"+item->text(7)+"</a>";
    }
    description += "</td>";
    // thumbnail;
    QString thumbnailPath = apps->thumbnailPath(packageName);
    if( !thumbnailPath.isEmpty() ) {
       description += "<td width='170' align=right><img src='"+thumbnailPath+"'></td>";
    }
    description += "</tr></table>";
    description += "</div>";
    descriptionTextBrowser->setHtml( description );
}


void Eduversum::downloadThumbnail(QString packageName)
{
    setDisabled(true);

    alreadyDownloadedThumbmnails << packageName;
    QUrl url("http://screenshots.debian.net/thumbnail/"+packageName);
    QFileInfo fileInfo(url.path());
    QString fileName = fileInfo.fileName();
    file = new QFile(  QDir::homePath()+"/.cache/eduversum/"+packageName+".png");

    http->setHost(url.host(), url.port() != -1 ? url.port() : 80);
    if (!url.userName().isEmpty()) {
        http->setUser(url.userName(), url.password());
    }

    httpRequestAborted = false;
    httpGetId = http->get(url.path(), file);
}


void Eduversum::httpRequestFinished(int requestId, bool error)
{
    setDisabled(false);

    if (httpRequestAborted) {
       if (file) {
           file->close();
           file->remove();
           delete file;
           file = 0;
       }
       return;
    }
    if (requestId != httpGetId) {return;}
    file->close();
    if (error) {
       file->remove();
       QMessageBox::information(this, tr("HTTP"), tr("Download failed: %1.").arg(http->errorString()));
    }
    delete file;
    file = 0;
    formatAppDescription();
}


//------------------------------------------------------------------------------
//-- button functions ----------------------------------------------------------
//------------------------------------------------------------------------------

void Eduversum::execApp()
{
    QString status = treeWidget->currentItem()->text(4);
    QString exec = treeWidget->currentItem()->text(5);
    if(  status == "notinstalled" or status == "install" or exec == "none" ) {return;}

    QString terminal = treeWidget->currentItem()->text(8);
    if( terminal == "true" or terminal == "yes") {
        exec = "konsole --noclose -e "+exec;
    }

    QStringList arguments = exec.split(" ");
    QString program = arguments[0];
    arguments.removeFirst();

    QProcess *myProcess = new QProcess(this);
    myProcess->start(program, arguments);
}


void Eduversum::showHomepage()
{
    QString homepage = treeWidget->currentItem()->text(7);
    QDesktopServices::openUrl(QUrl(homepage));
}



void Eduversum::copyExample()
{
    if(QMessageBox::question(this, "Beispieldateien", "Sollen die Beispieldateien auf dem Desktop angezeigt werden?", QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
    {
        QString package = treeWidget->currentItem()->text(3);
        package = package.split(" ")[0];
        copyDir("/usr/share/seminarix-samples/"+package, QDir::homePath ()+"/Desktop/"+package );
    }
}


void Eduversum::copyDir(QString sourceDir, QString destinationDir)
{
    if( !QFile::exists( destinationDir ) ) {
        QDir().mkdir(destinationDir);
    }

    QStringList fileList = QDir( sourceDir ).entryList( QDir::Files );
    for ( QStringList::Iterator it = fileList.begin(); it != fileList.end(); ++it ) {
        QFile(sourceDir+"/"+*it).copy( destinationDir+"/"+*it );
    }

    QStringList dirList = QDir( sourceDir ).entryList( QDir::Dirs );
    for( int i = 2; i < dirList.count(); i++) {
        copyDir(sourceDir+"/"+dirList[i], destinationDir+"/"+dirList[i] );
    }
}


//------------------------------------------------------------------------------
//-- teaching -----------------------------------------------------------------
//------------------------------------------------------------------------------

void Eduversum::showDocsPage()
{
    contentStackedWidget->setCurrentIndex(2);
}


void Eduversum::showDoc()
{
    QString homepage = docsTreeWidget->currentItem()->text(1);
    if(!homepage.isEmpty()) {
        QDesktopServices::openUrl(QUrl(homepage));
    }
}


void Eduversum::showDocDescription()
{
    QString title = docsTreeWidget->currentItem()->text(0);
    QString description = docsTreeWidget->currentItem()->text(2);
    if( !description.isEmpty() ) {
        docsTextBrowser->setText("<h2>"+title+"</h2>"+description);
    }
}



//------------------------------------------------------------------------------
//-- web links -----------------------------------------------------------------
//------------------------------------------------------------------------------

void Eduversum::showWeblinkPage()
{
    contentStackedWidget->setCurrentIndex(1);
}


void Eduversum::showWebDescription()
{
    QString title = webTreeWidget->currentItem()->text(0);
    QString description = webTreeWidget->currentItem()->text(2);
    webTextBrowser->setText("<h2>"+title+"</h2>"+description);
}


void Eduversum::showWeblink()
{
    QDesktopServices::openUrl(webTreeWidget->currentItem()->text(1));
}


//------------------------------------------------------------------------------
//-- tray events ---------------------------------------------------------------
//------------------------------------------------------------------------------

void Eduversum::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            if( isHidden() ) {
                show();
            } else {
                hide();
            }
        case QSystemTrayIcon::MiddleClick:
        default:
            ;
    }
}


void Eduversum::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
        QMessageBox::information(this, tr("Systray"), tr("To close the program, chose 'Quit' in the menu of the systray.") );
        hide();
        event->ignore();
    }
}


//------------------------------------------------------------------------------
//-- about ---------------------------------------------------------------------
//------------------------------------------------------------------------------

void Eduversum::showAbout()
{
    if( isHidden() ) {
        trayIcon->showMessage ( tr("About Eduversum"), About::text() , QSystemTrayIcon::Information, 20000 );
    } else {
        QMessageBox::information(this, tr("About Eduversum"), About::text() );
    }
}


//------------------------------------------------------------------------------
//-- help ---------------------------------------------------------------------
//------------------------------------------------------------------------------

void Eduversum::setHelp()
{
    help = tr("Eduversum is a tool with which you can easily install or uninstall educational programs (Installation: click on the empty box next to the application's name, Unistallation: click the box to remove the tick). For this purpose, the administrator password is required, because software installation or uninstallation is, as a rule, system-wide work.");
}


void Eduversum::showHelp()
{
    QString locale = Settings::language();
    if( locale != "de" ) {locale = "en";}
    QDesktopServices::openUrl(QUrl("/usr/share/doc/eduversum/benutzung-eduversum-"+locale+".html"));
}


void Eduversum::showDisclaimer()
{
    QString locale = Settings::language();
    if( locale != "de" ) {locale = "en";}
    QDesktopServices::openUrl(QUrl(appdir+"weblinks/disclaimer-eduversum-"+locale+".html"));
}


//------------------------------------------------------------------------------
//-- install/remove ------------------------------------------------------------
//------------------------------------------------------------------------------


void Eduversum::installRemove()
{
    QStringList packages = treeWidget->currentItem()->text(3).split(" ");

    setDisabled ( TRUE );
    PackageManager *pm = new PackageManager();
    connect(pm, SIGNAL(finished()), this, SLOT(installRemoveFinished()));
    if(installRemovePushButton->text() == tr("Install")) {
        pm->installPackages(packages);
    } else {
        pm->removePackages(packages);
    }
}


void Eduversum::installRemoveFinished()
{
    setDisabled ( FALSE );
    apps = new AppLoader();
    QTreeWidgetItem *item = treeWidget->currentItem();
    showCategoryApps();
    item = apps->find(item->text(1) );
    treeWidget->setCurrentItem(item);
    showApp(item);
}
