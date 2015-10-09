#-------------------------------------------------
#
# Project created by QtCreator 2015-09-20T17:32:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UncleZhou_UI
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    newimagedialog.cpp \
    util.cpp \
    imageeditwindow.cpp \
    swatch.cpp \
    layerdock.cpp \
    canvas.cpp \
    layeritem.cpp \
    layermanager.cpp \
    navigatordock.cpp \
    navigatorcanvas.cpp \
    tooloptiondock.cpp \
    tooloptionframe.cpp

HEADERS  += mainwindow.h \
    newimagedialog.h \
    util.h \
    imageeditwindow.h \
    swatch.h \
    layerdock.h \
    canvas.h \
    layeritem.h \
    layermanager.h \
    navigatordock.h \
    navigatorcanvas.h \
    tooloptiondock.h \
    tooloptionframe.h

FORMS    +=

RESOURCES += \
    resource.qrc
