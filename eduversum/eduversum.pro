
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
 src/eduversum.cpp \
 src/iconloader.cpp \
 src/apploader.cpp \
 src/easyXml.cpp
DESTDIR = .

target.path = /usr/bin
iconstarget.path = /usr/share/icons/hicolor/scalable/apps/
iconstarget.files = src/eduversum.svgz
langtarget.path = /usr/share/eduversum/translations/
langtarget.files = translations/eduversum_de.qm
shortcuttarget.path = /usr/share/applications
shortcuttarget.files = src/eduversum.desktop
doctarget.path = /usr/share/doc/eduversum
doctarget.files = doc/*

appdata.path = /usr/share/eduversum
appdata.files = data/apps data/categories data/icons data/sh data/manual data/weblinks

INSTALLS += target iconstarget langtarget shortcuttarget appdata doctarget

HEADERS += src/eduversum.h \
src/iconloader.h \
src/easyXml.h \
src/apploader.h
TARGET = src/eduversum

FORMS += src/eduversum.ui

