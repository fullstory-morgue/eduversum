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
#include "mainwindow.h"


//------------------------------------------------------------------------------
//-- init ----------------------------------------------------------------------
//------------------------------------------------------------------------------

MainWindow::MainWindow (QMainWindow* parent, Qt::WFlags flags): QMainWindow (parent, flags)
{
	appdir      = "/usr/share/eduversum/";

	loadGui();
	loadData();

}


void MainWindow::loadGui()
{

 	setupUi(this);
	iconloader = new IconLoader();

	execPushButton->setIcon( QPixmap( appdir+"/icons/exec.png") );
	homepagePushButton->setIcon( QPixmap( appdir+"/icons/homepage.png") );
	examplePushButton->setIcon( QPixmap( appdir+"/icons/example.png") );
	showChangesPushButton->setIcon( QPixmap( appdir+"/icons/show.png") );

	cancelChangesPushButton->setIcon( QPixmap( appdir+"/icons/cancel.png") );
	discardChangesPushButton->setIcon( QPixmap( appdir+"/icons/discard.png") );
	applyChangesPushButton->setIcon( QPixmap( appdir+"/icons/apply.png") );

	setWindowIcon( QPixmap( appdir+"/icons/eduversum.png") );
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


  

	// tray
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);


	trayIcon = new QSystemTrayIcon;

	stuffAction = new QAction("Arbeitsmaterialen", this);
	connect(stuffAction, SIGNAL(triggered()), this, SLOT(showStuff()));
	aboutAction = new QAction(QString::fromUtf8("Über Eduversum"), this);
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));
	quitAction = new QAction("&Beenden", this);
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

	trayIconMenu = new QMenu(this);
	//trayIconMenu->addSeparator();
	trayIconMenu->addAction(stuffAction);
	trayIconMenu->addAction(aboutAction);
	trayIconMenu->addAction(quitAction);

	trayIcon->setContextMenu(trayIconMenu);
	trayIcon->setIcon(QPixmap( appdir+"/icons/eduversum.png"));
	trayIcon->setVisible(TRUE);
	trayIcon->show();

	//connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

	setAbout();
	setHelp();
	descriptionTextBrowser->setText( "<h2>Willkomen</h2>"+help );



	unsetGui();

}


void MainWindow::unsetGui()
{
	stackedWidget->setCurrentIndex(0);
	execPushButton->hide();
	homepagePushButton->hide();
	examplePushButton->hide();
}


void MainWindow::loadData()
{
	AppLoader *apploader = new AppLoader();

	appTreeWidget = apploader->importApps();
	QTreeWidget *catTreeWidget = apploader->importCategories();


	// show categories
	QTreeWidgetItemIterator it(catTreeWidget);
	while (*it) {
		QTreeWidgetItem *item = new QTreeWidgetItem(categoriesTreeWidget, 0);
		item->setIcon( 0, QIcon( appdir+"categories/icons/"+(*it)->text(0)) );
		item->setText( 0, (*it)->text(1) );
		item->setText( 1, (*it)->text(2) );
		++it;
	}

}

//------------------------------------------------------------------------------
//-- show apps -----------------------------------------------------------------
//------------------------------------------------------------------------------

