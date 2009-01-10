
TEMPLATE = app
CONFIG += qt
QT += network

CODECFORSRC = UTF-8
CODECFORTR = ISO-8859-1

OBJECTS_DIR = build/
MOC_DIR = build/
UI_DIR = ui/



SOURCES += src/main.cpp \
 src/mainwindow.cpp
DESTDIR = .

target.path = /usr/bin
data.path = /usr/share/eduversum-installer
data.files = icons sh

INSTALLS += target data

HEADERS += src/mainwindow.h
TARGET = src/eduversum-installer

FORMS += src/mainwindow.ui

