/*
 * edu.cpp
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

#include <QFile>
#include <QDir>
#include <QXmlStreamReader>
#include <QTextStream>
#include <QDebug>


#include "apploader.h"
#include "easyXml.h"
#include "settings.h"


AppLoader::AppLoader()
{
        language = Settings::language();
        appdir = Settings::applicationPath();
        QDir::home().mkpath(".cache");
        QDir::home().mkpath(".cache/eduversum");
        QDir::home().mkpath(".cache/thumbnails");
        iconloader = new IconLoader();
        importApps();
}


QList<QTreeWidgetItem *> AppLoader::list()
{
        return apps;
}

QList<QTreeWidgetItem *> AppLoader::list(QString category, int status)
{
        QList<QTreeWidgetItem *> output;
        QListIterator<QTreeWidgetItem*> i(apps);
        while (i.hasNext()) {
                QTreeWidgetItem *item = i.next();
                // check category
                if( !item->text(2).contains(category) ) {
                       continue;
                }
                QString appStatus = item->text(4);
                if( status == 1 and appStatus == "notinstalled") { // just installed programms
                        continue;
                } else if( status == 2 && appStatus == "installed") {   // just not installed programms
                       continue;
                }
                output.append(item);
        }
        return output;
}

QList<QTreeWidgetItem *> AppLoader::search(QString searchString)
{
    QList<QTreeWidgetItem *> output;
    QListIterator<QTreeWidgetItem*> i(apps);
    while (i.hasNext()) {
        QTreeWidgetItem *it = i.next();
        if( it->text(1).contains( searchString ) or it->text(6).contains( searchString ) ) {
            output.append(it);
        }
    }
    return output;
}

QTreeWidgetItem* AppLoader::find(QString id)
{
    QListIterator<QTreeWidgetItem*> i(apps);
    while (i.hasNext()) {
        QTreeWidgetItem *item = i.next();
        if( item->text(1) == id ) {
            return item;
        }
    }
    return false;
}

QString AppLoader::thumbnailPath(QString packageName) {
    if( QFile::exists(appdir+"thumbnails/"+packageName+".png") ){
        return appdir+"thumbnails/"+packageName+".png";
    }
    if( QFile::exists(QDir::homePath()+"/.cache/eduversum/thumbnails/"+packageName+".png") ) {
        return QDir::homePath()+"/.cache/eduversum/thumbnails/"+packageName+".png";
    }
    return "";
}


void AppLoader::changePackageStatus(QTreeWidgetItem* package, QString status)
{
        int at = apps.indexOf(package);
        apps.value(at)->setText(4, status );
}


void AppLoader::setDescription(QTreeWidgetItem* package, QString description)
{
    int at = apps.indexOf(package);
    apps.value(at)->setText(6, description );

    QDir::home().mkpath(".cache/eduversum/description");
    QDir::home().mkpath(".cache/eduversum/description/"+language);
    QString packageName = package->text(3);
    QFile file( QDir::homePath()+"/.cache/eduversum/description/"+language+"/"+packageName+".html");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text) ) { return;}
    QTextStream out(&file);
    out << description;
}



void AppLoader::importApps()
{
    QString applicationsDir = appdir+"apps/xml/";
    QStringList appslicationFiles = QDir( applicationsDir ).entryList( QDir::Files );
    for ( QStringList::Iterator it = appslicationFiles.begin(); it != appslicationFiles.end(); ++it ) {
        importApp(applicationsDir,*it);
    }
}


void AppLoader::importApp(QString xmlFilePath, QString xmlFileName) {

    //  0: names
    //  1: id
    //  2: category
    //  3: package
    //  4: status
    //  5: exec
    //  6: description
    //  7: homepage
    //  6: desktopPath
    //  8: terminal
    //  9: icon
    // 10: example

    QString id = QString(xmlFileName).replace(".xml", "");
    QMap<QString,QString> appList = EasyXml::xmlToList(xmlFilePath+xmlFileName);

    if( !appList.contains("name") )    { appList.insert("name",    id);}
    if( !appList.contains("package") ) { appList.insert("package", id.toLower());}
    if( !appList.contains("exec" ) )   { appList.insert("exec",    id.toLower());}
    if( !appList.contains("icon") )    { appList.insert("icon",    id.toLower());}

    // icon
    appList.insert("icon", iconloader->getIconPath(appList.value("icon")) );
    //descripition

    QStringList packages = appList.value("package").split(" ");

    QString path = "apps/descriptions/"+language+"/"+packages[0]+".html";
    QString description = getDescription(path);

    // status
    QString status;
    if( QFile::exists( "/usr/share/doc/"+packages[0]+"/copyright" ) ) {
        status = "installed";
    } else {
        status = "notinstalled";
    }
    // hasExample
    QString example;
    QString packageDir = "/usr/share/seminarix-samples/"+packages[0];
    if( QFile::exists(packageDir) ) {
        example = "TRUE";
    } else {
        example =  "FALSE";
    }

    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(  0, appList.value("name"));
    item->setText(  1, id);
    item->setText(  2, appList.value("seminarix"));
    item->setText(  3, appList.value("package"));
    item->setText(  4, status);
    item->setText(  5, appList.value("exec"));
    item->setText(  6, description);
    item->setText(  7, appList.value("homepage"));
    item->setText(  8, appList.value("terminal"));
    item->setText(  9, appList.value("icon"));
    item->setText( 10, example);
    QPixmap ico = iconloader->getIcon(appList.value("icon"));
    item->setIcon( 0, ico );
    apps.append(item);

}


QString AppLoader::getDescription(QString path) {

    if ( QFile::exists(appdir+path) ) {
        QFile file(appdir+path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) { qDebug() << "Could not read file: " << appdir+path; };
        return file.readLine();
   } else if ( QFile::exists(QDir::homePath()+"/.cache/"+path) ) {
        QFile file(QDir::homePath()+"/.cache/"+path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) { qDebug() << "Could not read file: " << QDir::homePath()+"/.cache/"+path; };
        return file.readLine();
   }
   qDebug() << "Could not read file: " << appdir+path;
   return "";
}

QList<QTreeWidgetItem *> AppLoader::importCategories() {

    QString categoriesDir  = appdir+"categories/xml/";
    QString descriptionDir = appdir+"categories/descriptions/";
    QStringList categoriesFiles = QDir( categoriesDir ).entryList( QDir::Files );
    if( QDir(descriptionDir+language).exists() ) {
        descriptionDir += language+"/";
    } else {
        descriptionDir += "en/";
    }

    QList<QTreeWidgetItem *> items;
    for ( QStringList::Iterator it = categoriesFiles.begin(); it != categoriesFiles.end(); ++it ) {
        QMap<QString,QString> categoriesList = EasyXml::xmlToList(categoriesDir + *it);
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setIcon( 0, QIcon( appdir+"categories/icons/"+categoriesList.value("icon")+".png" ) );
        item->setText( 0, categoriesList.value("name") );
        QString id = QString(*it).replace(".xml", "");
        item->setText( 1, id );

        QString path = "categories/descriptions/"+language+"/"+QString(*it).replace(".xml",".html");
        item->setText( 2, getDescription(path) );
        items.append(item);

    }
    return items;
}
