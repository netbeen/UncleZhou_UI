#include "imageeditwindow.h"

#include <QGridLayout>
#include <QIcon>
#include <QSpinBox>
#include <QDebug>
#include <QPushButton>



ImageEditWindow::ImageEditWindow(config::editPosition editPosition, config::editLevel editLevel, QWidget* parent) : QMainWindow(parent)
{
    this->layerManager = LayerManager::getInstance();   //
    this->layerManager->init(editPosition);                           //优先初始化图层管理器

    this->initWindowLayout(editLevel);
    this->initActions(editLevel);


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

    this->paletteDock = new PaletteDock(this);
    this->paletteDock->setWindowTitle("Palette");
    this->paletteDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    this->addDockWidget(Qt::RightDockWidgetArea, this->paletteDock);
    QObject::connect(this->paletteDock, &PaletteDock::colorSelected, this->canvas, &Canvas::setColor);



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

    this->noneAction = new QAction(QIcon(":/image/none.png"),"&None",this);
    QObject::connect(this->noneAction, &QAction::triggered, this, &ImageEditWindow::noneToolSlot);
    this->moveAction = new QAction(QIcon(":/image/move.png"),"&Move",this);
    QObject::connect(this->moveAction, &QAction::triggered, this, &ImageEditWindow::moveToolSlot);
    this->pencilAction= new QAction(QIcon(":/image/pencil.png"),"&Pencil",this);
    QObject::connect(this->pencilAction, &QAction::triggered, this, &ImageEditWindow::pencilToolSlot);
    this->eraserAction= new QAction(QIcon(":/image/eraser.png"),"&Eraser",this);
    QObject::connect(this->eraserAction, &QAction::triggered, this, &ImageEditWindow::eraserToolSlot);
    this->selectionAction= new QAction(QIcon(":/image/selection.png"),"&Selection",this);
    QObject::connect(this->selectionAction, &QAction::triggered, this, &ImageEditWindow::selectionToolSlot);
    this->bucketAction= new QAction(QIcon(":/image/bucket.png"),"&Bucket",this);
    QObject::connect(this->bucketAction, &QAction::triggered, this, &ImageEditWindow::bucketToolSlot);
    this->zoomInAction= new QAction(QIcon(":/image/zoomIn.png"),"&ZoomIn",this);
    QObject::connect(this->zoomInAction, &QAction::triggered, this, &ImageEditWindow::zoomInToolSlot);
    this->zoomOutAction= new QAction(QIcon(":/image/zoomOut.png"),"&ZoomOut",this);
    QObject::connect(this->zoomOutAction, &QAction::triggered, this, &ImageEditWindow::zoomOutToolSlot);

    this->toolActionVector = std::vector<QAction*>();

    this->toolActionVector.push_back(this->noneAction);
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
        QObject::connect(this->paletteDock, &PaletteDock::colorSelected, this->foreSwatch, &Swatch::setColor);

        this->toolbar->addSeparator();

        this->backSwatch = new Swatch(this);
        this->backSwatch->setMinimumSize(30,30);
        this->backSwatch->setColor(QColor(255,255,255));
        this->toolbar->addWidget(backSwatch);
    }

    this->noneToolOptionFrame = new ToolOptionFrame("None",this);

    this->moveToolOptionFrame = new ToolOptionFrame("Move",this);

    this->pencilToolOptionFrame = new ToolOptionFrame("Pencil",this);   //  铅笔工具选项卡配置
    QLabel* pencilRadiusLabel = new QLabel("Pencil radius: ",this->pencilToolOptionFrame);
    this->pencilToolOptionFrame->mainLayout->addWidget(pencilRadiusLabel,1,0,1,1,Qt::AlignCenter);
    QSpinBox* pencilRadiusSpinBox = new QSpinBox(this->pencilToolOptionFrame);
    pencilRadiusSpinBox->setMinimumSize(QSize(75,30));
    pencilRadiusSpinBox->setValue(10);
    QObject::connect(pencilRadiusSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this->canvas, &Canvas::setPencilRadius);
    this->pencilToolOptionFrame->mainLayout->addWidget(pencilRadiusSpinBox,1,1,1,1,Qt::AlignCenter);

    this->eraserToolOptionFrame = new ToolOptionFrame("Eraser",this);   //橡皮工具选项卡配置
    QLabel* eraserRadiusLabel = new QLabel("Eraser radius: ",this->eraserToolOptionFrame);
    this->eraserToolOptionFrame->mainLayout->addWidget(eraserRadiusLabel,1,0,1,1,Qt::AlignCenter);
    QSpinBox* eraserRadiusSpinBox = new QSpinBox(this->eraserToolOptionFrame);
    eraserRadiusSpinBox->setMinimumSize(QSize(75,30));
    eraserRadiusSpinBox->setValue(10);
    QObject::connect(eraserRadiusSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this->canvas, &Canvas::setEraserRadius);
    this->eraserToolOptionFrame->mainLayout->addWidget(eraserRadiusSpinBox,1,1,1,1,Qt::AlignCenter);

    this->bucketToolOptionFrame = new ToolOptionFrame("Bucket",this);

    this->selectionToolOptionFrame = new ToolOptionFrame("Selection",this);

    this->zoomToolOptionFrame = new ToolOptionFrame("Zoom",this);       //缩放工具的选项卡配置（放大缩小共享同一张选项卡）
    QLabel* magnificationLabel = new QLabel("Current magnification: ",this->eraserToolOptionFrame);
    this->zoomToolOptionFrame->mainLayout->addWidget(magnificationLabel,1,0,1,1,Qt::AlignCenter);
    this->magnificationValueLabel = new QLabel("100%",this->eraserToolOptionFrame);
    this->zoomToolOptionFrame->mainLayout->addWidget(this->magnificationValueLabel,1,1,1,1,Qt::AlignCenter);
    QObject::connect(this->canvas, &Canvas::scaleFactorChanged, this, &ImageEditWindow::receiveScaleChanged);
    QPushButton* resetTheScaleFactorButton = new QPushButton("Reset Scale Factor",this->zoomToolOptionFrame);
    this->zoomToolOptionFrame->mainLayout->addWidget(resetTheScaleFactorButton,2,0,1,1,Qt::AlignCenter);
    QObject::connect(resetTheScaleFactorButton, &QPushButton::clicked, this->canvas, &Canvas::resetScale);
}


