#include "imageeditwindow.h"

#include <QGridLayout>

ImageEditWindow::ImageEditWindow(config::editPosition editPosition, config::editLevel editLevel, QWidget* parent) : QMainWindow(parent)
{
    this->initWindowLayout();



}


void ImageEditWindow::initWindowLayout(){
    this->setStyleSheet("background-color: #333337; color: #f1f1f1;");
    this->setMinimumWidth(1024);
    this->setMinimumHeight(768);

    this->toolbar = new QToolBar("toolbar",this);
    this->addToolBar(Qt::LeftToolBarArea,this->toolbar);




}