void MainWindow::showCategoryApps()
{

	if ( categoriesTreeWidget->selectedItems().count() < 1 )
		return;
	treeWidget->clear();

	treeWidget->show();
	frame0->show();
	stackedWidget->show();

	// show category descrption
	QString category = categoriesTreeWidget->selectedItems().first()->text(1);
	descriptionTextBrowser->clear();
	descriptionTextBrowser->setSource(appdir+"categories/html/"+category+".html");



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


void MainWindow::searchApps()
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

void MainWindow::showAppInList(QTreeWidgetItemIterator it)
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

void MainWindow::showApp()
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

void MainWindow::execApp()
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


void MainWindow::showHomepage()
{
	if ( treeWidget->selectedItems().count() < 1 ) 
		return;

	QString homepage = treeWidget->selectedItems().first()->text(7);
	QDesktopServices::openUrl(QUrl(homepage));

}


void MainWindow::copyExample()
{
	if(QMessageBox::question(this, "Beispieldateien", "Sollen die Beispieldateien auf dem Desktop angezeigt werden?", QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
	{
		QString package = treeWidget->selectedItems().first()->text(3);
		package = package.split(" ")[0];
		copyDir("/usr/share/seminarix-samples/"+package, QDir::homePath ()+"/Desktop/"+package );
	}
}

void MainWindow::copyDir(QString sourceDir, QString destinationDir)
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
//-- tray events ---------------------------------------------------------------
//------------------------------------------------------------------------------

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
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



void MainWindow::closeEvent(QCloseEvent *event)
{
	if (trayIcon->isVisible()) {
		QMessageBox::information(this, "Systray", QString::fromUtf8("Um das Programm zu beenden, wählen sie 'Beenden' im Kontextmenue des Programms im Systemabschnitt der Leiste.") );
		hide();
		event->ignore();
	}
}


//------------------------------------------------------------------------------
//-- about ---------------------------------------------------------------------
//------------------------------------------------------------------------------

void MainWindow::setAbout()
{
	about += "\nEntwickler:\n";
	about += QString::fromUtf8("Fabian Würtz <xadras@sidux.com>\n");

	about += "\nPaketzusammenstellung und Dokumentation:\n";
	about += "Roland Engert (RoEn)\n";
	about += QString::fromUtf8("Björn Jilg (BlueShadow)\n");
	about += "Thomas Kross (captagon)\n";
	about += "Hendrik Lehmbruch (hendrikL)\n";
	about += "Dinko Sabo (cobra) <cobra@sidux.com>\n";

	
	about += "\nIcons:\n";
	about += QString::fromUtf8("Bernard Gray")+" <bernard.gray@gmail.com>\n";

	about += "\nWeitere Beteiligte:\n";
	about += "Stefan Lippers-Hollmann (slh)\n";
	about += "Ferdi Thommes (devil)\n";
	about += "Horst Tritremmel (hjt)\n";
 	about += "Wolf-Dieter Zimmermann (emile)\n";

	about += "\nLizenz: GPL" ;
}



void MainWindow::showAbout()
{
	if( isHidden() )
		trayIcon->showMessage ( QString::fromUtf8("Über Eduversum"), about, QSystemTrayIcon::Information, 20000 );
	else
		QMessageBox::information(this, QString::fromUtf8("Über Eduversum"), about );
	
}

//------------------------------------------------------------------------------
//-- help ---------------------------------------------------------------------
//------------------------------------------------------------------------------

void MainWindow::setHelp()
{
	help = QString::fromUtf8("Eduversum ist ein Hilfsmittel, mit dessen Hilfe Programme aus dem Bildungsbereich sehr einfach installiert oder deinstalliert werden können (Installation: im leeren Kästchen vor der Anwendung einen Haken setzen, Deinstallation: den Haken entfernen). Zu diesem Zwecke ist das Administratorpasswort erforderlich, da Softwareinstallation bzw. die Deinstallation in der Regel systemweite Arbeiten sind.");
}


void MainWindow::showHelp()
{ 
	QMessageBox::information(this, "Eduversum Hilfe", help );
}

//------------------------------------------------------------------------------
//-- stuff ---------------------------------------------------------------------
//------------------------------------------------------------------------------

void MainWindow::showStuff()
{
	QString exec;
	if( QFile::exists("/usr/bin/dolphin") )
		exec = "dolphin";
	else if( QFile::exists("/usr/bin/konqueror") )
		exec = "konqueror";
	else if( QFile::exists("/usr/bin/nautilus") )
		exec = "nautilus";
	else if( QFile::exists("/usr/bin/thunar") )
		exec = "thunar";
	else if( QFile::exists("/usr/bin/pcmanfm") )
		exec = "pcmanfm";
	else {
		QMessageBox::information(this, QString::fromUtf8("Fehler"), QString::fromUtf8("Es wurde kein Dateimanager gefunden") );
		return;
	}

	QStringList arguments;
	arguments << "/usr/share/seminarix-notebook-training/uebersicht.html";

	QProcess *myProcess = new QProcess(this);
	myProcess->start(exec, arguments);

}


//------------------------------------------------------------------------------
//--seminarix-latex-----------------------------------------------------------------
//------------------------------------------------------------------------------

void MainWindow::showSeminarixLatex()
{
	QString exec;
	if( QFile::exists("/usr/bin/dolphin") )
		exec = "dolphin";
	else if( QFile::exists("/usr/bin/konqueror") )
		exec = "konqueror";
	else if( QFile::exists("/usr/bin/nautilus") )
		exec = "nautilus";
	else if( QFile::exists("/usr/bin/thunar") )
		exec = "thunar";
	else if( QFile::exists("/usr/bin/pcmanfm") )
		exec = "pcmanfm";
	else {
		QMessageBox::information(this, QString::fromUtf8("Fehler"), QString::fromUtf8("Es wurde kein Dateimanager gefunden") );
		return;
	}

	QStringList arguments;
	arguments << "/usr/share/seminarix-latex";

	QProcess *myProcess = new QProcess(this);
	myProcess->start(exec, arguments);

}

//------------------------------------------------------------------------------
//--open-source-----------------------------------------------------------------
//------------------------------------------------------------------------------

void MainWindow::showOpenSource()
{
	QString exec;
	if( QFile::exists("/usr/bin/dolphin") )
		exec = "dolphin";
	else if( QFile::exists("/usr/bin/konqueror") )
		exec = "konqueror";
	else if( QFile::exists("/usr/bin/nautilus") )
		exec = "nautilus";
	else if( QFile::exists("/usr/bin/thunar") )
		exec = "thunar";
	else if( QFile::exists("/usr/bin/pcmanfm") )
		exec = "pcmanfm";
	else {
		QMessageBox::information(this, QString::fromUtf8("Fehler"), QString::fromUtf8("Es wurde kein Dateimanager gefunden") );
		return;
	}

	QStringList arguments;
	arguments << "/usr/share/seminarix-notebook-training/projekte/freiesoftware/freiesoftware.html";

	QProcess *myProcess = new QProcess(this);
	myProcess->start(exec, arguments);

}



//------------------------------------------------------------------------------
//-- changed -------------------------------------------------------------------
//------------------------------------------------------------------------------

void MainWindow::changed()
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



void MainWindow::showChanges()
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
		QMessageBox::information(this, QString::fromUtf8("Keine Veränderungen"), QString::fromUtf8("Es sind keine Veränderungen vorhanden. Zum Programme zu installieren oder zu deinstallieren einfach das Markierungsfeld aktiveren bzw. deaktivieren.") );
	else {
		descriptionTextBrowser->setText("<h3>&Auml;nderungen ausf&uuml;hren</h3>Die aufgef&uuml;hrten Programme in der oberen Liste werden installiert (gr&uuml;n) beziehungsweise deinstalliert (rot).");
		stackedWidget->setCurrentIndex(1);
	}

}


void MainWindow::applyChanges()
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

void MainWindow::applyChangesFinished()
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

void MainWindow::discardChanges()
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


void MainWindow::cancelChanges()
{
	treeWidget->clear();
	descriptionTextBrowser->clear();
	unsetGui();
}


