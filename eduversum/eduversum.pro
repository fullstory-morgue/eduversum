
TEMPLATE = app
CONFIG += qt
QT += xml network

CODECFORSRC = UTF-8
CODECFORTR = ISO-8859-1

OBJECTS_DIR = build/
MOC_DIR = build/
UI_DIR = ui/

TRANSLATIONS = translations/eduversum_de.ts

RESOURCES += eduversum.qrc

SOURCES += src/main.cpp \
 src/eduversum.cpp \
 src/iconloader.cpp \
 src/apploader.cpp \
 src/easyXml.cpp \
 src/packageManager.cpp \
 src/about.cpp \
 src/settings.cpp
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
appdata.files = data/apps data/categories data/weblinks

INCLUDEPATH += /usr/include/PackageKit/packagekit-qt
LIBS += -lpackagekit-qt

INSTALLS += target iconstarget langtarget shortcuttarget appdata doctarget

HEADERS += src/eduversum.h \
 src/iconloader.h \
 src/easyXml.h \
 src/apploader.h \
 src/packageManager.h \
 src/about.h \
 src/settings.h
TARGET = src/eduversum

FORMS += src/eduversum.ui

