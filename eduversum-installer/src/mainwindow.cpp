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
	process = new QProcess(this);
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
	titleLabel->setText("<h3>Welcome</h3>");
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
	else {
		int i = contentStackedWidget->currentIndex()+1;
		if( i == contentStackedWidget->count() )
			i = 0;
		contentStackedWidget->setCurrentIndex(i);
	}
}


//------------------------------------------------------------------------------
//-- update package list -------------------------------------------------------
//------------------------------------------------------------------------------

void MainWindow::aptGetUpdate()
{

	if( radioButton->isChecked() ) {
		status = "aptGetUpdate";
		titleLabel->setText("<h3>Resynchronize the package index files</h3>");
		contentStackedWidget->setCurrentIndex(1);
		nextPushButton->setDisabled(TRUE);

		output.clear();
		startProgressBar();
		QStringList arguments;
		arguments.append("update");
		QString program = "apt-get";
		connect( process, SIGNAL(readyReadStandardOutput()),this, SLOT(readOutput()));
		connect( process, SIGNAL(finished(int)),this, SLOT(processFinished()));
		process->start(program, arguments );
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
		titleLabel->setText("<h3>Remove packages</h3>");
		contentStackedWidget->setCurrentIndex(2);
		nextPushButton->setDisabled(TRUE);

		QTreeWidgetItem *item1 = new QTreeWidgetItem(treeWidget, 0);
		item1->setIcon( 0, QIcon( appDir+"icons/wait.png") );
		item1->setText( 0, "Please wait..." );

		output.clear();
		QStringList arguments;
		arguments.append("remove");
		arguments += remove;
		QString program = appdir+"sh/getDep.sh";
		connect( process, SIGNAL(readyReadStandardOutput()),this, SLOT(readOutput()));
		connect( process, SIGNAL(finished(int)),this, SLOT(processOutput()));
		process->start(program, arguments );
	}
	else
		installPackages();
}

void MainWindow::removePackagesDpkg()
{
	status = "removePackagesDpkg";
	titleLabel->setText("<h3>Remove packages</h3>");
	contentStackedWidget->setCurrentIndex(1);
	nextPushButton->setDisabled(TRUE);
	outputTextBrowser->clear();

	output.clear();
	startProgressBar();
	QStringList arguments;
	arguments.append("-r");
	arguments += removedPackages;
	QString program = "dpkg";
	connect( process, SIGNAL(readyReadStandardOutput()),this, SLOT(readOutput()));
	connect( process, SIGNAL(finished(int)),this, SLOT(processFinished()));
	process->start(program, arguments );



}

//------------------------------------------------------------------------------
//-- install packages ----------------------------------------------------------
//------------------------------------------------------------------------------

void MainWindow::installPackages()
{

	if( install.count() > 0 ) {
		status = "installPackages";
		titleLabel->setText("<h3>Install packages</h3>");
		contentStackedWidget->setCurrentIndex(2);
		nextPushButton->setDisabled(TRUE);


		treeWidget->clear();
		QTreeWidgetItem *item1 = new QTreeWidgetItem(treeWidget, 0);
		item1->setIcon( 0, QIcon( appDir+"icons/wait.png") );
		item1->setText( 0, "Please wait..." );
	
		output.clear();
		QStringList arguments;
		arguments.append("install");
		arguments += install;
		QString program = appDir+"sh/getDep.sh";
		connect( process, SIGNAL(readyReadStandardOutput()),this, SLOT(readOutput()));
		connect( process, SIGNAL(finished(int)),this, SLOT(processOutput()));
		process->start(program, arguments );
	}
	else
		workDone();
}

