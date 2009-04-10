#ifndef EDUVERSUM_H
#define EDUVERSUM_H

#include "iconloader.h"
#include "QMenu"
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QMessageBox>



#include "ui_eduversum.h"

class Eduversum : public QMainWindow, Ui::Eduversum
{
	Q_OBJECT

	public:
		Eduversum (QMainWindow* parent = 0, Qt::WFlags flags = 0);

		void loadGui();
		void unsetGui();
		void loadData();
		void showAppInList(QTreeWidgetItemIterator);

		QString appdir, iconpath;
		QString about, help;
		QString addRemoveApp;
		QTreeWidget *appTreeWidget;
		IconLoader *iconloader;

 
	public slots:
		virtual void showCategoryApps();
		virtual void searchApps();
		virtual void showApp();
		virtual void execApp();
		virtual void showHomepage();
		virtual void copyExample();
		virtual void copyDir(QString, QString);
		virtual void changed();
		virtual void applyChanges();
		virtual void applyChangesFinished();
		virtual void cancelChanges();
		virtual void discardChanges();
		virtual void showChanges();
		virtual void setAbout();
		virtual void showAbout();
		virtual void setHelp();
		virtual void showHelp();
		virtual void showStuff();
		virtual void showSeminarixLatex();
		virtual void showOpenSource();


	private slots:
		void iconActivated(QSystemTrayIcon::ActivationReason reason);


	private:
		QAction *minimizeAction;
		QAction *maximizeAction;
		QAction *restoreAction;
		QAction *quitAction;
		QAction *aboutAction;
		QAction *stuffAction;
		QSystemTrayIcon *trayIcon;
		QMenu *trayIconMenu;
		QString chooseFilenamanager();

	protected:
		void closeEvent(QCloseEvent *event);


};

#endif
