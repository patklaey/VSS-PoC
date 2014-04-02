#-------------------------------------------------
#
# Project created by QtCreator 2014-03-21T12:58:47
#
#-------------------------------------------------

QT       += core gui


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = vss_simple
TEMPLATE = app

INCLUDEPATH += "C:\\Program Files\\Microsoft SDKs\\Windows\\v7.1\\Include"
INCLUDEPATH += "C:\\Program Files\\Microsoft Visual Studio 12.0\\VC\\include"

win32: LIBS += -lvssapi

win32: LIBS += -lvss_uuid

SOURCES += main.cpp\
        mainwindow.cpp \
    counter.cpp \
    shadow_copy.cpp

HEADERS  += mainwindow.h \
    counter.h \
    shadow_copy.h \
    error.h

FORMS    += mainwindow.ui




