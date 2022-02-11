#-------------------------------------------------
#
# Project created by QtCreator 2022-01-01T00:39:24
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include( QProgressIndicator/qprogressindicator.pri )
include( qjson4/QJson4.pri )

TARGET = GuiderII-klipper
TEMPLATE = app

target.path = /root
INSTALLS += target

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
QMAKE_CXXFLAGS += -std=c++0x


SOURCES += \
    main.cpp \
    mainwindow.cpp \
    dialogwindow.cpp \
    openfilewindow.cpp \
    simpleserver.cpp \
    buzzerthread.cpp \
    buttonthread.cpp \
    systemcommandswindow.cpp

HEADERS += \
    mainwindow.h \
    dialogwindow.h \
    openfilewindow.h \
    simpleserver.h \
    buzzerthread.h \
    buttonthread.h \
    systemcommandswindow.h

FORMS += \
    mainwindow.ui \
    dialogwindow.ui \
    openfilewindow.ui \
    systemcommandswindow.ui