void ImageEditWindow::moveToolSlot(){
    QCursor moveCursor = QCursor(QPixmap(":image/move.png").scaled(QSize(40,40)),0,0);
    this->setCursor(moveCursor);
    this->canvas->setOperationType(config::Move);
    emit this->sendFrameToToolOptionDock(this->moveToolOptionFrame);
}

void ImageEditWindow::pencilToolSlot(){
    this->setCursor(Qt::CrossCursor);
    this->canvas->setOperationType(config::Pencil);
    emit this->sendFrameToToolOptionDock(this->pencilToolOptionFrame);

}

void ImageEditWindow::eraserToolSlot(){
    this->setCursor(Qt::CrossCursor);
    this->canvas->setOperationType(config::Eraser);
    emit this->sendFrameToToolOptionDock(this->eraserToolOptionFrame);
}

void ImageEditWindow::selectionToolSlot(){
    this->setCursor(Qt::CrossCursor);
    emit this->sendFrameToToolOptionDock(this->selectionToolOptionFrame);
}

void ImageEditWindow::bucketToolSlot(){
    QCursor bucketCursor = QCursor(QPixmap(":image/bucket.png").scaled(QSize(40,40)));
    this->setCursor(bucketCursor);
    this->canvas->setOperationType(config::Bucket);
    emit this->sendFrameToToolOptionDock(this->bucketToolOptionFrame);
}

void ImageEditWindow::zoomInToolSlot(){
    QCursor zoomInCursor = QCursor(QPixmap(":image/zoomIn.png").scaled(QSize(40,40)));
    this->setCursor(zoomInCursor);
    this->canvas->setOperationType(config::ZoomIn);
    emit this->sendFrameToToolOptionDock(this->zoomToolOptionFrame);

}

void ImageEditWindow::zoomOutToolSlot(){
    QCursor zoomOutCursor = QCursor(QPixmap(":image/zoomOut.png").scaled(QSize(40,40)));
    this->setCursor(zoomOutCursor);
    this->canvas->setOperationType(config::ZoomOut);
    emit this->sendFrameToToolOptionDock(this->zoomToolOptionFrame);
}

void ImageEditWindow::noneToolSlot(){
    this->setCursor(Qt::ArrowCursor);
    this->canvas->setOperationType(config::None);
    emit this->sendFrameToToolOptionDock(this->noneToolOptionFrame);
}


void ImageEditWindow::receiveScaleChanged(float inputScale){
    this->magnificationValueLabel->setText(QString("%1%").arg(inputScale*100));
}

