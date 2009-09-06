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



#include "apploader.h"
#include "eduversum.h"
#include "easyXml.h"


//------------------------------------------------------------------------------
//-- init ----------------------------------------------------------------------
//------------------------------------------------------------------------------

Eduversum::Eduversum (QWidget* parent, Qt::WFlags flags): QWidget (parent, flags)
{
	appdir      = "/usr/share/eduversum/";
	loadGui();
	loadData();
}


void Eduversum::loadGui()
{

    setupUi(this);
    iconloader = new IconLoader();
    setWindowIcon( QIcon("/usr/share/icons/hicolor/scalable/apps/eduversum.svgz") );

    // buttons
    applicationsButton->setIcon( QPixmap(appdir+"/icons/apps.svg") );
    webButton->setIcon( QPixmap( appdir+"/icons/web.svg") );
    docsButton->setIcon( QPixmap( appdir+"/icons/docs.svg") );
    helpButton->setIcon( QPixmap(appdir+"/icons/help.svg") );
    execPushButton->setIcon( QPixmap( appdir+"/icons/exec.png") );
    homepagePushButton->setIcon( QPixmap( appdir+"/icons/homepage.png") );
    examplePushButton->setIcon( QPixmap( appdir+"/icons/example.png") );
    showChangesPushButton->setIcon( QPixmap( appdir+"/icons/show.png") );
    cancelChangesPushButton->setIcon( QPixmap( appdir+"/icons/cancel.png") );
    discardChangesPushButton->setIcon( QPixmap( appdir+"/icons/discard.png") );
    applyChangesPushButton->setIcon( QPixmap( appdir+"/icons/apply.png") );


    //descriptionTextBrowser->setOpenExternalLinks(TRUE); // open links in a external browser
    connect(categoriesTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),this, SLOT( showCategoryApps() ) );
    connect(treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),this, SLOT( showApp() ) );
    connect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)),this, SLOT( changed() ) );
    connect(treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this, SLOT( execApp() ) );
    connect(searchLineEdit, SIGNAL(textChanged(const QString)), SLOT(searchApps()));
    connect(comboBox, SIGNAL(currentIndexChanged(int)),this, SLOT( showCategoryApps() ) );
    connect(execPushButton, SIGNAL(clicked()), SLOT(execApp()));
    connect(homepagePushButton, SIGNAL(clicked()), SLOT(showHomepage()));
    connect(examplePushButton, SIGNAL(clicked()), SLOT(copyExample()));
    connect(showChangesPushButton, SIGNAL(clicked()), SLOT(showChanges()));
    connect(cancelChangesPushButton, SIGNAL(clicked()), SLOT(cancelChanges()));
    connect(discardChangesPushButton, SIGNAL(clicked()), SLOT(discardChanges()));
    connect(applyChangesPushButton, SIGNAL(clicked()), SLOT(applyChanges()));


    // help menu
    QMenu *helpMenu = new QMenu(this);
    QAction *helpAction = new QAction(QIcon(appdir+"/icons/help.svg"),tr("Eduversum Help"), this);
    connect(helpAction, SIGNAL(triggered()), this, SLOT(showHelp()));
    aboutAction = new QAction( QIcon("/usr/share/icons/hicolor/scalable/apps/eduversum.svgz"), tr("About Eduversum"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));
    helpMenu->addAction(helpAction);
    helpMenu->addAction(aboutAction);
    helpButton->setMenu(helpMenu);



    // tray
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon = new QSystemTrayIcon;
    //stuffAction = new QAction(tr("Notebook-Education"), this);
    //connect(stuffAction, SIGNAL(triggered()), this, SLOT(showStuff()));
    quitAction = new QAction(QIcon(appdir+"/icons/exit.svg"), tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    trayIconMenu = new QMenu(this);
    //trayIconMenu->addSeparator();
    //trayIconMenu->addAction(stuffAction);
    trayIconMenu->addAction(helpAction);
    trayIconMenu->addAction(aboutAction);
    trayIconMenu->addAction(quitAction);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon("/usr/share/icons/hicolor/scalable/apps/eduversum.svgz"));
    trayIcon->setVisible(TRUE);
    trayIcon->show();
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));


    setAbout();
    setHelp();
    descriptionTextBrowser->setText( "<h2>"+tr("Welcome")+"</h2>"+help );


    unsetGui();

}


