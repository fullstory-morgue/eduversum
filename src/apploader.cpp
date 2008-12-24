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
#include <QDomDocument>


#include "apploader.h"

AppLoader::AppLoader()
{
	appdir = "/usr/share/eduversum/";
}


QTreeWidget* AppLoader::importApps()
{
	QTreeWidget *outputTreeWidget = new QTreeWidget;
	QString applicationsDir = appdir+"apps/";
	QStringList appslicationFiles = QDir( applicationsDir ).entryList( QDir::Files );


	for ( QStringList::Iterator it = appslicationFiles.begin(); it != appslicationFiles.end(); ++it ) {


		QString name;        //0
		QString id;          //1
		QString category;    //2
		QString package;     //3
		QString status;      //4
		QString exec;        //5
		QString description; //6
		QString homepage;    //7
		QString desktopPath; //6.1
		QString terminal;    //8
		QString icon;        //9
		QString example;     //10

		QTreeWidget *categoryValues = xmlToList( applicationsDir + *it );


		id = QString(*it).replace(".xml", "");

		name        = getXmlValue(categoryValues, "name");
		category    = getXmlValue(categoryValues, "seminarix");
		package     = getXmlValue(categoryValues, "package");
		exec        = getXmlValue(categoryValues, "exec");
		description = getXmlValue(categoryValues, "description");
		desktopPath = getXmlValue(categoryValues, "desktopPath");
		homepage    = getXmlValue(categoryValues, "homepage");
		terminal    = getXmlValue(categoryValues, "terminal");
		icon        = getXmlValue(categoryValues, "icon");

		if(name == "")
			name = id;
		if( package == "" )
			package = id.toLower();
		if( exec == "")
			exec = id.toLower();
		if( icon == "")
			icon = id.toLower();
		// status
		if( QFile::exists( "/usr/share/doc/"+package.split(" ")[0]+"/copyright" ) )
			status = "installed";
		else
			status = "notinstalled";
		// hasExample
		QString packageDir = "/usr/share/seminarix-samples/"+package.split( " " )[0];
		if( QFile::exists(packageDir) )
			example = "TRUE";
		else
			example =  "FALSE";


		QTreeWidgetItem *item = new QTreeWidgetItem(outputTreeWidget, 0);
		item->setText(  0, name);
		item->setText(  1, id);
		item->setText(  2, category);
		item->setText(  3, package);
		item->setText(  4, status);
		item->setText(  5, exec);
		item->setText(  6, description);
		item->setText(  7, homepage);
		item->setText(  8, terminal);
		item->setText(  9, icon); 
		item->setText( 10, example);

	}

	return outputTreeWidget;
}


QTreeWidget* AppLoader::importCategories() {

	QTreeWidget *outputTreeWidget = new QTreeWidget;
	QString categoriesDir = appdir+"categories/";
	QStringList categoriesFiles = QDir( categoriesDir ).entryList( QDir::Files );


	for ( QStringList::Iterator it = categoriesFiles.begin(); it != categoriesFiles.end(); ++it ) {

		QTreeWidget *categoryValues = xmlToList( categoriesDir + *it );
		QTreeWidgetItem *item = new QTreeWidgetItem(outputTreeWidget, 0);
		item->setText( 0, getXmlValue(categoryValues, "icon") );
		item->setText( 1, getXmlValue(categoryValues, "name") );
		QString id = QString(*it).replace(".xml", "");
		item->setText( 2, id );
	}

	return outputTreeWidget;
}


QTreeWidget* AppLoader::xmlToList(QString inputString) {

	QTreeWidget *outputTreeWidget = new QTreeWidget;

	QDomDocument doc("mydocument");
	QFile file( inputString );
	if (!file.open(QIODevice::ReadOnly))
		return outputTreeWidget;
	if (!doc.setContent(&file)) {
		file.close();
		return outputTreeWidget;
	}
	file.close();

	QDomElement docElem = doc.documentElement();
	QDomNode n = docElem.firstChild();
	while(!n.isNull()) {
		QDomElement e = n.toElement(); // try to convert the node to an element.
		if(!e.isNull()) {
			QTreeWidgetItem *item = new QTreeWidgetItem(outputTreeWidget, 0);
			item->setText( 0, e.tagName() );
			item->setText( 1, e.text() );
		}
		n = n.nextSibling();
	}
 
	return outputTreeWidget;
}

QString AppLoader::getXmlValue(QTreeWidget *inputTreeWidget, QString inputString) {

	QString outputString;
	if( inputTreeWidget->findItems(inputString, Qt::MatchExactly, 0 ).count() > 0 )
		outputString = inputTreeWidget->findItems(inputString, Qt::MatchExactly, 0).first()->text(1);
	return outputString;
}