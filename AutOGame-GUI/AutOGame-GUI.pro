#-------------------------------------------------
#
# Project created by QtCreator 2014-09-03T14:23:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AutOGame-GUI
TEMPLATE = app


SOURCES += main.cpp\
        autogame.cpp

HEADERS  += autogame.h

FORMS    += autogame.ui

CONFIG += mobility
MOBILITY = 

OTHER_FILES += \
    android/AndroidManifest.xml

RESOURCES += \
    resources.qrc

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

