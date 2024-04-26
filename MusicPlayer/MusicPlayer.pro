#-------------------------------------------------
#
# Project created by QtCreator 2023-11-27T15:16:30
#
#-------------------------------------------------

QT       += core gui
QT += multimedia
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MusicPlayer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    musicset.cpp

HEADERS  += mainwindow.h \
    musicset.h

FORMS    += mainwindow.ui \
    musicset.ui

RESOURCES += \
    res.qrc
