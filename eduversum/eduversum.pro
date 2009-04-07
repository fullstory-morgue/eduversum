
TEMPLATE = app
CONFIG += qt
QT += xml

CODECFORSRC = UTF-8
CODECFORTR = ISO-8859-1

OBJECTS_DIR = build/
MOC_DIR = build/
UI_DIR = ui/


TRANSLATIONS = translations/eduversum_de.ts



SOURCES += src/main.cpp \
 src/mainwindow.cpp \
 src/iconloader.cpp \
 src/apploader.cpp
DESTDIR = .

target.path = /usr/bin
iconstarget.path = /usr/share/icons/hicolor/32x32/apps/
iconstarget.files = src/eduversum.png
langtarget.path = /usr/share/eduversum_de/translations/
langtarget.files = translations/eduversum_de_de.qm
shortcuttarget.path = /usr/share/applications
shortcuttarget.files = src/eduversum.desktop

appdata.path = /usr/share/eduversum
appdata.files = data/apps data/categories data/icons data/sh data/manual

INSTALLS += target iconstarget langtarget shortcuttarget appdata

HEADERS += src/mainwindow.h \
src/iconloader.h \
src/apploader.h
TARGET = src/eduversum

FORMS += src/mainwindow.ui

