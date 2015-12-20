#include "mainwindow.h"
#include <QApplication>
#include <QGridLayout>
#include <QWidget>
#include <QLabel>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QAction>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include "newimagedialog.h"
#include "util.h"
#include "imageeditwindow.h"
#include "QFileInfo"


/**
 * @brief MainWindow::MainWindow
 * @brief mainwindow的构造函数
 * @param parent 是qt widget的父指针
 * @return 没有返回值
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    this->initWindowLayout();
    this->initAction();

    QIcon icon(":/image/open.png"); //设置程序图标
    this->setWindowIcon(icon);

    this->fileMenu = this->menuBar()->addMenu("&File");
    this->fileMenu->addAction(this->loadSourceImageAction);

    this->helpMenu = this->menuBar()->addMenu("&Help");
    this->helpMenu->addAction(this->aboutAction);

    this->menuBar()->setStyleSheet(" QMenuBar{background-color: #333337; padding-left: 5px;}QMenuBar::item {background-color: #333337; padding:2px; margin:6px 10px 0px 0px;} QMenuBar::item:selected {background: #3e3e40;} QMenuBar::item:pressed {background: #1b1b1c;}");

    QToolBar* toolbar = this->addToolBar("Standard Tool Bar");
    toolbar->addAction(this->loadSourceImageAction);

    //QStatusBar* statusBar = this->statusBar();

}


/**
 * @brief MainWindow::initAction
 * @brief 用于mainwindow的action的定义、connect、插入
 * @param 没有参数
 * @return 没有返回值
 */
void MainWindow::initAction(){
    this->loadSourceImageAction = new QAction(QIcon(":/image/open.png"),"&Load Source Image",this);
    QObject::connect(this->loadSourceImageAction, &QAction::triggered, this, &MainWindow::loadSourceImage);

    this->viewSourceImageAction = new QAction(QIcon(":/image/open.png"),"&View Source Image",this);
    QObject::connect(this->viewSourceImageAction, &QAction::triggered, this, &MainWindow::viewSourceImage);
    this->viewSourceImageAction->setEnabled(false);

    this->editSourceGuidanceAction = new QAction(QIcon(":/image/open.png"),"&Edit Source Guidance",this);
    QObject::connect(this->editSourceGuidanceAction, &QAction::triggered, this, &MainWindow::editSourceGuidance);
    this->editSourceGuidanceAction->setEnabled(false);

    this->viewTargetGuidanceAction = new QAction(QIcon(":/image/open.png"),"&View Target Guidance",this);
    QObject::connect(this->viewTargetGuidanceAction, &QAction::triggered, this, &MainWindow::viewTargetGuidance);
    this->viewTargetGuidanceAction->setEnabled(false);

    this->createNewTargetGuidanceAction = new QAction(QIcon(":/image/open.png"),"&Create New Guidance",this);
    QObject::connect(this->createNewTargetGuidanceAction, &QAction::triggered, this, &MainWindow::newImage);

    this->editTargetGuidanceAction = new QAction(QIcon(":/image/open.png"),"&Edit Target Guidance",this);
    QObject::connect(this->editTargetGuidanceAction, &QAction::triggered, this, &MainWindow::editTargetGuidance);
    this->editTargetGuidanceAction->setEnabled(false);

    this->aboutAction = new QAction(QIcon(":/image/open.png"),"&About",this);
    QObject::connect(this->aboutAction, &QAction::triggered, this, &MainWindow::about);

    //Action 插入段
    this->sourceImageWidget->addAction(this->viewSourceImageAction);     //左上角的view
    this->sourceImageWidget->addAction(this->loadSourceImageAction);     //左上角的load

    this->sourceGuidanceWidget->addAction(new QAction("&View",this));
    this->sourceGuidanceWidget->addAction(this->editSourceGuidanceAction);

    this->targetGuidanceWidget->addAction(this->viewTargetGuidanceAction);
    this->targetGuidanceWidget->addAction(this->createNewTargetGuidanceAction);
    this->targetGuidanceWidget->addAction(this->editTargetGuidanceAction);


}


/**
 * @brief MainWindow::loadSourceImage
 * @brief load image的action的入口函数，用于载入一个原图，并显示在左上角，顺带初始化右上角的source guidance（同等大小的白图）。
 * @param 没有参数
 * @return 没有返回值
 */