void Eduversum::showAppsPage()
{
    contentStackedWidget->setCurrentIndex(0);
}


void Eduversum::unsetGui() {
    stackedWidget->setCurrentIndex(0);
    execPushButton->hide();
    homepagePushButton->hide();
    examplePushButton->hide();
}


void Eduversum::loadData()
{
	QString  language = "en";
	if( QLocale::system().name().left(2) == "de" )
		language = "de";  
  
  
	// load apps
	AppLoader *apploader = new AppLoader();
	appTreeWidget = apploader->importApps();
	QTreeWidget *catTreeWidget = apploader->importCategories();

	// load categories
	QTreeWidgetItemIterator it(catTreeWidget);
	while (*it) {
			QTreeWidgetItem *item = new QTreeWidgetItem(categoriesTreeWidget, 0);
			item->setIcon( 0, QIcon( appdir+"categories/icons/"+(*it)->text(0)) );
			item->setText( 0, (*it)->text(1) );
			item->setText( 1, (*it)->text(2) );
			item->setText( 2, (*it)->text(3) );
			++it;
	}

	// load docs
	docsTextBrowser->setText("<h2>"+tr("Teaching Aids")+"</h2>"+tr("Here you find a collection of teaching aids. To open one, double click on the specific entry."));
	QStringList teachings = QDir( appdir+"/teaching/"+language+"/").entryList( QDir::Files );
	foreach(QString teaching, teachings) {
		EasyXml *projectData = new EasyXml(appdir+"/teaching/"+language+"/"+teaching);
		if(projectData->getValue("name").count() > 0) {
			QTreeWidgetItem *item = new QTreeWidgetItem(docsTreeWidget);
			item->setText( 0, projectData->getValue("name")[0] );
			QStringList tmpList = projectData->getValue("description");
			if( tmpList.count() > 0)
				item->setText( 2, tmpList[0] );
			QStringList titles = projectData->getValue((QStringList() << "document" << "title" ));
			QStringList paths  = projectData->getValue((QStringList() << "document" << "path"  ));
			if(titles.count() == paths.count()) {
				for (int i = 0; i < titles.count(); i++) {
					QTreeWidgetItem *item2 = new QTreeWidgetItem(item);
					item2->setText( 0, titles[i] );
					item2->setText( 1, paths[i] );
				}
			}
		}
	}
    
  
	// load web links
	webTextBrowser->setText("<h2>"+tr("Weblinks")+"</h2>"+tr("Here you find a collection of links, that are relation with the topics linux and education.To a link, double click on the specific entry."));
	QStringList links = QDir( appdir+"/weblinks/"+language+"/").entryList( QDir::Files );
	foreach(QString link, links) {
		EasyXml *projectData = new EasyXml(appdir+"/weblinks/"+language+"/"+link);
		QStringList tmp;
		QTreeWidgetItem *item = new QTreeWidgetItem(webTreeWidget);
		tmp = projectData->getValue((QStringList() << "title" ));
		if( tmp.count() > 0 )
			item->setText( 0, tmp[0] );
		tmp = projectData->getValue((QStringList() << "url" ));
		if( tmp.count() > 0 )
			item->setText( 1, tmp[0] );
		tmp = projectData->getValue((QStringList() << "description" ));
		if( tmp.count() > 0 )
			item->setText( 2, tmp[0] );
    }
    
}




void Eduversum::readTree(QDomNode n, QTreeWidgetItem *item)
{

    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            QTreeWidgetItem *item2;
            if (item)
                item2 = new QTreeWidgetItem(item);
            else
                item2 = new QTreeWidgetItem(docsTreeWidget);
            item2->setText( 0, e.tagName() );
            item2->setText( 1, e.text() );
            item2->setSelected(true);

            readTree(n.firstChild(), item2);

         }
         n = n.nextSibling();
    }
}




