
TEMPLATE = app
CONFIG += qt
QT += network

CODECFORSRC = UTF-8
CODECFORTR = ISO-8859-1

OBJECTS_DIR = build/
MOC_DIR = build/
UI_DIR = ui/

TRANSLATIONS = translations/sidux-apt-qt4_de.ts

SOURCES += src/main.cpp \
 src/mainwindow.cpp
DESTDIR = .

target.path = /usr/bin
data.path = /usr/share/sidux-apt-qt4
data.files = icons sh
langtarget.path = /usr/share/sidux-apt-qt4/translations/
langtarget.files = translations/sidux-apt-qt4_de.qm

INSTALLS += target data langtarget

HEADERS += src/mainwindow.h
TARGET = src/sidux-apt-qt4

FORMS += src/mainwindow.ui

