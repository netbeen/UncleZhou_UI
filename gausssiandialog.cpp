#include "gausssiandialog.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDial>
#include <iostream>
#include <QLabel>

GausssianDialog::GausssianDialog(int xCenterCoordinate, int yCenterCoordinate, QColor color, QImage& image, QWidget* parent):QDialog(parent),imageWidth(image.width()),imageHeight(image.height()),xCenterCoordinate(xCenterCoordinate),yCenterCoordinate(yCenterCoordinate),color(color){
    this->initDialogLayout();
    this->ptr2image = &image;
    this->gaussianFunctionPainter = new GaussianFunctionPainter(xCenterCoordinate, yCenterCoordinate,imageWidth/2,imageHeight/2);
    this->gaussianFunctionPainter->draw(image,color);
}

void GausssianDialog::initDialogLayout(){
    this->setWindowTitle("Please set the sigma parameter");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    this->setLayout(mainLayout);

    QHBoxLayout* dialLayout = new QHBoxLayout(this);
    mainLayout->addLayout(dialLayout);

    QVBoxLayout* xDialLayout = new QVBoxLayout(this);
    dialLayout->addLayout(xDialLayout);

    QVBoxLayout* yDialLayout = new QVBoxLayout(this);
    dialLayout->addLayout(yDialLayout);

    xDialLayout->addWidget(new QLabel("Sigma X:",this));
    yDialLayout->addWidget(new QLabel("Sigma Y:",this));

    this->xDial = new QDial(this);
    this->xDial->setMinimumSize(200,200);
    this->yDial = new QDial(this);
    this->yDial->setMinimumSize(200,200);

    xDialLayout->addWidget(xDial);
    yDialLayout->addWidget(yDial);

    xDial->setMinimum(0);
    xDial->setMaximum(this->imageWidth);
    xDial->setValue(this->imageWidth/2);

    yDial->setMinimum(0);
    yDial->setMaximum(this->imageHeight);
    yDial->setValue(this->imageHeight/2);

    QObject::connect(xDial,&QDial::valueChanged,this,&GausssianDialog::sigmaXChangedSlot);
    QObject::connect(yDial,&QDial::valueChanged,this,&GausssianDialog::sigmaYChangedSlot);
}

void GausssianDialog::sigmaXChangedSlot(int newSigmaX){
    this->gaussianFunctionPainter->setSigma(newSigmaX,this->yDial->value());
    this->gaussianFunctionPainter->draw(*(this->ptr2image), this->color);
    emit this->updateCanvas();
}

void GausssianDialog::sigmaYChangedSlot(int newSigmaY){
    this->gaussianFunctionPainter->setSigma(this->xDial->value(), newSigmaY);
    this->gaussianFunctionPainter->draw(*(this->ptr2image), this->color);
    emit this->updateCanvas();
}
