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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
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
    this->sourceImageLabel->addAction(new QAction("&View",this));
    QGridLayout* sourceImageLabelGridLayout = new QGridLayout(this->sourceImageLabel);    //在label内建一个layout放置一个Frame，可以保证右键菜单的背景不受改变
    this->sourceImageLabel->setLayout(sourceImageLabelGridLayout);
    this->sourceImageFrame = new QFrame(this->sourceImageLabel);
    sourceImageLabelGridLayout->addWidget(this->sourceImageFrame,0,0);

    this->sourceGuidanceLabel = new QLabel(centerWidget);   //右上角label声明：放置source guidance
    this->sourceGuidanceLabel->setStyleSheet("background-color: #696969;");
    gridLayout->addWidget(this->sourceGuidanceLabel,0,1);
    this->sourceGuidanceLabel->setContextMenuPolicy(Qt::ActionsContextMenu);       //激活右键菜单策略
    this->sourceGuidanceLabel->addAction(new QAction("&View",this));
    this->sourceGuidanceLabel->addAction(new QAction("&Edit",this));
    QGridLayout* sourceGuidanceLabelGridLayout = new QGridLayout(this->sourceGuidanceLabel);    //在label内建一个layout放置一个Frame，可以保证右键菜单的背景不受改变
    this->sourceGuidanceLabel->setLayout(sourceGuidanceLabelGridLayout);
    this->sourceGuidanceFrame = new QFrame(this->sourceGuidanceLabel);
    sourceGuidanceLabelGridLayout->addWidget(this->sourceGuidanceFrame,0,0);

    QLabel* label3 = new QLabel(centerWidget);
    label3->setStyleSheet("background-color: #696969;");
    gridLayout->addWidget(label3,1,0);
    label3->setContextMenuPolicy(Qt::ActionsContextMenu);       //激活右键菜单策略
    label3->addAction(new QAction("&View",this));

    QLabel* label4 = new QLabel(centerWidget);
    label4->setStyleSheet("background-color: #696969;");
    gridLayout->addWidget(label4,1,1);
    label4->setContextMenuPolicy(Qt::ActionsContextMenu);       //激活右键菜单策略
    label4->addAction(new QAction("&View",this));

    QAction* openAction = new QAction(QIcon(":/image/open.png"),"&Open",this);
    QObject::connect(openAction, &QAction::triggered, this, &MainWindow::loadSourceImage);

    QMenu* file = this->menuBar()->addMenu("&File");
    file->addAction(openAction);

    QMenu* htlp = this->menuBar()->addMenu("&Help");

    QToolBar* toolbar = this->addToolBar("Standard Tool Bar");
    toolbar->addAction(openAction);

    QStatusBar* statusBar = this->statusBar();

}

//载入source image的入口函数
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
