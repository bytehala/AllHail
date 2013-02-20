#-------------------------------------------------
#
# Project created by QtCreator 2013-02-11T14:18:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AllHail
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    newprojectdialog.cpp \
    textureregiondialog.cpp \
    testoutputdialog.cpp

HEADERS  += mainwindow.h \
    newprojectdialog.h \
    textureregiondialog.h \
    testoutputdialog.h

FORMS    += mainwindow.ui \
    newprojectdialog.ui \
    textureregiondialog.ui \
    testoutputdialog.ui

DEFINES *= QT_USE_QSTRINGBUILDER
