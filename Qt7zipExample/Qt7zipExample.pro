#-------------------------------------------------
#
# Project created by QtCreator 2019-03-23T19:03:23
#
#-------------------------------------------------

TARGET = Qt7zipExample
TEMPLATE = app

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

DESTDIR += bin
UI_DIR += ui
MOC_DIR += moc
RCC_DIR += rcc
OBJECTS_DIR += obj

SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui

CODECFORSRC = UTF-8

include(../Qt7zip/Qt7zip.pri)
