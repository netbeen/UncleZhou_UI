#include "binaryclassificationdialog.h"
#include "layermanager.h"
#include <QRadioButton>
#include <QVBoxLayout>



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
        QString radioName = QString::number(elem[0]) + " "+ QString::number(elem[1]) + " " + QString::number(elem[2]);
        QRadioButton* radio = new QRadioButton(radioName);
        QObject::connect(radio, &QRadioButton::clicked, this, &BinaryClassificationDialog::findTheToggledButton);
        radioLayout->addWidget(radio);
        this->radioButtonVector.push_back(radio);
    }

}

void BinaryClassificationDialog::findTheToggledButton(){
    size_t index = 0;
    while(this->radioButtonVector.at(index)->isChecked() == false){
        index++;
        if(index == radioButtonVector.size()){
            return;
        }
    }

    this->sendColor(this->colorVector.at(index));
}
