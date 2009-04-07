
#include <QApplication>
#include "eduversum.h"

int main ( int argc, char *argv[] )
{

	QApplication app(argc, argv);
	Eduversum *mainWindow = new Eduversum();
	mainWindow -> show();
	return app.exec();

}
