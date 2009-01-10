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

 #ifndef MAINWINDOW_H
 #define MAINWINDOW_H

 #include <QDialog>

 #include <QDialogButtonBox>
 #include <QFile>
 #include <QHttp>
 #include <QHttpResponseHeader>
 #include <QLabel>
 #include <QLineEdit>
  #include <QProgressDialog>
  #include <QPushButton>
  #include <QSslError>
  #include <QAuthenticator>


#include "QMenu"
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QMessageBox>
#include <QProcess>
#include <QHttp>
#include <QFile>
#include <QProgressDialog>
#include <QDialogButtonBox>


#include "ui_mainwindow.h"

class MainWindow : public QWidget, Ui::eduversumGui
 {
     Q_OBJECT

	public:
		MainWindow(QStringList, QWidget* parent = 0, Qt::WFlags flags = 0);
		QString status;
		int progress, currentDownload;
		QTimer *timer;
		QProcess *process;
		QStringList output, install, remove, newPackages, updatedPackages, removedPackages, downloads;


	private slots:
		void next();

		void aptGetUpdate();

		void removePackages();
		void removePackagesDpkg();

		void installPackages();
		void downloadPackages();
		void installPackagesDpkg();

		void workDone();

		// process functions
		void readOutput();
		void processOutput();
		void processFinished();

		// progress bar
		void startProgressBar();
		void stopProgressBar();
		void runProgressBar();

		// download functions
		void downloadFile();
		void cancelDownload();
		void downloadFinished(int, bool);
		void readResponseHeader(const QHttpResponseHeader &responseHeader);
		void updateDataReadProgress(int bytesRead, int totalBytes);

		void toggleDetails();


	private:
		QProgressDialog *progressDialog;
		QHttp *http;
		int httpGetId;
		bool httpRequestAborted;
 };

 #endif