void MainWindow::loadSourceImage(){
    QFileDialog* fileDialog = new QFileDialog(this, tr("Open image File"), ".", tr("Image files(*.png *.jpg *.JPG)"));
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    fileDialog->setViewMode(QFileDialog::Detail);

    if (fileDialog->exec() == QDialog::Accepted)    //成功选择一个png图片
    {
        this->sourceImage =new QImage(fileDialog->selectedFiles().first()); //读入图片

        QString filename;   //取出完整文件名
        for(int i = 1; ;i++){
            QString section = fileDialog->selectedFiles().first().section('/',i,i);
            if(section.size() == 0){
                break;
            }else{
                filename = section;
            }
        }
        QString filenameWithoutSuffix = filename.section('.',0,0);

        QDir qdir;
        QFileInfo dirNameInfo = QFileInfo(filenameWithoutSuffix);
        //判断当前目录下，以该文件命名的文件夹是否存在
        if(dirNameInfo.isDir() == false){
            qdir.mkdir(filenameWithoutSuffix);  //新建文件夹
        }else{

        }
        Util::dirName = filenameWithoutSuffix;  //给静态类赋值，保存目录名称

        this->sourceImage->save((Util::dirName+"/sourceImage.png").toUtf8().data());
        this->sourceImageFrame->setStyleSheet("background-image: url("+Util::dirName+"/sourceImage.png);background-position:center center;background-repeat: no-repeat");

        this->sourceGuidance = new QImage(this->sourceImage->width(), this->sourceImage->height(), QImage::Format_RGB888);  //新建一个同等大小的空图像
        this->sourceGuidance->fill(QColor(255,255,255));        //填充白色
        for(QString elem : Util::guidanceChannel){
            this->sourceGuidance->save(Util::dirName+"/sourceGuidance"+elem+".png");
        }
        this->sourceGuidance->save((Util::dirName+"/multiLabelClassificationResult.png").toUtf8().data());
        this->sourceGuidanceFrame->setStyleSheet("background-image: url("+Util::dirName+"/sourceGuidanceLabelChannel.png);background-position:center center;background-repeat: no-repeat"); //显示在右上角

        this->sourceImageWidgetStackedLayout->setCurrentIndex(1);
        this->sourceGuidanceWidgetStackedLayout->setCurrentIndex(1);

        this->viewSourceImageAction->setEnabled(true);
        this->viewSourceImageAction->setEnabled(true);
        this->editSourceGuidanceAction->setEnabled(true);
    }
}

MainWindow::~MainWindow()
{
}


/**
 * @brief MainWindow::initWindowLayout
 * @brief 用于管理mainwindow的窗口设置、layout布局、内部控件声明及摆放
 * @param 没有参数
 * @return 没有返回值
 */
