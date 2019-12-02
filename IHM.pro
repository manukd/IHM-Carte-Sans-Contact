#-------------------------------------------------
#
# Project created by QtCreator 2019-10-04T13:27:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IHM
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui

LIBS += -LC:\\Users\\ed494244\\Desktop\\TP_ComSansFil\\IHM -lODALID

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../Librairie+sample/Librairie+sample/MSVC2010/ -lODALI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Librairie+sample/Librairie+sample/MSVC2010/ -lODALId
else:unix: LIBS += -L$$PWD/../Librairie+sample/Librairie+sample/MSVC2010/ -lODALI

INCLUDEPATH += $$PWD/../Librairie+sample/Librairie+sample/MSVC2010
DEPENDPATH += $$PWD/../Librairie+sample/Librairie+sample/MSVC2010
