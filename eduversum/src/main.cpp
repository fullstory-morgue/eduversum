#include <QTranslator>
#include <QApplication>
#include "eduversum.h"

int main ( int argc, char *argv[] )
{
	QString appName = "eduversum";
	QString locale = QLocale::system().name();
	QTranslator translator;
	translator.load(appName+"_"+locale, "/usr/share/"+appName+"/translations" );

	QApplication app(argc, argv);
        app.installTranslator(&translator);
	Eduversum *mainWindow = new Eduversum();
	mainWindow -> show();
	return app.exec();

}