void MainWindow::initWindowLayout(){
    this->setWindowState(Qt::WindowMaximized);      //设置窗口最大化
    this->setStyleSheet("background-color: #333337; color: #f1f1f1;");      //设置窗口样式（酷黑）

    QWidget* centerWidget = new QWidget(this);  //声明widget并设置为窗口的中央widget（这样才能设置layout）
    this->setCentralWidget(centerWidget);

    QGridLayout* gridLayout = new QGridLayout(centerWidget);    //在中央widget开启网格布局
    centerWidget->setLayout(gridLayout);

    this->sourceImageWidget = new QWidget(centerWidget);  //左上角widget声明：放置source image
    this->sourceImageWidget->setStyleSheet("background-color: #696969;");
    gridLayout->addWidget(sourceImageWidget,0,0);
    this->sourceGuidanceWidget = new QWidget(centerWidget);   //右上角label声明：放置source guidance
    this->sourceGuidanceWidget->setStyleSheet("background-color: #696969;");
    gridLayout->addWidget(this->sourceGuidanceWidget,0,1);
    this->targetGuidanceWidget = new QWidget(centerWidget);   //  左下角label声明：放置target guidance
    this->targetGuidanceWidget->setStyleSheet("background-color: #696969;");
    gridLayout->addWidget(this->targetGuidanceWidget,1,0);
    this->targetImageWidget = new QWidget(centerWidget);  //右下角widget声明：放置target image
    this->targetImageWidget->setStyleSheet("background-color: #696969;");
    gridLayout->addWidget(targetImageWidget,1,1);


    this->sourceImageWidget->setContextMenuPolicy(Qt::ActionsContextMenu);       //激活右键菜单策略
    this->sourceGuidanceWidget->setContextMenuPolicy(Qt::ActionsContextMenu);       //激活右键菜单策略
    this->targetGuidanceWidget->setContextMenuPolicy(Qt::ActionsContextMenu);       //激活右键菜单策略
    this->targetImageWidget->setContextMenuPolicy(Qt::ActionsContextMenu);      //激活右键菜单策略

    this->sourceImageWidgetStackedLayout = new QStackedLayout(this->sourceImageWidget);
    this->sourceImageWidget->setLayout(this->sourceImageWidgetStackedLayout);
    this->sourceImageFrame = new QFrame(this->sourceImageWidget);
    QLabel* hintLabel1 = new QLabel("Source Image Area", this->sourceImageWidget);
    hintLabel1->setStyleSheet("font-size : 40px");
    hintLabel1->setAlignment(Qt::AlignCenter);
    this->sourceImageWidgetStackedLayout->addWidget(hintLabel1);
    this->sourceImageWidgetStackedLayout->addWidget(this->sourceImageFrame);


    this->sourceGuidanceWidgetStackedLayout = new QStackedLayout(this->sourceGuidanceWidget);    //在label内建一个layout放置一个Frame，可以保证右键菜单的背景不受改变
    this->sourceGuidanceWidget->setLayout(this->sourceGuidanceWidgetStackedLayout);
    this->sourceGuidanceFrame = new QFrame(this->sourceGuidanceWidget);
    QLabel* hintLabel2 = new QLabel("Source Guidance Area", this->sourceGuidanceWidget);
    hintLabel2->setStyleSheet("font-size : 40px");
    hintLabel2->setAlignment(Qt::AlignCenter);
    this->sourceGuidanceWidgetStackedLayout->addWidget(hintLabel2);
    this->sourceGuidanceWidgetStackedLayout->addWidget(this->sourceGuidanceFrame);

    this->targetGuidanceWidgetStackedLayout = new QStackedLayout(this->targetGuidanceWidget);    //在label内建一个layout放置一个Frame，可以保证右键菜单的背景不受改变
    this->targetGuidanceWidget->setLayout(this->targetGuidanceWidgetStackedLayout);
    this->targetGuidanceFrame = new QFrame(this->targetGuidanceWidget);
    QLabel* hintLabel3 = new QLabel("Target Guidance Area", this->targetGuidanceWidget);
    hintLabel3->setStyleSheet("font-size : 40px");
    hintLabel3->setAlignment(Qt::AlignCenter);
    this->targetGuidanceWidgetStackedLayout->addWidget(hintLabel3);
    this->targetGuidanceWidgetStackedLayout->addWidget(this->targetGuidanceFrame);

    this->targetImageWidgetStackedLayout = new QStackedLayout(this->targetImageWidget);    //在label内建一个layout放置一个Frame，可以保证右键菜单的背景不受改变
    this->targetImageWidget->setLayout(this->targetImageWidgetStackedLayout);
    this->targetImageFrame = new QFrame(this->targetImageWidget);
    QLabel* hintLabel4 = new QLabel("Target Image Area", this->targetImageWidget);
    hintLabel4->setStyleSheet("font-size : 40px");
    hintLabel4->setAlignment(Qt::AlignCenter);
    this->targetImageWidgetStackedLayout->addWidget(hintLabel4);
    this->targetImageWidgetStackedLayout->addWidget(this->targetImageFrame);

    this->targetImageWidget->addAction(new QAction("&View",this));

}



/**
 * @brief MainWindow::newImage
 * @brief mainwindow中点击新建导引图片的action
 * @param 没有参数
 * @return 没有返回值
 */
void MainWindow::newImage(){
    NewImageDialog* newImageDialog = new NewImageDialog();
    if(newImageDialog->exec() == QDialog::Accepted){
        this->targetGuidanceFrame->setStyleSheet("background-image: url("+Util::dirName+"/targetGuidanceLabelChannel.png);background-position:center center;background-repeat: no-repeat"); //显示在右上角
    }
    this->targetGuidanceWidgetStackedLayout->setCurrentIndex(1);
    this->viewTargetGuidanceAction->setEnabled(true);
    this->editTargetGuidanceAction->setEnabled(true);
}





void MainWindow::editSourceGuidance(){
    ImageEditWindow* imageEditWindow = new ImageEditWindow(config::sourceGuidance, config::editable, this);
    imageEditWindow->show();
}



void MainWindow::viewSourceImage(){
    ImageEditWindow* imageEditWindow = new ImageEditWindow(config::sourceImage, config::readOnly, this);
    imageEditWindow->show();
}


void MainWindow::about()
{
    QMessageBox::about(this, tr("About Uncle Zhou UI"),
                       tr("<p><b>Uncle Zhou UI</b></p>"
                          "Open sourced under the <b>MIT</b> license.<br/>"
                          "Copyright (c) 2015 NetBeen<br/>"
                          "netbeen.cn@gmail.com"));


}


void MainWindow::editTargetGuidance(){
    ImageEditWindow* imageEditWindow = new ImageEditWindow(config::targetGuidance, config::editable, this);
    imageEditWindow->show();
}

void MainWindow::viewTargetGuidance(){
    ImageEditWindow* imageEditWindow = new ImageEditWindow(config::targetGuidance, config::readOnly, this);
    imageEditWindow->show();
}