//------------------------------------------------------------------------------
//-- show apps -----------------------------------------------------------------
//------------------------------------------------------------------------------

void Eduversum::showCategoryApps()
{

	if ( categoriesTreeWidget->selectedItems().count() < 1 )
		return;
	treeWidget->clear();

	treeWidget->show();
	frame0->show();
	stackedWidget->show();

	// show category descrption
	QString category    = categoriesTreeWidget->selectedItems().first()->text(0);
	QString description = categoriesTreeWidget->selectedItems().first()->text(2);
	descriptionTextBrowser->clear();
	descriptionTextBrowser->setHtml("<h3>"+category+"</h3>"+description);



	QTreeWidgetItemIterator it(appTreeWidget);
	while (*it) {
		bool show = TRUE;
		if( comboBox->currentIndex() == 0 ) {}       // all programms
		else if( comboBox->currentIndex() == 1 ) {   // just installed programms
			QString status0 = (*it)->text(4);
			if( status0 == "notinstalled" or status0 == "install" )
				show = FALSE;
		}
		else if( comboBox->currentIndex() == 2 ) {   // just not installed programms
			QString status0 = (*it)->text(4);
			if( status0 == "installed" or  status0 == "remove" )
				show = FALSE;
		}
		if( ! (*it)->text(2).contains(category) )
			show = FALSE;
		if( show) 
			showAppInList(it);
		++it;
	}

	unsetGui();
}


void Eduversum::searchApps()
{
	treeWidget->clear();

	QTreeWidgetItemIterator it(appTreeWidget);
	while (*it) {
		if( (*it)->text(1).contains( searchLineEdit->text() ) or (*it)->text(6).contains( searchLineEdit->text() ) )
			showAppInList(it);
		++it;
	}

	unsetGui();
}


void Eduversum::showAppInList(QTreeWidgetItemIterator it)
{
	QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget, 0);
	item->setText(  0, (*it)->text( 0) ); // name
	item->setText(  1, (*it)->text( 1) ); // id
	item->setText(  2, (*it)->text( 2) ); // category
	item->setText(  3, (*it)->text( 3) ); // package
	item->setText(  4, (*it)->text( 4) ); // status
	item->setText(  5, (*it)->text( 5) ); // exec
	item->setText(  6, (*it)->text( 6) ); // description
	item->setText(  7, (*it)->text( 7) ); // homepage
	item->setText(  8, (*it)->text( 8) ); // terminal
	item->setText(  9, (*it)->text( 9) ); // icon
	item->setText( 10, (*it)->text(10) ); // example
	QPixmap icon = iconloader->getIcon((*it)->text( 9));
	item->setIcon( 0, icon );
	// show status
	QString status = (*it)->text(4);
	if( status == "installed" )
		item->setCheckState(0,Qt::Checked);
	else if( status == "notinstalled" )
		item->setCheckState(0,Qt::Unchecked);
	else if( status == "install" ) {
		item->setCheckState(0,Qt::Checked);
		for( int i = 0; i < 3; i++)
				item->setBackground(i, QColor(Qt::green) );
	}
	else if( status == "remove" ) {
		item->setCheckState(0,Qt::Unchecked);
		for( int i = 0; i < 3; i++)
				item->setBackground(i, QColor(Qt::red) );
	}

}


void Eduversum::showApp()
{

	if ( treeWidget->selectedItems().count() < 1 ) 
		return;

	// Show description
	QString name = treeWidget->selectedItems().first()->text(0);
	QString description =  treeWidget->selectedItems().first()->text(6);
	descriptionTextBrowser->setText( "<h2>"+name+"</h2>"+description );

	//Show homepage buttons
	QString homepage = treeWidget->selectedItems().first()->text(7);
	if( homepage != "")
		homepagePushButton->show();
	else
		homepagePushButton->hide();

	// Show exec button
	QString status = treeWidget->selectedItems().first()->text(4);
	QString exec = treeWidget->selectedItems().first()->text(5);
	if( status == "notinstalled" or status == "install" or exec == "none")
		execPushButton->hide();
	else
		execPushButton->show();

	// Show exmaple button
	QString example = treeWidget->selectedItems().first()->text(10);
	if( example == "TRUE" )
		examplePushButton->show();
	else
		examplePushButton->hide();


}


