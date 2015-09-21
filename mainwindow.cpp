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

    this->sourceImageLabel = new QLabel(centerWidget);
    this->sourceImageLabel->setStyleSheet("background-color: #696969;");
    gridLayout->addWidget(sourceImageLabel,0,0);
    QLabel* label2 = new QLabel(centerWidget);
    label2->setStyleSheet("background-color: #696969;");
    gridLayout->addWidget(label2,0,1);
    QLabel* label3 = new QLabel(centerWidget);
    label3->setStyleSheet("background-color: #696969;");
    gridLayout->addWidget(label3,1,0);
    QLabel* label4 = new QLabel(centerWidget);
    label4->setStyleSheet("background-color: #696969;");
    gridLayout->addWidget(label4,1,1);

    QAction* openAction = new QAction(QIcon(":/image/open.png"),"&Open",this);
    QObject::connect(openAction, &QAction::triggered, this, &MainWindow::loadSourceImage);

    QMenu* file = this->menuBar()->addMenu("&File");
    file->addAction(openAction);

    QMenu* about = this->menuBar()->addMenu("&About");

    QToolBar* toolbar = this->addToolBar("TOOL");
    toolbar->addAction(openAction);

    QStatusBar* statusBar = this->statusBar();

}

//载入source image的入口函数
void MainWindow::loadSourceImage(){
    QFileDialog* fileDialog = new QFileDialog(this, tr("Open image File"), ".", tr("Image files(*.png)"));
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    fileDialog->setViewMode(QFileDialog::Detail);

    if (fileDialog->exec() == QDialog::Accepted)
    {
        this->sourceImage =new QImage(fileDialog->selectedFiles().first());     //此处会有内存泄漏，以后处理
        this->sourceImageLabel->setPixmap(QPixmap::fromImage(*this->sourceImage));
    }
}

MainWindow::~MainWindow()
{
}
