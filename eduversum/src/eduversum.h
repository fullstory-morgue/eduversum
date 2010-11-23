#ifndef EDUVERSUM_H
#define EDUVERSUM_H

#include "apploader.h"
#include "about.h"
#include "packageManager.h"

#include "QMenu"
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QMessageBox>
#include <QDomElement>
#include <QHttp>
#include <QFile>


#include "ui_eduversum.h"

class Eduversum : public QWidget, Ui::Eduversum
{
        Q_OBJECT

        public:
                Eduversum(QWidget* parent = 0, Qt::WFlags flags = 0);

                void loadGui();
                void unsetGui();
                void loadData();

                QString appdir, iconpath, help;
                QStringList alreadyDownloadedThumbmnails;

                AppLoader *apps;
                PackageManager *pm;



        public slots:
                virtual void showCategoryApps();
                virtual void searchApps();
                virtual void showApp(QTreeWidgetItem*);
                virtual void execApp();
                virtual void showHomepage();
                virtual void copyExample();
                virtual void copyDir(QString, QString);
                virtual void showAbout();
                virtual void setHelp();
                virtual void showHelp();
                virtual void showDisclaimer();
                virtual void showAppsPage();
                virtual void showDocsPage();
                virtual void showDoc();
                virtual void showDocDescription();
                virtual void showWeblinkPage();
                virtual void showWebDescription();
                virtual void showWeblink();
                virtual void installRemove();
                virtual void installRemoveFinished();



        private slots:
                void iconActivated(QSystemTrayIcon::ActivationReason reason);
                void downloadThumbnail(QString);
                void httpRequestFinished(int requestId, bool error);
                void formatAppDescription();


        private:
                QAction *minimizeAction;
                QAction *maximizeAction;
                QAction *restoreAction;
                QAction *quitAction;
                QAction *aboutAction;
                QAction *disclaimerAction;
                QAction *stuffAction;
                QSystemTrayIcon *trayIcon;
                QMenu *trayIconMenu;

                QHttp *http;
                QFile *file;
                bool httpRequestAborted;
                int httpGetId;


        protected:
                void closeEvent(QCloseEvent *event);


};

#endif
