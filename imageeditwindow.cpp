#include "imageeditwindow.h"
#include "binaryclassificationdialog.h"

#include <QGridLayout>
#include <QIcon>
#include <QSpinBox>
#include <QDebug>
#include <QPushButton>
#include "BinaryClassification/MyBinaryClassification.h"
#include "graphcut/graphcut.h"


/**
 * @brief ImageEditWindow::ImageEditWindow
 * @brief 图像编辑窗口的构造函数
 * @param editPosition 主窗口的4个窗格中，具体哪一个窗格被编辑
 * @param editLevel 编辑级别
 * @param parent 父对象
 * @return 没有返回值
 */
ImageEditWindow::ImageEditWindow(config::editPosition editPosition, config::editLevel editLevel, QWidget* parent) : QMainWindow(parent)
{
    this->layerManager = LayerManager::getInstance();   //
    this->layerManager->init(editPosition);                           //优先初始化图层管理器

    this->initWindowLayout(editLevel);
    this->initActions(editLevel);

    this->testFunctionMenu = this->menuBar()->addMenu("&Test functions");
    this->testFunctionMenu->addAction(this->densityPeakInteractiveAction);
    this->testFunctionMenu->addAction(this->viewPatchDistributeAction);
    this->testFunctionMenu->addAction(this->binaryClassificationAction);

    this->menuBar()->setStyleSheet(" QMenuBar{background-color: #333337; padding-left: 5px;}QMenuBar::item {background-color: #333337; padding:2px; margin:6px 10px 0px 0px;} QMenuBar::item:selected {background: #3e3e40;} QMenuBar::item:pressed {background: #1b1b1c;}");

    //处理SuperPixel的类
    this->readSuperPixelDat = new ReadSuperPixelDat();
    readSuperPixelDat->segmentSourceImage();
}


/**
 * @brief ImageEditWindow::initWindowLayout
 * @brief 初始化窗体布局，更具编辑级别创建不同的UI
 * @param editLevel 编辑级别
 * @return 没有返回值
 */
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


void ImageEditWindow::keyPressEvent(QKeyEvent *e){
    /*if (e->key() == Qt::Key_B){
        std::cout << "Qt::Key_B" << std::endl;
    }*/
}


void ImageEditWindow::densityPeakInteractiveSlot(){
    DensityPeakDialog* densityPeakDialog = new DensityPeakDialog(this);
    if(densityPeakDialog->exec() == QDialog::Accepted){
        ;
    }
}

void ImageEditWindow::viewPatchDistributeSlot(){
    ViewPatchDistributeDialog* viewPatchDistributeDialog = new ViewPatchDistributeDialog(this);
    if(viewPatchDistributeDialog->exec() == QDialog::Accepted){
        ;
    }
}

void ImageEditWindow::binaryClassificationSlot(){
    LayerItem* currentDisplayLayerItem = this->layerManager->getDisplayLayerItem();
    cv::Mat_<cv::Vec3b> cvImage;
    Util::convertQImageToMat(currentDisplayLayerItem->image,cvImage);
    Util::clearFragment(cvImage);
    cv::imwrite("sourceGuidanceLabelChannel.png",cvImage);

    //开启选颜色对话框
    BinaryClassificationDialog* binaryClassificationDialog = new BinaryClassificationDialog(this);
    QObject::connect(binaryClassificationDialog,&BinaryClassificationDialog::sendColor, this, &ImageEditWindow::getClassificationColor);
    binaryClassificationDialog->exec();

    cv::Vec3b BK_Color(uchar(255), uchar(255), uchar(255)); //whilte
    cv::Vec3b Cur_Color = this->classificationColor;

    //利用mask图像，生成analyse文件
    std::cout << "this->readSuperPixelDat->analyseLabelFile(); start." << std::endl;
    this->readSuperPixelDat->analyseLabelFile();
    std::cout << "this->readSuperPixelDat->analyseLabelFile(); end." << std::endl;

    // Read Source Image
    cv::Mat img = cv::imread("sourceImage.png");

    // Read Mask
    cv::Mat mask = cv::imread("sourceGuidanceLabelChannel.png");

    // Run RFBinaryClassification
    cv::Mat_<cv::Vec3b> outputImg;
    CMyBinaryClassification myTest;
    myTest.SetParametes(8, 8);
    //myTest.RandomForestBinaryClassification(img, mask, outputImg, BK_Color, Cur_Color);

    std::cout << "myTest.RandomForestBinaryClassification start." << std::endl;
    myTest.RandomForestBinaryClassification(img,outputImg,BK_Color,Cur_Color,"analyseResult.txt");
    std::cout << "myTest.RandomForestBinaryClassification end." << std::endl;

    //Util::dilateAndErode(outputImg);
    cv::imwrite("afterDilateAndErode.png",outputImg);

    cv::imshow("before",outputImg);
    GraphCut* graphcut = new GraphCut();
    graphcut->main(img,outputImg);
    cv::imshow("after",outputImg);


    Util::meldTwoCVMat(cvImage,outputImg);

    Util::convertMattoQImage(cvImage,currentDisplayLayerItem->image);

    this->canvas->update();
}

