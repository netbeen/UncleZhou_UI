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



    QMenu* file = this->menuBar()->addMenu("&File");
    file->addAction(this->loadSourceImageAction);

    QMenu* htlp = this->menuBar()->addMenu("&Help");

    QToolBar* toolbar = this->addToolBar("Standard Tool Bar");
    toolbar->addAction(this->loadSourceImageAction);

    QStatusBar* statusBar = this->statusBar();

}


/**
 * @brief MainWindow::initAction
 * @brief 用于mainwindow的action的定义、connect、插入
 * @param 没有参数
 * @return 没有返回值
 */
void MainWindow::initAction(){
    this->loadSourceImageAction = new QAction(QIcon(":/image/open.png"),"&Load",this);
    QObject::connect(this->loadSourceImageAction, &QAction::triggered, this, &MainWindow::loadSourceImage);

    //Action 插入段
    this->sourceImageLabel->addAction(new QAction("&View",this));
    this->sourceImageLabel->addAction(this->loadSourceImageAction);     //左上角的load
    this->sourceGuidanceLabel->addAction(new QAction("&View",this));
    this->sourceGuidanceLabel->addAction(new QAction("&Edit",this));
    this->targetGuidanceLabel->addAction(new QAction("&View",this));
    this->targetGuidanceLabel->addAction(new QAction("&Create",this));
    this->targetGuidanceLabel->addAction(new QAction("&Edit",this));
}


/**
 * @brief MainWindow::loadSourceImage
 * @brief load image的action的入口函数，用于载入一个原图，并显示在左上角，顺带初始化右上角的source guidance（同等大小的白图）。
 * @param 没有参数
 * @return 没有返回值
 */
void MainWindow::loadSourceImage(){
    QFileDialog* fileDialog = new QFileDialog(this, tr("Open image File"), ".", tr("Image files(*.png)"));
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    fileDialog->setViewMode(QFileDialog::Detail);

    if (fileDialog->exec() == QDialog::Accepted)    //成功选择一个png图片
    {
        this->sourceImage =new QImage(fileDialog->selectedFiles().first());     //此处会有内存泄漏，以后处理
        const QString filename = fileDialog->selectedFiles().first();
        this->sourceImageFrame->setStyleSheet("background-image: url(" + filename + ");background-position:center center;background-repeat: no-repeat");

        this->sourceGuidance = new QImage(this->sourceImage->width(), this->sourceImage->height(), QImage::Format_RGB888);  //新建一个同等大小的空图像
        this->sourceGuidance->fill(QColor(255,255,255));        //填充白色
        this->sourceGuidance->save("./sourceGuidance.png");
        this->sourceGuidanceFrame->setStyleSheet("background-image: url(./sourceGuidance.png);background-position:center center;background-repeat: no-repeat"); //显示在右上角
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

    this->sourceImageLabel = new QLabel(centerWidget);  //左上角label声明：放置source image
    this->sourceImageLabel->setStyleSheet("background-color: #696969;");
    gridLayout->addWidget(sourceImageLabel,0,0);
    this->sourceImageLabel->setContextMenuPolicy(Qt::ActionsContextMenu);       //激活右键菜单策略
    QGridLayout* sourceImageLabelGridLayout = new QGridLayout(this->sourceImageLabel);    //在label内建一个layout放置一个Frame，可以保证右键菜单的背景不受改变
    this->sourceImageLabel->setLayout(sourceImageLabelGridLayout);
    this->sourceImageFrame = new QFrame(this->sourceImageLabel);
    sourceImageLabelGridLayout->addWidget(this->sourceImageFrame,0,0);

    this->sourceGuidanceLabel = new QLabel(centerWidget);   //右上角label声明：放置source guidance
    this->sourceGuidanceLabel->setStyleSheet("background-color: #696969;");
    gridLayout->addWidget(this->sourceGuidanceLabel,0,1);
    this->sourceGuidanceLabel->setContextMenuPolicy(Qt::ActionsContextMenu);       //激活右键菜单策略
    QGridLayout* sourceGuidanceLabelGridLayout = new QGridLayout(this->sourceGuidanceLabel);    //在label内建一个layout放置一个Frame，可以保证右键菜单的背景不受改变
    this->sourceGuidanceLabel->setLayout(sourceGuidanceLabelGridLayout);
    this->sourceGuidanceFrame = new QFrame(this->sourceGuidanceLabel);
    sourceGuidanceLabelGridLayout->addWidget(this->sourceGuidanceFrame,0,0);

    this->targetGuidanceLabel = new QLabel(centerWidget);   //  左下角label声明：放置target guidance
    this->targetGuidanceLabel->setStyleSheet("background-color: #696969;");
    gridLayout->addWidget(this->targetGuidanceLabel,1,0);
    this->targetGuidanceLabel->setContextMenuPolicy(Qt::ActionsContextMenu);       //激活右键菜单策略
    QGridLayout* targetGuidanceLabelGridLayout = new QGridLayout(this->targetGuidanceLabel);    //在label内建一个layout放置一个Frame，可以保证右键菜单的背景不受改变
    this->targetGuidanceLabel->setLayout(targetGuidanceLabelGridLayout);
    this->targetGuidanceFrame = new QFrame(this->targetGuidanceLabel);
    targetGuidanceLabelGridLayout->addWidget(this->targetGuidanceFrame,0,0);

    QLabel* label4 = new QLabel(centerWidget);
    label4->setStyleSheet("background-color: #696969;");
    gridLayout->addWidget(label4,1,1);
    label4->setContextMenuPolicy(Qt::ActionsContextMenu);       //激活右键菜单策略
    label4->addAction(new QAction("&View",this));

}
