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

 #include <QApplication>
 #include <QTranslator>
 #include "mainwindow.h"



int main(int argc, char *argv[])
{

	QString appName = "aptosid-apt-qt4";
	QApplication app(argc, argv);
	QStringList args = app.arguments();
        QString locale = QLocale::system().name();
        QTranslator translator;
        translator.load(appName+"_"+locale, "/usr/share/"+appName+"/translations" );
        app.installTranslator(&translator);

	if (getuid() != 0)
		printf("No root\n");
	else if( args.count() < 2 )
		printf("No arguments!\n");
	else {
		MainWindow *mainWin = new MainWindow( args );; 
		mainWin->show();
		return app.exec();
	}


 } 
