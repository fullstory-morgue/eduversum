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

#include <QFile>
#include <QDir>
#include <QDomDocument>


#include "iconloader.h"

IconLoader::IconLoader()
{
	iconpath = "/usr/share/icons/hicolor/";
	appdir      = "/usr/share/eduversum/";
}


QPixmap IconLoader::getIcon(QString iconname)
{
	iconname = iconname.replace(".png", "").replace(".xpm", "");

	QImage ico;


	if( QFile::exists(iconpath+"32x32/apps/"+iconname+".png") )
		ico = QImage(iconpath+"32x32/apps/"+iconname+".png");
	else if( QFile::exists(iconpath+"48x48/apps/"+iconname+".png") )
		ico = QImage(iconpath+"48x48/apps/"+iconname+".png");
	else if( QFile::exists("/usr/share/pixmaps/"+iconname+".xpm") )
		ico = QImage("/usr/share/pixmaps/"+iconname+".xpm");
	else if( QFile::exists("/usr/share/pixmaps/"+iconname+"-icon.xpm") )
		ico = QImage("/usr/share/pixmaps/"+iconname+"-icon.xpm");
	else if( QFile::exists("/usr/share/pixmaps/"+iconname+".png") )
		ico = QImage("/usr/share/pixmaps/"+iconname+".png");
	else if( QFile::exists("/usr/share/icons/"+iconname+".png") )
		ico = QImage("/usr/share/icons/"+iconname+".png");
	else if( QFile::exists("/usr/share/"+iconname+"/"+iconname+".xpm") )
		ico = QImage("/usr/share/"+iconname+"/"+iconname+".xpm");
	else if( QFile::exists("/usr/share/"+iconname+"/pixmaps/"+iconname+".xpm") )
		ico = QImage("/usr/share/"+iconname+"/pixmaps/"+iconname+".xpm");
	else if( QFile::exists("/usr/share/"+iconname+"/icons/"+iconname+".ico") )
		ico = QImage("/usr/share/"+iconname+"/icons/"+iconname+".ico");
	else if( QFile::exists("/usr/share/"+iconname+"/icons/"+iconname+".xpm") )
		ico = QImage("/usr/share/"+iconname+"/icons/"+iconname+".xpm");
	else if( QFile::exists ( "/usr/share/app-install/+icons/"+iconname+".png" ) )
		ico = QImage( "/usr/share/app-install/icons/"+iconname+".png" );
	else if( QFile::exists( "/usr/share/app-install/icons/"+iconname+".xpm" ) )
		ico = QImage( "/usr/share/app-install/icons/"+iconname+".xpm" );
	else
	{
		ico = QImage( appdir+"icons/empty.png" );
	}

	ico.scaledToHeight( 32, Qt::SmoothTransformation );
	QPixmap icon = QPixmap::fromImage(ico);
	return icon;
}
