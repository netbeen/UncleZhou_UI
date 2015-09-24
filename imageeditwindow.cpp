#include "imageeditwindow.h"

#include <QGridLayout>
#include <QIcon>

ImageEditWindow::ImageEditWindow(config::editPosition editPosition, config::editLevel editLevel, QWidget* parent) : QMainWindow(parent)
{
    this->initWindowLayout();
    this->initActions();




}


void ImageEditWindow::initWindowLayout(){
    this->setStyleSheet("background-color: #333337; color: #f1f1f1;");
    this->setMinimumWidth(1024);
    this->setMinimumHeight(768);

    this->toolbar = new QToolBar("Toolbar",this);
    this->addToolBar(Qt::LeftToolBarArea,this->toolbar);

    this->layerDock = new LayerDock(this);
    this->layerDock->setWindowTitle("Layer");
    this->layerDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    this->addDockWidget(Qt::RightDockWidgetArea, this->layerDock);





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
