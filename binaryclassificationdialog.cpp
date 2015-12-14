#include "binaryclassificationdialog.h"
#include "layermanager.h"
#include <QRadioButton>
#include <QVBoxLayout>
#include <QPushButton>



BinaryClassificationDialog::BinaryClassificationDialog(QWidget* parent):QDialog(parent)
{
    this->layerManager = LayerManager::getInstance();

    this->initDialogLayout();
}


void BinaryClassificationDialog::initDialogLayout(){
    this->setWindowTitle("Please choose a color as positive example.");
    this->setMinimumSize(400, 150);

    QVBoxLayout* radioLayout = new QVBoxLayout(this);
    this->setLayout(radioLayout);

    LayerItem* currentDisplayLayerItem = this->layerManager->getDisplayLayerItem();

    QImage image = currentDisplayLayerItem->image;
    cv::Mat_<cv::Vec3b> cvMat;
    Util::convertQImageToMat(image,cvMat);

    this->colorVector = std::vector<cv::Vec3b>();
    cv::Vec3b write = cv::Vec3b(255,255,255);
    for(int y_offset = 0; y_offset < cvMat.rows; y_offset++){
        for(int x_offset = 0; x_offset < cvMat.cols; x_offset++){
            cv::Vec3b currentColor = cvMat.at<cv::Vec3b>(y_offset,x_offset);
            if(currentColor == write){
                continue;
            }
            bool existed = false;
            for(cv::Vec3b elem : this->colorVector){
                if(elem[0] == currentColor[0] && elem[1] == currentColor[1]  && elem[2] == currentColor[2] ){
                    existed = true;
                    break;
                }
            }
            if(existed == false){
                colorVector.push_back(currentColor);
            }
        }
    }

    for(cv::Vec3b elem : this->colorVector){
        QPushButton* pushButton = new QPushButton();
        pushButton->setStyleSheet("background-color: rgb("+QString::number(elem[2])+","+QString::number(elem[1])+","+ QString::number(elem[0]) + ");");
        QObject::connect(pushButton,&QPushButton::clicked,this,&BinaryClassificationDialog::findTheToggledButton);
        this->pushButtonVector.push_back(pushButton);
        radioLayout->addWidget(pushButton);
    }


}

void BinaryClassificationDialog::findTheToggledButton(){
    for(size_t i = 0; i < this->pushButtonVector.size(); i++){
        if(this->pushButtonVector.at(i)->hasFocus() == true){
            this->sendColor(this->colorVector.at(i));
            this->accept();
            return;
        }
    }
    exit(2);
}
