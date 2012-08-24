#-------------------------------------------------
#
# Project created by QtCreator 2012-08-04T14:43:56
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = 3mview
TEMPLATE = app
CONFIG += static


SOURCES += main.cpp\
        mainwindow.cpp \
    aboutdialog.cpp \
    glwidget.cpp

HEADERS  += mainwindow.h \
    aboutdialog.h \
    glwidget.h
INCLUDEPATH += \
    /usr/include/openbabel-2.0
LIBS += \
        -lopenbabel
FORMS    += mainwindow.ui \
    aboutdialog.ui
