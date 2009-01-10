
TEMPLATE = app
CONFIG += qt
QT += network

CODECFORSRC = UTF-8
CODECFORTR = ISO-8859-1

OBJECTS_DIR = build/
MOC_DIR = build/
UI_DIR = ui/


#FORMS =
#TRANSLATIONS = qbat_de.ts



SOURCES += src/main.cpp \
 src/mainwindow.cpp
DESTDIR = .

target.path = /usr/bin
#iconstarget.path = /usr/share/icons/hicolor/32x32/apps/
#iconstarget.files = src/eduversum.png
#langtarget.path = /usr/share/eduversum/lang
#langtarget.files = eduversum_*.qm
#shortcuttarget.path = /usr/share/applications
#shortcuttarget.files = src/eduversum.desktop

#appdata.path = /usr/share/eduversum
#appdata.files = data/apps data/categories data/icons data/sh

INSTALLS += target #iconstarget langtarget shortcuttarget appdata

HEADERS += src/mainwindow.h
TARGET = src/eduversum-installer

FORMS += src/mainwindow.ui

