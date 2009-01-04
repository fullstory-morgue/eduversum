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

MainWindow::MainWindow (QWidget* parent, Qt::WFlags flags): QWidget (parent, flags)
{
	appdir      = "/usr/share/eduversum/";

	loadGui();
	loadData();



}


void MainWindow::loadGui()
{

 	setupUi(this);
	iconloader = new IconLoader();
	frame2->hide();
	execPushButton->setIcon( QPixmap( appdir+"/icons/exec.png") );
	homepagePushButton->setIcon( QPixmap( appdir+"/icons/homepage.png") );
	examplePushButton->setIcon( QPixmap( appdir+"/icons/example.png") );
	showChangesPushButton->setIcon( QPixmap( appdir+"/icons/show.png") );

	cancelChangesPushButton->setIcon( QPixmap( appdir+"/icons/cancel.png") );
	discardChangesPushButton->setIcon( QPixmap( appdir+"/icons/discard.png") );
	applyChangesPushButton->setIcon( QPixmap( appdir+"/icons/apply.png") );

	setWindowIcon( QPixmap( appdir+"/icons/eduversum.png") );
	descriptionTextBrowser->setOpenExternalLinks(TRUE); // open links in a external browser

	connect(categoriesTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),this, SLOT( showCategoryApps() ) );
	connect(treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),this, SLOT( showApp() ) );
	connect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)),this, SLOT( changed() ) );
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
	trayIcon = new QSystemTrayIcon;

	quitAction = new QAction(tr("&Beenden"), this);
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

	trayIconMenu = new QMenu(this);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(quitAction);

	trayIcon->setContextMenu(trayIconMenu);
	trayIcon->setIcon(QPixmap( appdir+"/icons/eduversum.png"));
	trayIcon->setVisible(TRUE);
	trayIcon->show();

	//connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));


	descriptionTextBrowser->setText( QString::fromUtf8("Eduversum ist Hilfsmittel, mit dessen Hilfe Programme aus dem Bildungsbereich sehr einfach installiert (im leeren Kästchen vor der Anwendung einen Haken setzen) oder deinstalliert werden (den Haken entfernen) können. Zu diesem Zwecke ist das Administratorenpasswort erforderlich, da Softwareinstallation bzw. die Deinstallation von Anwendungen in der Regel eine systemweite Arbeit ist.") );



	unsetGui();

}


void MainWindow::unsetGui()
{
	frame1->show();
	frame2->hide();
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
		item->setIcon( 0, QIcon( "/usr/share/icons/hicolor/32x32/apps/"+(*it)->text(0)) );
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
		for(uint i = 0; i < 3; i++)
				item->setBackground(i, QColor(Qt::green) );
	}
	else if( status == "remove" ) {
		item->setCheckState(0,Qt::Unchecked);
		for(uint i = 0; i < 3; i++)
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
	if( status == "installed" or status == "remove" )
		execPushButton->show();
	else
		execPushButton->hide();

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

	QString exec = treeWidget->selectedItems().first()->text(5);
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
		QString program =  appdir+"sh/copyExample";
		QStringList arguments;
		arguments << package;

		QProcess *myProcess = new QProcess(this);
		myProcess->start(program, arguments);

	}
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
	    QMessageBox::information(this, "Systray",
				    QString::fromUtf8("Um das Programm zu beenden, wählen sie 'Beenden' im Kontextmenue des Programms im Systemabschnitt der Leiste.") );



	    hide();
	    event->ignore();
	}
 }


//------------------------------------------------------------------------------
//-- tray events ---------------------------------------------------------------
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



	for(uint i = 0; i < 3; i++)
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
		frame1->hide();
		frame2->show();
	}

}


void MainWindow::applyChanges()
{
	QString program = "su-to-root";
	QStringList arguments;
	arguments << "-X" << "-c" << "x-terminal-emulator -e /usr/share/eduversum/sh/applyChanges"+addRemoveApp;

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


