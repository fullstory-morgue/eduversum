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


#ifndef ICONLOADER_H
#define ICONLOADER_H

#include <QTreeWidget>

class IconLoader;

class IconLoader
{
	public:
		IconLoader();
		QPixmap getIcon(QString);
                QString getIconPath(QString);
		QString appdir;
		QString iconpath;
		QString iconpath2;

};

#endif
