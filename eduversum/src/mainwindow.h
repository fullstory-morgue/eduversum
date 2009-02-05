#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "iconloader.h"
#include "QMenu"
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QMessageBox>



#include "ui_mainwindow.h"

class MainWindow : public QWidget, Ui::eduversumGui
{
	Q_OBJECT

	public:
		MainWindow (QWidget* parent = 0, Qt::WFlags flags = 0);

		void loadGui();
		void unsetGui();
		void loadData();
		void showAppInList(QTreeWidgetItemIterator);


		QString appdir;
		QString iconpath;
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
		virtual void showAbout();
		virtual void showStuff();


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

	protected:
		void closeEvent(QCloseEvent *event);




};

#endif
