/*
 * edu.cpp
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


#ifndef APPS_H
#define APPS_H

#include <QTreeWidget>
#include "iconloader.h"

class AppLoader
{
    public:
        AppLoader();
        QList<QTreeWidgetItem *> list();
        QList<QTreeWidgetItem *> list(QString, int);
        QList<QTreeWidgetItem *> search(QString);
        QList<QTreeWidgetItem *> importCategories();
        QTreeWidgetItem* find(QString);
        QString thumbnailPath(QString);

        void importApps();
        void importApp(QString,QString);
        void changePackageStatus(QTreeWidgetItem*, QString);
        void setDescription(QTreeWidgetItem*, QString);

        IconLoader *iconloader;
        QString appdir;
        QString language;
        QString getDescription(QString);
        QList<QTreeWidgetItem *> apps;

};

#endif
