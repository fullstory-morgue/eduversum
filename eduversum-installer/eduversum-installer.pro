
TEMPLATE = app
CONFIG += qt
QT += network

CODECFORSRC = UTF-8
CODECFORTR = ISO-8859-1

OBJECTS_DIR = build/
MOC_DIR = build/
UI_DIR = ui/

TRANSLATIONS = eduversum-installer_de.ts

SOURCES += src/main.cpp \
 src/mainwindow.cpp
DESTDIR = .

target.path = /usr/bin
data.path = /usr/share/eduversum-installer
data.files = icons sh
langtarget.path = /usr/share/eduversum-installer/translations/
langtarget.files = eduversum-installer_de.qm

INSTALLS += target data langtarget

HEADERS += src/mainwindow.h
TARGET = src/eduversum-installer

FORMS += src/mainwindow.ui

