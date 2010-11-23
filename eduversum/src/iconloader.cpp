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
#include "settings.h"


IconLoader::IconLoader()
{
    iconpath  = "/usr/share/icons/hicolor/";
    iconpath2 = "/usr/share/icons/crystalsvg/";
    appdir    = Settings::applicationPath();
}


QString IconLoader::getIconPath(QString iconname)
{
    if( QFile::exists(iconname) ) { return iconname;}
    iconname = iconname.replace(".png", "").replace(".xpm", "");
    QStringList paths;
    paths.append(iconpath+"48x48/apps/"+iconname+".png");
    paths.append(iconpath+"32x32/apps/"+iconname+".png");
    paths.append(iconpath2+"32x32/apps/"+iconname+".png");
    paths.append("/usr/share/pixmaps/"+iconname+".xpm");
    paths.append("/usr/share/pixmaps/"+iconname+"-icon.xpm");
    paths.append("/usr/share/pixmaps/"+iconname+".png");
    paths.append("/usr/share/icons/"+iconname+".png");
    paths.append("/usr/share/"+iconname+"/"+iconname+".xpm");
    paths.append("/usr/share/"+iconname+"/pixmaps/"+iconname+".xpm");
    paths.append("/usr/share/"+iconname+"/icons/"+iconname+".ico");
    paths.append("/usr/share/"+iconname+"/icons/"+iconname+".xpm");
    paths.append("/usr/share/"+iconname+"/images/icons/icon.png");
    paths.append("/usr/share/app-install/icons/"+iconname+".png");
    paths.append("/usr/share/app-install/icons/"+iconname+".xpm");
    paths.append("/usr/share/app-install/icons/"+iconname+".svg");
    foreach(QString path, paths) {
        if( QFile::exists( path ) ) {return path;}
    }
    return "";
}


QPixmap IconLoader::getIcon(QString iconPath)
{
    QImage ico;
    if( QFile::exists( iconPath.replace("48x48","32x32") ) ) {
        ico = QImage(iconPath);
    } else if( QFile::exists(iconPath) ) {
        ico = QImage(iconPath);
    } else {
        ico = QImage(":/res/empty.png");
    }

    ico.scaledToHeight( 32, Qt::SmoothTransformation );
    QPixmap icon = QPixmap::fromImage(ico);
    return icon;
}
