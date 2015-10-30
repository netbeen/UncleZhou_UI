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
    tooloptionframe.cpp \
    palettedock.cpp \
    paletteitem.cpp \
    densitypeakdialog.cpp \
    readonlycanvas.cpp \
    densitypeakcanvas.cpp

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
    tooloptionframe.h \
    palettedock.h \
    paletteitem.h \
    densitypeakdialog.h \
    readonlycanvas.h \
    densitypeakcanvas.h

FORMS    +=

RESOURCES += \
    resource.qrc

LIBS += /usr/local/lib/libopencv_core.so    \
		/usr/local/lib/libopencv_imgproc.so \
		/usr/local/lib/libopencv_highgui.so \
		/usr/local/lib/libopencv_objdetect.so \
		/usr/local/lib/libopencv_video.so	\
		/usr/local/lib/libopencv_videoio.so	\
		/usr/local/lib/libopencv_imgcodecs.so	\
		/usr/local/lib/libopencv_flann.so		\
