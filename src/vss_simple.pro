#-------------------------------------------------
#
# Project created by QtCreator 2014-03-21T12:58:47
#
#-------------------------------------------------

QT       += core gui


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = vss_simple
TEMPLATE = app

INCLUDEPATH += "/home/sst-admin/bin/inc/win2003/"

QMAKE_CXXFLAGS += -std=c++11 -w -static-libgcc

win32: LIBS += -lvssapi -lole32 -luuid

#win32: LIBS += -lvss_uuid

SOURCES += main.cpp\
        mainwindow.cpp \
    counter.cpp \
    shadow_copy.cpp

HEADERS  += mainwindow.h \
    counter.h \
    shadow_copy.h \
    error.h

FORMS    += mainwindow.ui