void ImageEditWindow::getClassificationColor(cv::Vec3b newColor){
    this->classificationColor = newColor;
    std::cout<< "分类色被设置为" << newColor << std::endl;
}

/**
 * @brief ImageEditWindow::initActions
 * @brief 初始化actions，根据不同的编辑级别，构建不同的菜单action
 * @param editLevel
 * @return 没有返回值
 */
void ImageEditWindow::initActions(config::editLevel editLevel){
    this->noneAction = new QAction(QIcon(":/image/none.png"),"&None",this);
    QObject::connect(this->noneAction, &QAction::triggered, this, &ImageEditWindow::noneToolSlot);
    this->moveAction = new QAction(QIcon(":/image/move.png"),"&Move",this);
    QObject::connect(this->moveAction, &QAction::triggered, this, &ImageEditWindow::moveToolSlot);
    this->pencilAction= new QAction(QIcon(":/image/pencil.png"),"&Pencil",this);
    QObject::connect(this->pencilAction, &QAction::triggered, this, &ImageEditWindow::pencilToolSlot);
    this->eraserAction= new QAction(QIcon(":/image/eraser.png"),"&Eraser",this);
    this->eraserAction->setShortcut(Qt::Key_E);
    QObject::connect(this->eraserAction, &QAction::triggered, this, &ImageEditWindow::eraserToolSlot);
    this->magicEraserAction= new QAction(QIcon(":/image/magicEraser.png"),"&Magic Eraser",this);
    QObject::connect(this->magicEraserAction, &QAction::triggered, this, &ImageEditWindow::magicEraserToolSlot);
    this->polygonAction= new QAction(QIcon(":/image/polygon.png"),"&Polygon",this);
    this->polygonAction->setShortcut(Qt::Key_R);
    QObject::connect(this->polygonAction, &QAction::triggered, this, &ImageEditWindow::polygonToolSlot);
    this->bucketAction= new QAction(QIcon(":/image/bucket.png"),"&Bucket",this);
    QObject::connect(this->bucketAction, &QAction::triggered, this, &ImageEditWindow::bucketToolSlot);
    this->zoomInAction= new QAction(QIcon(":/image/zoomIn.png"),"&ZoomIn",this);
    QObject::connect(this->zoomInAction, &QAction::triggered, this, &ImageEditWindow::zoomInToolSlot);
    this->zoomOutAction= new QAction(QIcon(":/image/zoomOut.png"),"&ZoomOut",this);
    QObject::connect(this->zoomOutAction, &QAction::triggered, this, &ImageEditWindow::zoomOutToolSlot);

    this->densityPeakInteractiveAction = new QAction(QIcon(":image/open.png"),"&Density Peak",this);
    QObject::connect(this->densityPeakInteractiveAction,&QAction::triggered, this, &ImageEditWindow::densityPeakInteractiveSlot);
    this->viewPatchDistributeAction = new QAction(QIcon(":image/open.png"),"&View Patch Distribute",this);
    QObject::connect(this->viewPatchDistributeAction,&QAction::triggered, this, &ImageEditWindow::viewPatchDistributeSlot);
    this->binaryClassificationAction = new QAction(QIcon(":image/open.png"),"&Binary Classification",this);
    this->binaryClassificationAction->setShortcut(Qt::Key_B);
    QObject::connect(this->binaryClassificationAction,&QAction::triggered, this, &ImageEditWindow::binaryClassificationSlot);



    this->toolActionVector = std::vector<QAction*>();

    this->toolActionVector.push_back(this->noneAction);
    this->toolActionVector.push_back(this->moveAction);
    this->toolActionVector.push_back(this->zoomInAction);
    this->toolActionVector.push_back(this->zoomOutAction);

    if(editLevel == config::editable){
        this->toolActionVector.push_back(this->pencilAction);
        this->toolActionVector.push_back(this->eraserAction);
        this->toolActionVector.push_back(this->magicEraserAction);
        this->toolActionVector.push_back(this->polygonAction);
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

    this->magicEraserToolOptionFrame = new ToolOptionFrame("Magic Eraser",this);   //魔术橡皮工具选项卡配置

    this->bucketToolOptionFrame = new ToolOptionFrame("Bucket",this);

    this->polygonToolOptionFrame = new ToolOptionFrame("Polygon",this);

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

/**
 * @brief ImageEditWindow::moveToolSlot
 * @brief 移动工具的slot函数
 * @return 没有返回值
 */
void ImageEditWindow::moveToolSlot(){
    QCursor moveCursor = QCursor(QPixmap(":image/move.png").scaled(QSize(40,40)),0,0);
    this->setCursor(moveCursor);
    this->canvas->setOperationType(config::Move);
    emit this->sendFrameToToolOptionDock(this->moveToolOptionFrame);
}

/**
 * @brief ImageEditWindow::pencilToolSlot
 * @brief 铅笔工具的slot函数
 * @return 没有返回值
 */
void ImageEditWindow::pencilToolSlot(){
    this->setCursor(Qt::CrossCursor);
    this->canvas->setOperationType(config::Pencil);
    emit this->sendFrameToToolOptionDock(this->pencilToolOptionFrame);
}

/**
 * @brief ImageEditWindow::eraserToolSlot
 * @brief 橡皮工具的slot工具
 * @return 没有返回值
 */
void ImageEditWindow::eraserToolSlot(){
    this->setCursor(Qt::CrossCursor);
    this->canvas->setOperationType(config::Eraser);
    emit this->sendFrameToToolOptionDock(this->eraserToolOptionFrame);
}

/**
 * @brief ImageEditWindow::magicEraserToolSlot
 * @brief 魔术橡皮擦的slot工具
 * @return 没有返回值
 */
void ImageEditWindow::magicEraserToolSlot(){
    this->setCursor(Qt::CrossCursor);
    this->canvas->setOperationType(config::MagicEraser);
    emit this->sendFrameToToolOptionDock(this->magicEraserToolOptionFrame);
}


/**
 * @brief ImageEditWindow::polygonToolSlot
 * @brief 多边形的slot工具
 * @return 没有返回值
 */
void ImageEditWindow::polygonToolSlot(){
    this->setCursor(Qt::CrossCursor);
    this->canvas->setOperationType(config::Polygon);
    emit this->sendFrameToToolOptionDock(this->polygonToolOptionFrame);
}


/**
 * @brief ImageEditWindow::bucketToolSlot
 * @brief 颜料桶的slot函数
 * @return 没有返回值
 */
void ImageEditWindow::bucketToolSlot(){
    QCursor bucketCursor = QCursor(QPixmap(":image/bucket.png").scaled(QSize(40,40)));
    this->setCursor(bucketCursor);
    this->canvas->setOperationType(config::Bucket);
    emit this->sendFrameToToolOptionDock(this->bucketToolOptionFrame);
}

/**
 * @brief ImageEditWindow::zoomInToolSlot
 * @brief 放大工具的slot函数
 * @return 没有返回值
 */
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


