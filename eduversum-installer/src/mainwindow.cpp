/***************************************************************************
 *   Copyright (C) 2009 by Fabian Wuertz   *
 *   xadras@sidux.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
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



 #include <QtGui>
 #include <QtNetwork>


 #include "mainwindow.h"

 MainWindow::MainWindow(QStringList args, QWidget* parent, Qt::WFlags flags): QWidget (parent, flags)
 {


	//load input;
	foreach (QString package, args) {
		if ( package.left(1) == "+")
			install.append(package.mid(1));
		else if ( package.left(1) == "-")
			remove.append(package.mid(1));
	}



	timer = new QTimer(this);
	http = new QHttp(this);
	setupUi(this);
	contentStackedWidget->setCurrentIndex(0);
	//setWindowIcon( QPixmap( appdir+"/icons/eduversum.png") );
	outputTextBrowser->hide();
	descriptionTextBrowser->hide();
	warningLabel->hide();
	noteLabel->hide();
	connect(nextPushButton, SIGNAL(clicked()), SLOT(next()));
	connect(detailsPushButton, SIGNAL(clicked()), SLOT(toggleDetails()));

	status = "welcome";
	titleLabel->setText("<h3>"+tr("Welcome")+"</h3>");
	appDir = "/usr/share/eduversum-installer/"; 
 }





void MainWindow::next()
{

	if( status == "welcome" )
		aptGetUpdate();
	else if( status == "aptGetUpdate" )
		removePackages();
	else if(  status == "removePackages" )
		removePackagesDpkg();
	else if(  status == "removePackagesDpkg" )
		installPackages();
	else if(  status == "installPackages" )
		downloadPackages();
	else if(  status == "downloadPackages" )
		installPackagesDpkg();
	else if(  status == "installPackagesDpkg" )
		workDone();
	else if(  status == "workDone" )
		close();
}


//------------------------------------------------------------------------------
//-- update package list -------------------------------------------------------
//------------------------------------------------------------------------------

void MainWindow::aptGetUpdate()
{

	if( radioButton->isChecked() ) {
		status = "aptGetUpdate";
		titleLabel->setText("<h3>"+tr("Resynchronize the package index files")+"</h3>");
		contentStackedWidget->setCurrentIndex(1);
		nextPushButton->setDisabled(TRUE);

		updateProcess = new QProcess(this);
		output.clear();
		startProgressBar();
		QStringList arguments;
		arguments.append("update");
		QString program = "apt-get";
		connect( updateProcess, SIGNAL(readyReadStandardOutput()),this, SLOT(readOutput()));
		connect( updateProcess, SIGNAL(finished(int)),this, SLOT(processFinished()));
		updateProcess->start(program, arguments );
	}
	else
		removePackages();
}


//------------------------------------------------------------------------------
//-- remove packages -----------------------------------------------------------
//------------------------------------------------------------------------------

void MainWindow::removePackages()
{
	if( remove.count() > 0 ) {
		status = "removePackages";
		titleLabel->setText("<h3>"+tr("Remove packages")+"</h3>");
		contentStackedWidget->setCurrentIndex(2);
		nextPushButton->setDisabled(TRUE);

		QTreeWidgetItem *item1 = new QTreeWidgetItem(treeWidget, 0);
		item1->setIcon( 0, QIcon( appDir+"icons/wait.png") );
		item1->setText( 0, tr("Please wait...") );

		removeProcess1 = new QProcess(this);
		output.clear();
		QStringList arguments;
		arguments.append("remove");
		arguments += remove;
		QString program = appDir+"sh/getDep.sh";
		connect( removeProcess1, SIGNAL(readyReadStandardOutput()),this, SLOT(readOutput()));
		connect( removeProcess1, SIGNAL(finished(int)),this, SLOT(processOutput()));
		removeProcess1->start(program, arguments );
	}
	else
		installPackages();
}

void MainWindow::removePackagesDpkg()
{
	status = "removePackagesDpkg";
	titleLabel->setText("<h3>"+tr("Remove packages")+"</h3>");
	contentStackedWidget->setCurrentIndex(1);
	nextPushButton->setDisabled(TRUE);
	outputTextBrowser->clear();

	removeProcess2 = new QProcess(this);
	output.clear();
	startProgressBar();
	QStringList arguments;
	arguments.append("-r");
	arguments += removedPackages;
	QString program = "dpkg";
	connect( removeProcess2, SIGNAL(readyReadStandardOutput()),this, SLOT(readOutput()));
	connect( removeProcess2, SIGNAL(finished(int)),this, SLOT(processFinished()));
	removeProcess2->start(program, arguments );



}

//------------------------------------------------------------------------------
//-- install packages ----------------------------------------------------------
//------------------------------------------------------------------------------

void MainWindow::installPackages()
{

	if( install.count() > 0 ) {
		status = "installPackages";
		titleLabel->setText("<h3>"+tr("Install packages")+"</h3>");
		contentStackedWidget->setCurrentIndex(2);
		nextPushButton->setDisabled(TRUE);


		treeWidget->clear();
		QTreeWidgetItem *item1 = new QTreeWidgetItem(treeWidget, 0);
		item1->setIcon( 0, QIcon( appDir+"icons/wait.png") );
		item1->setText( 0, tr("Please wait...") );

		installProcess1 = new QProcess(this);
		output.clear();
		QStringList arguments;
		arguments.append("install");
		arguments += install;
		QString program = appDir+"sh/getDep.sh";
		connect( installProcess1, SIGNAL(readyReadStandardOutput()),this, SLOT(readOutput()));
		connect( installProcess1, SIGNAL(finished(int)),this, SLOT(processOutput()));
		installProcess1->start(program, arguments );
	}
	else
		workDone();
}

void MainWindow::downloadPackages()
{
	if( downloads.count() > 0 ) {
		status = "downloadPackages";
		titleLabel->setText("<h3>"+tr("Download packages")+"</h3>");
		nextPushButton->setDisabled(TRUE);
		contentStackedWidget->setCurrentIndex(3);
		treeWidget->clear();
		foreach (QString download, downloads) {
			//downloadFile(download);
			QTreeWidgetItem *item = new QTreeWidgetItem(downloadTreeWidget, 0);
			QStringList tmpArray = download.split("/");
			QStringList tmpArray2 = tmpArray[tmpArray.count()-1].split("_");
			QString packageName = tmpArray2[0];
			QString packageVersion = tmpArray2[1];
			item->setIcon( 0, QIcon( appDir+"icons/notok.png") );
			item->setText( 1, packageName);
			item->setText( 2, packageVersion);
			item->setText( 3, download);
			downloadTreeWidget->header()->resizeSection(0, 23);
		}
		currentDownload = 0;
		downloadFile();
	}
	else
		installPackagesDpkg();
}


void MainWindow::installPackagesDpkg()
{
	status = "installPackagesDpkg";
	titleLabel->setText("<h3>"+tr("Install packages")+"</h3>");
	QStringList storedDebFiles;
	contentStackedWidget->setCurrentIndex(1);

	QStringList aptArchive = QDir( "/var/cache/apt/archives/" ).entryList( QDir::Files );
	foreach (QString package, newPackages) {
		QStringList tmpArray = aptArchive.filter(package+"_");
		if( tmpArray.count() > 0)
			storedDebFiles.append("/var/cache/apt/archives/"+tmpArray[tmpArray.count()-1]);
	}
	foreach (QString package, updatedPackages) {
		QStringList tmpArray = aptArchive.filter(package+"_");
		if( tmpArray.count() > 0)
			storedDebFiles.append("/var/cache/apt/archives/"+tmpArray[tmpArray.count()-1]);
	}

	outputTextBrowser->clear();
	nextPushButton->setDisabled(TRUE);

	installProcess2 = new QProcess(this);
	output.clear();
	startProgressBar();
	QStringList arguments;
	arguments.append("-i");
	arguments += storedDebFiles;
	QString program = "dpkg";
	connect( installProcess2, SIGNAL(readyReadStandardOutput()),this, SLOT(readOutput()));
	connect( installProcess2, SIGNAL(finished(int)),this, SLOT(processOutput()));
	installProcess2->start(program, arguments );
}

//------------------------------------------------------------------------------
//-- work done -----------------------------------------------------------------
//------------------------------------------------------------------------------

void MainWindow::workDone()
{
	status = "workDone";
	titleLabel->setText("<h3>"+tr("Process finished")+"</h3>");
	nextPushButton->setEnabled(TRUE);
	nextPushButton->setText( tr("close") );
	contentStackedWidget->setCurrentIndex(4);
}

//------------------------------------------------------------------------------
//-- process functions ---------------------------------------------------------
//------------------------------------------------------------------------------



void MainWindow::processFinished()
{
	stopProgressBar();
	nextPushButton->setEnabled(TRUE);
}



void MainWindow::readOutput()
{
	QByteArray result;
	if( status == "aptGetUpdate" )
		result = updateProcess->readAllStandardOutput();
	else if(  status == "removePackages" )
		result = removeProcess1->readAllStandardOutput();
	else if(  status == "removePackagesDpkg" )
		result = removeProcess2->readAllStandardOutput();
	else if(  status == "installPackages" )
		result = installProcess1->readAllStandardOutput();
	else if(  status == "installPackagesDpkg" )
		result = installProcess2->readAllStandardOutput();
	else if(  status == "downloadPackages" )
		result = downloadProcess->readAllStandardOutput();
  
	QStringList lines = QString(result).split("\n");
	foreach (QString line, lines)
		if(line != "")
			output.append(line);

	outputTextBrowser->textCursor().insertText( QString::fromUtf8(result) );
	outputTextBrowser->ensureCursorVisible();
}


void MainWindow::processOutput()
{

	stopProgressBar();
	nextPushButton->setEnabled(TRUE);

	int lineNumber = output.indexOf("The following NEW packages will be installed:") + 1;
	newPackages.clear();
	while( output[lineNumber].left(1) == " ") {
		newPackages += output[lineNumber].replace("  ", "").split(" "); 
		lineNumber++;
	}

	lineNumber = output.indexOf("The following packages will be upgraded:") + 1;
	updatedPackages.clear();
	while( output[lineNumber].left(1) == " ") {
		updatedPackages += output[lineNumber].replace("  ", "").split(" "); 
		lineNumber++;
	}
	lineNumber = output.indexOf("The following packages will be REMOVED:") + 1;
	removedPackages.clear();
	while( output[lineNumber].left(1) == " ") {
		removedPackages += output[lineNumber].replace("  ", "").split(" "); 
		lineNumber++;
	}

	if(newPackages.count() + updatedPackages.count() + removedPackages.count()== 0 ) {
		if(status == "removePackages")
			installPackages();
		else if(status == "installPackages")
			workDone();
	}
	else {
		QStringList allPackages;
		allPackages += newPackages;
		allPackages += updatedPackages;
		allPackages += removedPackages;
		if( allPackages.filter("xorg").count() > 0 or allPackages.filter("xserver").count() > 0 or allPackages.filter("x11").count()  ) {
			QMessageBox::information(this, tr("Error"), tr("It is not allowed to change xserver packages during the xserver is running!") );
			close();
		}

		treeWidget->clear();
		treeWidget->setWordWrap(FALSE);

		if( removedPackages.count() > remove.count() )
			warningLabel->show();

		if( newPackages.count()+updatedPackages.count() > install.count() ) 
			noteLabel->show();

		if( newPackages.count() > 0 ) {
			QTreeWidgetItem *item1 = new QTreeWidgetItem(treeWidget, 0);
			item1->setIcon( 0, QIcon( appDir+"icons/install.png") );
			item1->setText( 0, tr("Packages to install")+"  ("+QString::number(newPackages.count())+"):" );
			foreach (QString package, newPackages) {
				QTreeWidgetItem *item = new QTreeWidgetItem(item1, 0);
				item->setText( 0, package );
				if( install.lastIndexOf(package) == -1 )
					item->setBackground(0, QColor(250, 164, 49, 127) );
			}
		}
		if( updatedPackages.count() > 0 ) {
			QTreeWidgetItem *item2 = new QTreeWidgetItem(treeWidget, 0);
			item2->setIcon( 0, QIcon( appDir+"icons/update.png") );
			item2->setText( 0, tr("Packages to update")+" ("+QString::number(updatedPackages.count())+"):" );
			foreach (QString package, updatedPackages) {
				QTreeWidgetItem *item = new QTreeWidgetItem(item2, 0);;
				item->setText( 0, package );
				if( install.lastIndexOf(package) == -1 )
					item->setBackground(0, QColor(250, 164, 49, 127) );
			}
		}
		if( removedPackages.count() > 0 ) {
			QTreeWidgetItem *item3 = new QTreeWidgetItem(treeWidget, 0);
			item3->setIcon( 0, QIcon( appDir+"icons/remove.png") );
			item3->setText( 0, tr("Packages to remove")+ " ("+QString::number(removedPackages.count())+"):" );
			foreach (QString package, removedPackages) {
				QTreeWidgetItem *item = new QTreeWidgetItem(item3, 0);
				item->setText( 0, package );
				if( remove.lastIndexOf(package) == -1 )
					item->setBackground(0, QColor(241, 76, 76, 127) );
			}
		}

		foreach (QString line, output)
			if(line.contains("http://")) { 
				line = line.split(" ")[0];
				line.replace("'","");
				downloads.append(line);
			}
	} 
}

//------------------------------------------------------------------------------
//-- ProgressBar ---------------------------------------------------------------
//------------------------------------------------------------------------------


void MainWindow::startProgressBar()
{
	pFrame->setGeometry( 2, 2, 100, 26);
	progress = 5;
	connect(timer, SIGNAL(timeout()), this, SLOT(runProgressBar()));
	timer->start(50);
}

void MainWindow::stopProgressBar()
{
	timer->stop();
	pFrame->setGeometry( 2, 2, progressBarFrame->width()-4, 26);
}

void MainWindow::runProgressBar()
{
	if( pFrame->x()+pFrame->width()+5+4 > progressBarFrame->width() and progress > 0)
		progress = 0-5;
	else if( pFrame->x()-5-4 < 0 and progress < 0)
		progress = 5;
	pFrame->setGeometry( pFrame->x()+progress, pFrame->y(), 100, 26);
}



//------------------------------------------------------------------------------
//-- download functions --------------------------------------------------------
//------------------------------------------------------------------------------



 void MainWindow::downloadFile()
 {

	downloadProcess = new QProcess(this);
	output.clear();
	QString program = "wget";
	connect( downloadProcess, SIGNAL(readyReadStandardError()),this, SLOT(updateDownloadStatus()));
	connect( downloadProcess, SIGNAL(finished(int)),this, SLOT(downloadFinished()));
	downloadProcess->setWorkingDirectory("/var/cache/apt/archives");
	downloadProcess->start(program, downloads );


 }

 void MainWindow::updateDownloadStatus()
 {
	QByteArray result= downloadProcess->readAllStandardError();



	QStringList test = QString::fromUtf8(result).split(" ");

	if( currentDownload < downloads.count() )
		if( QString::fromUtf8(result).contains(downloads[currentDownload]) ) {
			    downloadLabel->setText( tr("Downloading")+ " "+downloads[currentDownload]+"." );
			    if(currentDownload > 0) {
					if( downloadTreeWidget->findItems(downloads[currentDownload-1], Qt::MatchExactly, 3 ).count() > 0 ) {
						downloadTreeWidget->findItems(downloads[currentDownload-1], Qt::MatchExactly, 3 ).first()->setIcon(0,QIcon( appDir+"icons/ok.png") );
					 }
			    }
			    currentDownload++;
		}

	if(test.count() > 1 ) {
		if( test[1].contains("%") ) {
			QString percentage = test[1].split("%")[0];
			if( percentage.toInt() < 100 )
 				    downloadProgressBar->setValue(percentage.toInt());
 		}
 	}

 }

 void MainWindow::downloadFinished()
 {
 	downloadProgressBar->setValue(100);
	if( downloadTreeWidget->findItems(downloads[downloads.count()-1], Qt::MatchExactly, 3 ).count() > 0 ) {
		downloadTreeWidget->findItems(downloads[downloads.count()-1], Qt::MatchExactly, 3 ).first()->setIcon(0,QIcon( appDir+"icons/ok.png") );
	}
	nextPushButton->setEnabled(TRUE);
 }



//------------------------------------------------------------------------------
//-- other ---------------------------------------------------------------------
//------------------------------------------------------------------------------

void MainWindow::toggleDetails()
{
	if( outputTextBrowser->isVisible() ) {
		outputTextBrowser->hide();
		detailsPushButton->setText( tr("show details") );
	}
	else {
		outputTextBrowser->show();
		detailsPushButton->setText( tr("hide details") );
	}
}


