#-------------------------------------------------
#
# Project created by QtCreator 2015-09-20T17:32:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -Wl,--stack,4194304000

TARGET = UncleZhou_UI
TEMPLATE = app

CONFIG += c++11

QMAKE_CXXFLAGS+= -fopenmp
QMAKE_LFLAGS +=  -fopenmp

INCLUDEPATH += eigen-eigen-b30b87236a1b

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
    densitypeakcanvas.cpp \
    viewpatchdistributedialog.cpp \
    viewpatchdistributecavvas.cpp \
    binaryclassificationdialog.cpp \
	Classification/GetImageFeatures.cpp \
	Classification/MySharkML.cpp \
    graphcut/graphcut.cpp \
    graphcut/gmm.cpp \
    graphcut/gco-v3.0/GCoptimization.cpp \
    graphcut/gco-v3.0/graph.cpp \
    graphcut/gco-v3.0/LinkedBlockList.cpp \
    graphcut/gco-v3.0/maxflow.cpp \
    SuperPixel/readsuperpixeldat.cpp \
    SuperPixel/SLICSuperpixels/SLIC.cpp \
	Classification/MyClassification.cpp \
    multilabelpreivewdock.cpp \
    undostack.cpp \
    Classification/BuildGraph.cpp \
    Classification/MyImageProc.cpp \
    Classification/MyMR8FilterBank.cpp \
    Classification/Saliency.cpp \
    Classification/SuperPixelGraph.cpp \
    gaussianfunctionpainter.cpp \
    gausssiandialog.cpp


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
    densitypeakcanvas.h \
    viewpatchdistributedialog.h \
    viewpatchdistributecavvas.h \
    binaryclassificationdialog.h \
	Classification/GetImageFeatures.h \
	Classification/MySharkML.h \
    graphcut/graphcut.h \
    graphcut/gmm.h \
    graphcut/gco-v3.0/block.h \
    graphcut/gco-v3.0/energy.h \
    graphcut/gco-v3.0/GCoptimization.h \
    graphcut/gco-v3.0/graph.h \
    graphcut/gco-v3.0/LinkedBlockList.h \
    SuperPixel/readsuperpixeldat.h \
    SuperPixel/SLICSuperpixels/SLIC.h \
	Classification/MyClassification.h \
    multilabelpreivewdock.h \
    undostack.h \
    Classification/MyImageProc.h \
    Classification/MyMR8FilterBank.h \
    Classification/Saliency.h \
    Classification/SuperPixelGraph.h \
    gaussianfunctionpainter.h \
    gausssiandialog.h


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
		/usr/local/lib/*.so	\
		/usr/local/lib/libshark.a \
		/home/netbeen/git_repo/Shark/build/lib/libshark.a


