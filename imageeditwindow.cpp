#include "imageeditwindow.h"

#include <QGridLayout>
#include <QIcon>



ImageEditWindow::ImageEditWindow(config::editPosition editPosition, config::editLevel editLevel, QWidget* parent) : QMainWindow(parent)
{
    this->layerManager = LayerManager::getInstance();   //
    this->layerManager->init(editPosition);                           //优先初始化图层管理器

    this->initWindowLayout(editLevel);
    this->initActions(editLevel);




    //对画板填充内容
    //this->canvas->init(editPosition);
}


void ImageEditWindow::initWindowLayout(config::editLevel editLevel){
    this->setWindowState(Qt::WindowMaximized);      //设置窗口最大化
    this->setStyleSheet("background-color: #333337; color: #f1f1f1;");


    this->toolbar = new QToolBar("Toolbar",this);
    this->addToolBar(Qt::LeftToolBarArea,this->toolbar);

    this->canvas = new Canvas(this);
    this->canvas->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    this->navigatorDock = new NavigatorDock(this);
    this->navigatorDock->setWindowTitle("Navigator");
    this->navigatorDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    this->addDockWidget(Qt::RightDockWidgetArea, this->navigatorDock);
    QObject::connect(this->canvas, &Canvas::canvasUpdatedSignal, this->navigatorDock, &NavigatorDock::navigatorUpdate);

    this->toolOptionDock = new ToolOptionDock(this);
    this->toolOptionDock->setWindowTitle("Tool Option");
    this->toolOptionDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    this->addDockWidget(Qt::RightDockWidgetArea, this->toolOptionDock);
    QObject::connect(this, &ImageEditWindow::sendFrameToToolOptionDock, this->toolOptionDock, &ToolOptionDock::setFrame);

    this->layerDock = new LayerDock(this);
    this->layerDock->setWindowTitle("Layer");
    this->layerDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    this->addDockWidget(Qt::RightDockWidgetArea, this->layerDock);
    QObject::connect(this->layerDock->isShowBackgroundCheckBox, &QCheckBox::toggled, this->canvas, &Canvas::receiveShowBackground);

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


void ImageEditWindow::initActions(config::editLevel editLevel){

    this->moveAction = new QAction(QIcon(":/image/move.png"),"&Move",this);
    QObject::connect(this->moveAction, &QAction::triggered, this, &ImageEditWindow::moveToolSlot);
    this->pencilAction= new QAction(QIcon(":/image/pencil.png"),"&Pencil",this);
    QObject::connect(this->pencilAction, &QAction::triggered, this, &ImageEditWindow::pencilToolSlot);
    this->eraserAction= new QAction(QIcon(":/image/eraser.png"),"&Eraser",this);
    QObject::connect(this->eraserAction, &QAction::triggered, this, &ImageEditWindow::eraserToolSlot);
    this->selectionAction= new QAction(QIcon(":/image/selection.png"),"&Selection",this);
    this->bucketAction= new QAction(QIcon(":/image/bucket.png"),"&Bucket",this);
    QObject::connect(this->bucketAction, &QAction::triggered, this, &ImageEditWindow::bucketToolSlot);
    this->zoomInAction= new QAction(QIcon(":/image/zoomIn.png"),"&ZoomIn",this);
    QObject::connect(this->zoomInAction, &QAction::triggered, this, &ImageEditWindow::zoomInToolSlot);
    this->zoomOutAction= new QAction(QIcon(":/image/zoomOut.png"),"&ZoomOut",this);
    QObject::connect(this->zoomOutAction, &QAction::triggered, this, &ImageEditWindow::zoomOutToolSlot);

    this->toolActionVector = std::vector<QAction*>();

    this->toolActionVector.push_back(this->moveAction);
    this->toolActionVector.push_back(this->zoomInAction);
    this->toolActionVector.push_back(this->zoomOutAction);

    if(editLevel == config::editable){
        this->toolActionVector.push_back(this->pencilAction);
        this->toolActionVector.push_back(this->eraserAction);
        this->toolActionVector.push_back(this->selectionAction);
        this->toolActionVector.push_back(this->bucketAction);
    }

    for(QAction* elem : toolActionVector){
        this->toolbar->addAction(elem);
    }

    if(editLevel == config::editable){
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


    this->moveToolOptionFrame = new ToolOptionFrame(this);
    this->moveToolOptionFrame->titleLabel->setText("Move");

    this->pencilToolOptionFrame = new ToolOptionFrame(this);
    this->pencilToolOptionFrame->titleLabel->setText("Pencil");

    this->eraserToolOptionFrame = new ToolOptionFrame(this);
    this->eraserToolOptionFrame->titleLabel->setText("Eraser");

    this->bucketToolOptionFrame = new ToolOptionFrame(this);
    this->bucketToolOptionFrame->titleLabel->setText("Bucket");

    this->selectionToolOptionFrame = new ToolOptionFrame(this);
    this->selectionToolOptionFrame->titleLabel->setText("Select");

    this->zoomInToolOptionFrame = new ToolOptionFrame(this);
    this->zoomInToolOptionFrame->titleLabel->setText("Zoom In");

    this->zoomOutToolOptionFrame = new ToolOptionFrame(this);
    this->zoomOutToolOptionFrame->titleLabel->setText("Zoom Out");
}


void ImageEditWindow::moveToolSlot(){
    this->canvas->setOperationType(config::Move);
    emit this->sendFrameToToolOptionDock(this->moveToolOptionFrame);
}

void ImageEditWindow::pencilToolSlot(){
    this->canvas->setOperationType(config::Pencil);
    emit this->sendFrameToToolOptionDock(this->pencilToolOptionFrame);

}

void ImageEditWindow::eraserToolSlot(){
    this->canvas->setOperationType(config::Eraser);
    emit this->sendFrameToToolOptionDock(this->eraserToolOptionFrame);
}

void ImageEditWindow::selectionToolSlot(){

}

void ImageEditWindow::bucketToolSlot(){
    this->canvas->setOperationType(config::Bucket);
    emit this->sendFrameToToolOptionDock(this->bucketToolOptionFrame);
}

void ImageEditWindow::zoomInToolSlot(){
    this->canvas->setOperationType(config::ZoomIn);
    emit this->sendFrameToToolOptionDock(this->zoomInToolOptionFrame);

}

void ImageEditWindow::zoomOutToolSlot(){
    this->canvas->setOperationType(config::ZoomOut);
    emit this->sendFrameToToolOptionDock(this->zoomOutToolOptionFrame);
}
