#include "imageeditwindow.h"

#include <QGridLayout>
#include <QIcon>

ImageEditWindow::ImageEditWindow(config::editPosition editPosition, config::editLevel editLevel, QWidget* parent) : QMainWindow(parent)
{
    this->layerManager = LayerManager::getInstance();
    this->layerManager->init(editPosition);

    this->initWindowLayout();
    this->initActions();


    //对画板填充内容
    this->canvas->init(editPosition);
}


void ImageEditWindow::initWindowLayout(){
    this->setWindowState(Qt::WindowMaximized);      //设置窗口最大化
    this->setStyleSheet("background-color: #333337; color: #f1f1f1;");


    this->toolbar = new QToolBar("Toolbar",this);
    this->addToolBar(Qt::LeftToolBarArea,this->toolbar);

    this->layerDock = new LayerDock(this);
    this->layerDock->setWindowTitle("Layer");
    this->layerDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    this->addDockWidget(Qt::RightDockWidgetArea, this->layerDock);


    this->canvas = new Canvas(this);
    this->canvas->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    this->scrollArea = new QScrollArea(this);
    this->scrollArea->setBackgroundRole(QPalette::Dark);
    this->scrollArea->setFrameShape(QFrame::NoFrame);   //消除边框
    this->scrollArea->setWidget(this->canvas);
    this->scrollArea->setWidgetResizable(true);
    this->scrollArea->setAlignment(Qt::AlignHCenter);

    this->setCentralWidget(this->scrollArea);


    //设置状态栏
    this->statusBar()->setStyleSheet("QStatusBar{background-color: #535353;}QStatusBar::item{border:0px}QSizeGrip{background-color: #535353;}");

    //this->menuBar()->setStyleSheet(" QMenuBar{background-color: #535353; padding-left: 5px;}QMenuBar::item {background-color: #535353; padding:2px; margin:6px 10px 0px 0px;} QMenuBar::item:selected {background: #3e3e40;} QMenuBar::item:pressed {background: #1b1b1c;}");




}


void ImageEditWindow::initActions(){

    this->moveAction = new QAction(QIcon(":/image/move.png"),"&Move",this);
    this->pencilAction= new QAction(QIcon(":/image/pencil.png"),"&Pencil",this);
    this->eraserAction= new QAction(QIcon(":/image/eraser.png"),"&Eraser",this);
    this->selectionAction= new QAction(QIcon(":/image/selection.png"),"&Selection",this);
    this->bucketAction= new QAction(QIcon(":/image/bucket.png"),"&Bucket",this);
    this->zoomInAction= new QAction(QIcon(":/image/zoom.png"),"&ZoomIn",this);
    this->zoomOutAction= new QAction(QIcon(":/image/zoom.png"),"&ZoomOut",this);

    this->toolActionVector = std::vector<QAction*>();

    this->toolActionVector.push_back(this->moveAction);
    this->toolActionVector.push_back(this->pencilAction);
    this->toolActionVector.push_back(this->eraserAction);
    this->toolActionVector.push_back(this->selectionAction);
    this->toolActionVector.push_back(this->bucketAction);
    this->toolActionVector.push_back(this->zoomInAction);
    this->toolActionVector.push_back(this->zoomOutAction);

    for(QAction* elem : toolActionVector){
        this->toolbar->addAction(elem);
    }
    this->toolbar->addSeparator();


    this->foreSwatch = new Swatch(this);
    this->foreSwatch->setMinimumSize(30,30);
    this->toolbar->addWidget(foreSwatch);

    this->toolbar->addSeparator();

    this->backSwatch = new Swatch(this);
    this->backSwatch->setMinimumSize(30,30);
    this->backSwatch->setColor(QColor(255,255,255));
    this->toolbar->addWidget(backSwatch);

}


void ImageEditWindow::moveToolSlot(){

}

void ImageEditWindow::pencilToolSlot(){

}

void ImageEditWindow::eraserToolSlot(){

}

void ImageEditWindow::selectionToolSlot(){

}

void ImageEditWindow::bucketToolSlot(){

}

void ImageEditWindow::zoomInToolSlot(){

}

void ImageEditWindow::zoomOutToolSlot(){

}