//------------------------------------------------------------------------------
//-- button functions ----------------------------------------------------------
//------------------------------------------------------------------------------

void Eduversum::execApp()
{
	if ( treeWidget->selectedItems().count() < 1 ) 
		return;

	QString status = treeWidget->selectedItems().first()->text(4);
	QString exec = treeWidget->selectedItems().first()->text(5);

	if(  status == "notinstalled" or status == "install" or exec == "none" )
		return;

	QString terminal = treeWidget->selectedItems().first()->text(8);
	if( terminal == "true" or terminal == "yes")
		exec = "konsole --noclose -e "+exec;

	
	QStringList arguments = exec.split(" ");
	QString program = arguments[0];
	arguments.removeFirst();

	QProcess *myProcess = new QProcess(this);
	myProcess->start(program, arguments);

}


void Eduversum::showHomepage()
{
	if ( treeWidget->selectedItems().count() < 1 ) 
		return;

	QString homepage = treeWidget->selectedItems().first()->text(7);
	QDesktopServices::openUrl(QUrl(homepage));

}



void Eduversum::copyExample()
{
	if(QMessageBox::question(this, "Beispieldateien", "Sollen die Beispieldateien auf dem Desktop angezeigt werden?", QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
	{
		QString package = treeWidget->selectedItems().first()->text(3);
		package = package.split(" ")[0];
		copyDir("/usr/share/seminarix-samples/"+package, QDir::homePath ()+"/Desktop/"+package );
	}
}


void Eduversum::copyDir(QString sourceDir, QString destinationDir)
{
	if( !QFile::exists( destinationDir ) )
		QDir().mkdir(destinationDir);

	QStringList fileList = QDir( sourceDir ).entryList( QDir::Files );
	for ( QStringList::Iterator it = fileList.begin(); it != fileList.end(); ++it )
		QFile(sourceDir+"/"+*it).copy( destinationDir+"/"+*it );

	QStringList dirList = QDir( sourceDir ).entryList( QDir::Dirs );
	for( int i = 2; i < dirList.count(); i++)
	      copyDir(sourceDir+"/"+dirList[i], destinationDir+"/"+dirList[i] ); 
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
        if ( docsTreeWidget->selectedItems().count() < 1 )
                return;

        QString homepage = docsTreeWidget->selectedItems().first()->text(1);
        if(!homepage.isEmpty())
            QDesktopServices::openUrl(QUrl(homepage));

}


void Eduversum::showDocDescription()
{
	if ( docsTreeWidget->selectedItems().count() < 1 ) 
		return;
	QString title = docsTreeWidget->selectedItems().first()->text(0);
	QString description = docsTreeWidget->selectedItems().first()->text(2);
	if( !description.isEmpty() )
	    docsTextBrowser->setText("<h2>"+title+"</h2>"+description);
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
	if ( webTreeWidget->selectedItems().count() < 1 ) 
		return;
	QString title = webTreeWidget->selectedItems().first()->text(0);
	QString description = webTreeWidget->selectedItems().first()->text(2);
	webTextBrowser->setText("<h2>"+title+"</h2>"+description);
}


void Eduversum::showWeblink()
{
	if ( webTreeWidget->selectedItems().count() < 1 ) 
		return;
	QDesktopServices::openUrl(webTreeWidget->selectedItems().first()->text(1));
}



//------------------------------------------------------------------------------
//-- tray events ---------------------------------------------------------------
//------------------------------------------------------------------------------

void Eduversum::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
		case QSystemTrayIcon::Trigger:
		case QSystemTrayIcon::DoubleClick:
			if( isHidden() )
				show();
			else
				hide();
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

void Eduversum::setAbout()
{
	about += "\n"+tr("Developer")+":\n";
	about += QString::fromUtf8("Fabian Würtz <xadras@sidux.com>\n");
	about += "Dinko Sabo <cobra@sidux.com>\n";

	about += "\n"+tr("Combination of the packages and documentation")+":\n";
	about += "Roland Engert (RoEn)\n";
	about += QString::fromUtf8("Björn Jilg (BlueShadow)\n");
	about += "Thomas Kross (captagon)\n";
	about += "Hendrik Lehmbruch (hendrikL)\n";
	about += "Nikolas Poniros <nponiros@yahoo.com>\n";
	about += "Dinko Sabo <cobra@sidux.com>\n";

	about += "\n"+tr("Contributors")+":\n";
	about += "Stefan Lippers-Hollmann (slh)\n";
	about += "Ferdi Thommes (devil)\n";
	about += "Horst Tritremmel (hjt)\n";
 	about += "Wolf-Dieter Zimmermann (emile)\n";

	about += "\n"+tr("License")+": GPL" ;
}


void Eduversum::showAbout()
{
	if( isHidden() )
		trayIcon->showMessage ( tr("About Eduversum"), about, QSystemTrayIcon::Information, 20000 );
	else
		QMessageBox::information(this, tr("About Eduversum"), about );
	
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
	QString locale = QLocale::system().name().left(2);
	if( locale != "de" )
		locale = "en";
	QDesktopServices::openUrl(QUrl("/usr/share/doc/eduversum/benutzung-eduversum-"+locale+".html"));
}


//------------------------------------------------------------------------------
//-- chose filemanager ---------------------------------------------------------
//------------------------------------------------------------------------------

QString Eduversum::chooseFilenamanager()
{
	QString fileManager;
	if( QFile::exists("/usr/bin/dolphin") )
		fileManager = "dolphin";
	else if( QFile::exists("/usr/bin/konqueror") )
		fileManager = "konqueror";
	else if( QFile::exists("/usr/bin/nautilus") )
		fileManager = "nautilus";
	else if( QFile::exists("/usr/bin/thunar") )
		fileManager = "thunar";
	else if( QFile::exists("/usr/bin/pcmanfm") )
		fileManager = "pcmanfm";
	return fileManager;
}


//------------------------------------------------------------------------------
//-- stuff ---------------------------------------------------------------------
//------------------------------------------------------------------------------

void Eduversum::showStuff()
{
	QString exec = chooseFilenamanager();
	if (exec.isEmpty() ) {
		QMessageBox::information(this, tr("Error"), tr("No file-manager was found!") );
		return;
	}

	QStringList arguments;
	arguments << "/usr/share/seminarix-notebook-training/";

	QProcess *myProcess = new QProcess(this);
	myProcess->start(exec, arguments);

}


//------------------------------------------------------------------------------
//--seminarix-latex-----------------------------------------------------------------
//------------------------------------------------------------------------------

void Eduversum::showSeminarixLatex()
{
	QString exec;
	QDir dir("/usr/share/seminarix-latex");
	if (!dir.exists())
	{
		QMessageBox::information(this, tr("Error"), tr("This functionality is only available after the installation of the seminarix-latex package. You can easily install this from the General category. It is recommended that you install this program only if you are installing onto a hard disk as the size of the data it installs is big.") );
	}
	else
	{
		QString exec = chooseFilenamanager();
		if (exec.isEmpty() ) {
			QMessageBox::information(this, tr("Error"), tr("No file-manager was found!") );
			return;
		}

		QStringList arguments;
		arguments << "/usr/share/seminarix-latex";

		QProcess *myProcess = new QProcess(this);
		myProcess->start(exec, arguments);
	}
}


//------------------------------------------------------------------------------
//--open-source-----------------------------------------------------------------
//------------------------------------------------------------------------------

void Eduversum::showOpenSource()
{
	QString exec = chooseFilenamanager();
	if (exec.isEmpty() ) {
		QMessageBox::information(this, tr("Error"), tr("No file-manager was found!") );
		return;
	}

	QStringList arguments;
	arguments << "/usr/share/seminarix-notebook-training/projekte/freiesoftware";

	QProcess *myProcess = new QProcess(this);
	myProcess->start(exec, arguments);

}


//------------------------------------------------------------------------------
//-- changed -------------------------------------------------------------------
//------------------------------------------------------------------------------

void Eduversum::changed()
{
	if ( treeWidget->selectedItems().count() < 1 ) 
		return;

	QString name = treeWidget->selectedItems().first()->text(0);
	QString status = treeWidget->selectedItems().first()->text(4);
	QString newStatus;
	QColor color;

	if( treeWidget->selectedItems().first()->checkState(0) == Qt::Checked ) {
		if( status == "notinstalled" ) {
			color = QColor(Qt::green);
			newStatus = "install";
		}
		else {
			color = QColor(Qt::white);
			newStatus = "installed";
		}
	}
	else {
		if( status == "installed" ) {
			color = QColor(Qt::red);
			newStatus = "remove";
		}
		else {
			color = QColor(Qt::white);
			newStatus = "notinstalled";
		}
	}



	for( int i = 0; i < 3; i++)
		treeWidget->selectedItems().first()->setBackground(i, color );

	appTreeWidget->findItems(name, Qt::MatchExactly, 0 ).first()->setText(4,newStatus );

}


void Eduversum::showChanges()
{
	QTreeWidgetItemIterator it(appTreeWidget);
	addRemoveApp.clear();

	bool noChanges = TRUE;
	while (*it) {
		bool add = FALSE; QString pre;
		if( (*it)->text(4) == "install"  ) {
			add = TRUE;
			pre = "+";
		}
		else if( (*it)->text(4) == "remove" ) {
			add = TRUE;
			pre = "-";
		}
		if( add ) {
			if( noChanges )
				treeWidget->clear();
			noChanges = FALSE;
			showAppInList(it);
			QStringList packages = (*it)->text(3).split(" ");
			for( QStringList::Iterator it = packages.begin(); it != packages.end(); ++it )
				addRemoveApp += " "+pre+*it;
		}
		++it;
	}

	if( noChanges )
		QMessageBox::information(this, tr("No Changes"), tr("There are no changes. To install/uninstall a program just click the box on the left of the program's name.") );
	else {
		descriptionTextBrowser->setText(tr("<h3>Apply Changes</h3>The programs listed above are going to be installed (green) respectively uninstalled (red)."));
		stackedWidget->setCurrentIndex(1);
	}

}


void Eduversum::applyChanges()
{
	QString program = "su-to-root";
	QStringList arguments;
	//arguments << "-X" << "-c" << "x-terminal-emulator -e /usr/share/eduversum/sh/applyChanges"+addRemoveApp;
	arguments << "-X" << "-c" << "sidux-apt-qt4"+addRemoveApp;

	QProcess *myProcess = new QProcess(this);
	myProcess->start(program, arguments);
  
	connect(myProcess, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(applyChangesFinished()));
	setDisabled ( TRUE );


}


void Eduversum::applyChangesFinished()
{
	setEnabled ( TRUE );
	QTreeWidgetItemIterator it(appTreeWidget);
	while (*it) {
			QString name = (*it)->text(0) ;
			QString package = (*it)->text(3);
			QString status;
			// status
			if( QFile::exists( "/usr/share/doc/"+package.split(" ")[0]+"/copyright" ) )
				status = "installed";
			else
				status = "notinstalled";
			appTreeWidget->findItems(name, Qt::MatchExactly, 0 ).first()->setText(4, status );

		++it;
	}
	cancelChanges();
}


void Eduversum::discardChanges()
{
	QTreeWidgetItemIterator it(appTreeWidget);
	while (*it) {
		if( (*it)->text(4) == "install"  ) {
			QString name = (*it)->text(0) ;
			appTreeWidget->findItems(name, Qt::MatchExactly, 0 ).first()->setText(4, "notinstalled" );
		}
		else if ( (*it)->text(4) == "remove" ) {
			QString name = (*it)->text(0) ;
			appTreeWidget->findItems(name, Qt::MatchExactly, 0 ).first()->setText(4, "installed" );
		}
		++it;
	}
	cancelChanges();
}


void Eduversum::cancelChanges()
{
	treeWidget->clear();
	descriptionTextBrowser->clear();
	unsetGui();
}