void MainWindow::downloadPackages()
{
	if( downloads.count() > 0 ) {
		status = "downloadPackages";
		titleLabel->setText("<h3>Download packages</h3>");
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
			item->setText( 0, packageName);
			item->setText( 1, packageVersion);
			item->setText( 2, download);
			item->setIcon( 0, QIcon( appDir+"icons/notok.png") );
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
	titleLabel->setText("<h3>Install packages</h3>");
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

	output.clear();
	startProgressBar();
	QStringList arguments;
	arguments.append("-i");
	arguments += storedDebFiles;
	QString program = "dpkg";
	connect( process, SIGNAL(readyReadStandardOutput()),this, SLOT(readOutput()));
	connect( process, SIGNAL(finished(int)),this, SLOT(processOutput()));
	process->start(program, arguments );

}

//------------------------------------------------------------------------------
//-- work done -----------------------------------------------------------------
//------------------------------------------------------------------------------

void MainWindow::workDone()
{
	status = "workDone";
	titleLabel->setText("<h3>Process finished</h3>");
	nextPushButton->setEnabled(TRUE);
	nextPushButton->setText("close");
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
	QByteArray result= process->readAllStandardOutput();
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
			QMessageBox::information(this, tr("Error"), "It is not allowed to change xserver packages during the xserver is running!");
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
			item1->setText( 0, "Packages to install ("+QString::number(newPackages.count())+"):" );
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
			item2->setText( 0, "Packages to update ("+QString::number(updatedPackages.count())+"):" );
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
			item3->setText( 0, "Packages to remove ("+QString::number(removedPackages.count())+"):" );
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
				QStringList tmpArray = line.split("/");
				QString fileName = tmpArray[tmpArray.count()-1];
				if( !QFile::exists("/var/cache/apt/archives/"+fileName) ) {
					downloads.append(line);
				}
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


	connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(downloadFinished(int, bool)));
	connect(http, SIGNAL(dataReadProgress(int, int)), this, SLOT(updateDataReadProgress(int, int)));
	connect(http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)), this, SLOT(readResponseHeader(const QHttpResponseHeader &)));

	QUrl url(downloads[currentDownload]);
	QFileInfo fileInfo(url.path());
	QString fileName = "/var/cache/apt/archives/"+fileInfo.fileName();

	if (QFile::exists(fileName))
		return;

	QFile *file = new QFile(fileName);
	if (!file->open(QIODevice::WriteOnly)) {
		QMessageBox::information(this, tr("Error"), tr("Unable to save the file %1: %2.").arg(fileName).arg(file->errorString()));
		delete file;
		file = 0;
		return;
	}

	QHttp::ConnectionMode mode = url.scheme().toLower() == "https" ? QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp;
	http->setHost(url.host(), mode, url.port() == -1 ? 0 : url.port());


	httpRequestAborted = false;
	QByteArray path = QUrl::toPercentEncoding(url.path(), "!$&'()*+,;=:@/");
	if (path.isEmpty())
		path = "/";
	httpGetId = http->get(path, file);

	downloadLabel->setText(tr("Downloading %1.").arg(fileName));

 }

 void MainWindow::cancelDownload()
 {
	//statusLabel->setText(tr("Download canceled."));
	httpRequestAborted = true;
	http->abort();
 }

void MainWindow::downloadFinished(int requestId, bool error)
{

	if (requestId != httpGetId)
		return;
	if ( error ) 
		QMessageBox::information( this, tr( "Error" ), tr( "Download failed: %1." ).arg( http->errorString() ));
	else {
		if ( downloadTreeWidget->findItems(downloads[currentDownload], Qt::MatchExactly, 2 ).count() > 0 ) {
			downloadTreeWidget->findItems(downloads[currentDownload], Qt::MatchExactly, 2 ).first()->setIcon( 0, QIcon( appDir+"icons/ok.png") );
		}
		http->close();
		currentDownload++;
		if(currentDownload < downloads.count())
			downloadFile();
		else
			nextPushButton->setEnabled(TRUE);
	}
 }


 void MainWindow::readResponseHeader(const QHttpResponseHeader &responseHeader)
 {
	switch (responseHeader.statusCode()) {
		case 200:                   // Ok
		case 301:                   // Moved Permanently
		case 302:                   // Found
		case 303:                   // See Other
		case 307:                   // Temporary Redirect
		// these are not error conditions
		break;

		default:
			QMessageBox::information(this, tr("Error"), tr("Download failed: %1.").arg(responseHeader.reasonPhrase()));
			httpRequestAborted = true;
			//progressDialog->hide();
			http->abort();
	}
 }

void MainWindow::updateDataReadProgress(int bytesRead, int totalBytes)
{
	if (httpRequestAborted)
		return;

	downloadProgressBar->setMaximum(totalBytes);
	downloadProgressBar->setValue(bytesRead);
	
		
}



//------------------------------------------------------------------------------
//-- other ---------------------------------------------------------------------
//------------------------------------------------------------------------------

void MainWindow::toggleDetails()
{
	if( outputTextBrowser->isVisible() ) {
		outputTextBrowser->hide();
		detailsPushButton->setText("show details");
	}
	else {
		outputTextBrowser->show();
		detailsPushButton->setText("hide details");
	}
}


