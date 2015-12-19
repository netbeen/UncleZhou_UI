#include "imageeditwindow.h"
#include "binaryclassificationdialog.h"

#include <QGridLayout>
#include <QIcon>
#include <QSpinBox>
#include <QDebug>
#include <QPushButton>
#include "Classification/MyClassification.h"
#include "graphcut/graphcut.h"

#include "Classification/MyImageProc.h"


/**
 * @brief ImageEditWindow::ImageEditWindow
 * @brief 图像编辑窗口的构造函数
 * @param editPosition 主窗口的4个窗格中，具体哪一个窗格被编辑
 * @param editLevel 编辑级别
 * @param parent 父对象
 * @return 没有返回值
 */
ImageEditWindow::ImageEditWindow(config::editPosition editPosition, config::editLevel editLevel, QWidget* parent) : QMainWindow(parent),isClassificationColorValid(false),isMultiLabelChecked(false)
{
    this->layerManager = LayerManager::getInstance();   //
    this->layerManager->init(editPosition);                           //优先初始化图层管理器

    this->undoStack = UndoStack::getInstance();
    this->undoStack->init(this->layerManager->getDisplayLayerItem()->image);

    this->initWindowLayout(editLevel);
    this->initActions(editLevel);

    this->testFunctionMenu = this->menuBar()->addMenu("&Test functions");
    this->testFunctionMenu->addAction(this->densityPeakInteractiveAction);
    this->testFunctionMenu->addAction(this->viewPatchDistributeAction);
    this->testFunctionMenu->addAction(this->binaryClassificationAction);
    this->testFunctionMenu->addAction(this->multiLabelClassificationAction);
    this->testFunctionMenu->addAction(this->undoAction);
    this->testFunctionMenu->addAction(this->undoAction);
    this->testFunctionMenu->addAction(this->classificationWithoutPopupAction);


    this->menuBar()->setStyleSheet(" QMenuBar{background-color: #333337; padding-left: 5px;}QMenuBar::item {background-color: #333337; padding:2px; margin:6px 10px 0px 0px;} QMenuBar::item:selected {background: #3e3e40;} QMenuBar::item:pressed {background: #1b1b1c;}");

    //处理SuperPixel的类
    this->readSuperPixelDat = new ReadSuperPixelDat();
    readSuperPixelDat->segmentSourceImage();

    CMyImageProc ttImgProc;
    cv::Mat srcImg = cv::imread("sourceImage.png");
    cv::Mat textonImg = ttImgProc.GenTextonMap(srcImg);
    cv::imwrite("src_texton.png", textonImg);
    cv::Mat sailImg = ttImgProc.GenSaliencyMap(srcImg);
    cv::imwrite("src_saliency.png", sailImg);
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

    //分类预览窗口
    this->multiLabelPreivewDock = new MultiLabelPreivewDock(this);
    this->multiLabelPreivewDock->setWindowTitle("Classification Preview");
    this->multiLabelPreivewDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    this->addDockWidget(Qt::RightDockWidgetArea, this->multiLabelPreivewDock);
    QObject::connect(this->canvas, &Canvas::updateColorButtonLayoutSignal, this->multiLabelPreivewDock, &MultiLabelPreivewDock::receiveUpdateColorLayoutSlot);
    QObject::connect(this->multiLabelPreivewDock, &MultiLabelPreivewDock::sendClassificationColor,this,&ImageEditWindow::setClassificationColor);
    QObject::connect(this->multiLabelPreivewDock, &MultiLabelPreivewDock::multiLabelCheckedSinal, this, &ImageEditWindow::setIsMultiLabelChecked);

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

MultiLabelPreivewDock* ImageEditWindow::getMultiLabelPreivewDock(){
    return this->multiLabelPreivewDock;
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

void ImageEditWindow::doMultiLabelClassificationAndSave(const cv::Mat inputImage){
    cv::Mat img = inputImage;

    cv::Mat_<cv::Vec3b> outputImg;
    CMyClassification myTest;
    myTest.SetParametes(8, 8);

    cv::Mat mask = cv::imread("sourceGuidanceLabelChannel.png");

    std::vector<std::string> morefeatImgs;
    std::string str_textonImg = "src_texton.png"; morefeatImgs.push_back(str_textonImg);
    std::string str_edgemap = "src_pgb.png"; morefeatImgs.push_back(str_edgemap);
    std::string str_saliency = "src_saliency.png"; morefeatImgs.push_back(str_saliency);
    myTest.RandomForest_SuperPixel(img,morefeatImgs,mask,outputImg,"output.dat");
    myTest.RunGraphCut(outputImg);

    cv::imwrite("multiLabelClassificationResult.png",outputImg);
}

// multi label
void ImageEditWindow::multiLabelClassificationSlot(){
    LayerItem* currentDisplayLayerItem = this->layerManager->getDisplayLayerItem();
    this->undoStack->push(currentDisplayLayerItem->image);
    cv::Mat_<cv::Vec3b> currentMask;
    Util::convertQImageToMat(currentDisplayLayerItem->image,currentMask);
    Util::clearFragment(currentMask);   //使用DFS清除细碎色块
    cv::imwrite("sourceGuidanceLabelChannel.png",currentMask);

    this->doMultiLabelClassificationAndSave(currentMask);   //进行MultiLabel的分类

    cv::Mat multiLabelClassificationResult = cv::imread("multiLabelClassificationResult.png");  //读取MultiLabel的结果

    Util::meldTwoCVMat(currentMask,multiLabelClassificationResult);

    Util::convertMattoQImage(currentMask,currentDisplayLayerItem->image);

    this->canvas->update();
}

void ImageEditWindow::setIsMultiLabelChecked(bool flag){
    this->isMultiLabelChecked = flag;
}



void ImageEditWindow::classificationWithoutPopupSlot(){
    LayerItem* currentDisplayLayerItem = this->layerManager->getDisplayLayerItem();
    this->undoStack->push(currentDisplayLayerItem->image);
    cv::Mat_<cv::Vec3b> currentMask;
    Util::convertQImageToMat(currentDisplayLayerItem->image,currentMask);
    //Util::clearFragment(cvImage);
    cv::imwrite("sourceGuidanceLabelChannel.png",currentMask);

    std::cout << "True, flag = " << true << ", " << this->isMultiLabelChecked << std::endl;
    if(this->isMultiLabelChecked == true){
        //do multi label classification.
        this->doMultiLabelClassificationAndSave(currentMask);   //进行MultiLabel的分类
        cv::Mat multiLabelClassificationResult = cv::imread("multiLabelClassificationResult.png");  //读取MultiLabel的结果
        Util::meldTwoCVMat(currentMask,multiLabelClassificationResult);
        Util::convertMattoQImage(currentMask,currentDisplayLayerItem->image);
        this->canvas->update();
    }else{
        //do binary classification.
        if(this->isClassificationColorValid == true){
            cv::Vec3b Cur_Color = this->classificationColor;
            cv::Mat multiLabelMask = cv::imread("sourceGuidanceLabelChannel.png");
            cv::Mat twoLabelMask;
            Util::convertMultiLabelMaskToTwoLabelMask(multiLabelMask,twoLabelMask,Cur_Color);
            cv::imwrite("twoLabelMask.png",twoLabelMask);
            //利用mask图像，生成analyse文件
            this->readSuperPixelDat->analyseLabelFile("twoLabelMask.png");
            // Read Source Image
            cv::Mat img = cv::imread("sourceImage.png");
            // Run RFBinaryClassification2
            cv::Mat_<cv::Vec3b> outputImg;
            CMyClassification myTest;
            //myTest.SetParametes(8, 8);
            std::vector<std::string> morefeatImgs;
            std::string str_textonImg = "src_texton.png"; morefeatImgs.push_back(str_textonImg);
            std::string str_edgemap = "src_pgb.png"; morefeatImgs.push_back(str_edgemap);
            std::string str_saliency = "src_saliency.png"; morefeatImgs.push_back(str_saliency);
            myTest.RandomForest_SuperPixel(img,morefeatImgs,twoLabelMask,outputImg,"output.dat");
            myTest.RunGraphCut(outputImg);
            cv::Mat oneLabelMask;
            Util::convertTwoLabelMaskToOneLabelMask(outputImg,oneLabelMask,Cur_Color);
            Util::meldTwoCVMat(currentMask,oneLabelMask);
            Util::convertMattoQImage(currentMask,currentDisplayLayerItem->image);
            this->canvas->update();
            //更新MultiLabel
            //this->readSuperPixelDat->analyseLabelFile("sourceGuidanceLabelChannel.png");
            this->doMultiLabelClassificationAndSave(currentMask);
            this->multiLabelPreivewDock->multiLabelCanvas->update();
        }
    }
}

void ImageEditWindow::binaryClassificationSlot(){
    LayerItem* currentDisplayLayerItem = this->layerManager->getDisplayLayerItem();
    this->undoStack->push(currentDisplayLayerItem->image);
    cv::Mat_<cv::Vec3b> cvImage;
    Util::convertQImageToMat(currentDisplayLayerItem->image,cvImage);
    //Util::clearFragment(cvImage);
    cv::imwrite("sourceGuidanceLabelChannel.png",cvImage);

    //开启选颜色对话框
    BinaryClassificationDialog* binaryClassificationDialog = new BinaryClassificationDialog(this);
    QObject::connect(binaryClassificationDialog,&BinaryClassificationDialog::sendColor, this, &ImageEditWindow::setClassificationColor);

    if(binaryClassificationDialog->exec() == QDialog::Accepted){
        cv::Vec3b Cur_Color = this->classificationColor;

        cv::Mat multiLabelMask = cv::imread("sourceGuidanceLabelChannel.png");
        cv::Mat twoLabelMask;
        Util::convertMultiLabelMaskToTwoLabelMask(multiLabelMask,twoLabelMask,Cur_Color);
        cv::imwrite("twoLabelMask.png",twoLabelMask);

        //利用mask图像，生成analyse文件
        this->readSuperPixelDat->analyseLabelFile("twoLabelMask.png");

        // Read Source Image
        cv::Mat img = cv::imread("sourceImage.png");

        // Run RFBinaryClassification2
        cv::Mat_<cv::Vec3b> outputImg;
        CMyClassification myTest;
        //myTest.SetParametes(8, 8);
        std::vector<std::string> morefeatImgs;
        std::string str_textonImg = "src_texton.png"; morefeatImgs.push_back(str_textonImg);
        std::string str_edgemap = "src_pgb.png"; morefeatImgs.push_back(str_edgemap);
        std::string str_saliency = "src_saliency.png"; morefeatImgs.push_back(str_saliency);
        myTest.RandomForest_SuperPixel(img,morefeatImgs,twoLabelMask,outputImg,"output.dat");
        myTest.RunGraphCut(outputImg);

        cv::Mat oneLabelMask;
        Util::convertTwoLabelMaskToOneLabelMask(outputImg,oneLabelMask,Cur_Color);

        Util::meldTwoCVMat(cvImage,oneLabelMask);

        Util::convertMattoQImage(cvImage,currentDisplayLayerItem->image);

        this->canvas->update();

        //更新MultiLabel
        //this->readSuperPixelDat->analyseLabelFile("sourceGuidanceLabelChannel.png");
        this->doMultiLabelClassificationAndSave(cvImage);
        this->multiLabelPreivewDock->multiLabelCanvas->update();
    }
}

void ImageEditWindow::undoSlot(){
    this->layerManager->getDisplayLayerItem()->image =this->undoStack->pop();
    this->canvas->update();
}

void ImageEditWindow::setClassificationColor(cv::Vec3b newColor){
    this->classificationColor = newColor;
    this->isClassificationColorValid = true;
    //std::cout<< "分类色被设置为" << newColor << std::endl;
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

    this->brokenLineAction= new QAction(QIcon(":/image/brokenLine.png"),"&Broken Line",this);
    QObject::connect(this->brokenLineAction, &QAction::triggered, this, &ImageEditWindow::brokenLineToolSlot);

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
    this->multiLabelClassificationAction = new QAction(QIcon(":image/open.png"),"&Multi Label Classification",this);
    QObject::connect(this->multiLabelClassificationAction,&QAction::triggered, this, &ImageEditWindow::multiLabelClassificationSlot);
    this->undoAction = new QAction(QIcon(":image/open.png"),"&Undo",this);
    this->undoAction->setShortcut(QKeySequence::Undo);
    QObject::connect(this->undoAction,&QAction::triggered, this, &ImageEditWindow::undoSlot);

    //免弹窗的分类action
    this->classificationWithoutPopupAction = new QAction(QIcon(":image/open.png"),"&Classification Without Pop Up",this);
    QObject::connect(this->classificationWithoutPopupAction, &QAction::triggered, this, &ImageEditWindow::classificationWithoutPopupSlot);
    this->classificationWithoutPopupAction->setShortcut(Qt::Key_C);



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
        this->toolActionVector.push_back(this->brokenLineAction);
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

    this->brokenLineToolOptionFrame = new ToolOptionFrame("Broken Line",this);  //折线工具选项卡配置
    QLabel* brokenLineLabel = new QLabel("Broken Line radius: ",this->brokenLineToolOptionFrame);
    this->brokenLineToolOptionFrame->mainLayout->addWidget(brokenLineLabel,1,0,1,1,Qt::AlignCenter);
    QSpinBox* brokenLineRadiusSpinBox = new QSpinBox(this->brokenLineToolOptionFrame);
    brokenLineRadiusSpinBox->setMinimumSize(QSize(75,30));
    brokenLineRadiusSpinBox->setValue(10);
    QObject::connect(brokenLineRadiusSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this->canvas, &Canvas::setBrokenLineRadius);
    this->brokenLineToolOptionFrame->mainLayout->addWidget(brokenLineRadiusSpinBox,1,1,1,1,Qt::AlignCenter);

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

void ImageEditWindow::brokenLineToolSlot(){
    this->setCursor(Qt::CrossCursor);
    this->canvas->setOperationType(config::BrokenLine);
    emit this->sendFrameToToolOptionDock(this->brokenLineToolOptionFrame);
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


