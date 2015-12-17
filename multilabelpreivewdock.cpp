#include "multilabelpreivewdock.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <unordered_set>

/**
 * @brief MultiLabelPreivewDock::MultiLabelPreivewDock
 * @brief 构造函数
 * @param parent
 */
MultiLabelPreivewDock::MultiLabelPreivewDock(QWidget* parent):QDockWidget(parent){

    this->colorVector = std::vector<cv::Vec3b>();
    this->colorButtonVector = std::vector<QPushButton*>();
    this->layerManager = LayerManager::getInstance();

    std::cout << this->colorVector.size() << std::endl;
    this->initDockLayout();
}

/**
 * @brief MultiLabelPreivewDock::addColor
 * @brief 每当画笔画下任何颜色，都会调用
 * @param newColor
 */
void MultiLabelPreivewDock::addColor(cv::Vec3b newColor){
    qDebug("In");
    std::vector<cv::Vec3b>::iterator iterator;// = std::find(this->colorVector.begin(), this->colorVector.end(), newColor);
    std::cout << "newColor:" << newColor << std::endl;
    if(cv::Vec3b(255,0,0) == newColor){
        ;
    }
    qDebug("2");
    bool existed = false;
    std::cout << this->colorVector.size() << std::endl;
    //this->colorVector.push_back(cv::Vec3b(0,123,124));
    //std::cout << this->colorVector.size() << std::endl;
    //std::cout << this->colorVector.front() << std::endl;
    //for(cv::Vec3b colorElem : this->colorVector){
        /*if(colorElem == newColor){
            existed = true;
            break;
        }*/
    //}

    /*if(iterator == this->colorVector.end()){
        qDebug("3");
        this->colorVector.push_back(newColor);
        this->updateColorButtonLayer();
    }else{
        qDebug("4");
        return;
    }*/

    /*if(existed == false){
        qDebug("5");
        this->colorVector.push_back(newColor);
        this->updateColorButtonLayer();
    }else{
        qDebug("6");
        return;
    }*/
}

/**
 * @brief MultiLabelPreivewDock::updateColorButtonLayout
 * @brief 删除所有按钮，然后重新插入按钮
 */
void MultiLabelPreivewDock::updateColorButtonLayout(){
    //删除所有按钮
    for(QPushButton* buttonElem : this->colorButtonVector){
        this->colorButtonLayout->removeWidget(buttonElem);
        buttonElem->deleteLater();
    }
    this->colorButtonVector.clear();
    for(cv::Vec3b colorElem : this->colorVector){
        QPushButton* button = new QPushButton(this);
        button->setCheckable(true);
        button->setStyleSheet("background-color: rgb("+QString::number(colorElem[2],10) +","+ QString::number(colorElem[1],10)+","+ QString::number(colorElem[0],10)+");");
        this->colorButtonVector.push_back(button);
        this->colorButtonLayout->addWidget(button);
    }
}


void MultiLabelPreivewDock::receiveUpdateColorLayoutSlot(){
    QImage image = this->layerManager->getDisplayLayerItem()->image;
    cv::Mat_<cv::Vec3b> cvImage;
    Util::convertQImageToMat(image,cvImage);

    std::vector<bool> colorExisted(this->colorVector.size(), false);

    std::vector<cv::Vec3b>::iterator iter;
    cv::Vec3b currentColor;
    cv::Vec3b white = cv::Vec3b(255,255,255);
    for(int y_offset = 0; y_offset < cvImage.rows; y_offset++){
        for(int x_offset = 0; x_offset < cvImage.cols; x_offset++){
            currentColor = cvImage.at<cv::Vec3b>(y_offset,x_offset);
            if(currentColor == white){
                continue;
            }
            iter = std::find(this->colorVector.begin(), this->colorVector.end(), currentColor);
            if(iter == this->colorVector.end()){
                this->colorVector.push_back(currentColor);
                colorExisted.push_back(true);
            }else{
                colorExisted.at(iter-this->colorVector.begin()) = true;
            }
        }
    }

    for(int i = colorVector.size()-1; i >= 0; i--){
        if(colorExisted.at(i) == false){
            this->colorVector.erase(this->colorVector.begin() + i);
        }
    }

    this->updateColorButtonLayout();

}

void MultiLabelPreivewDock::initDockLayout(){
    this->setMinimumSize(300, 300);
    this->setStyleSheet("background-color: #696969; padding: 0px;");
    this->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    this->setContentsMargins(0, 0, 0, 0);

    QWidget* mainWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(mainWidget);

    this->multiLabelCanvas = new ReadonlyCanvas("multiLabelClassificationResult.png",this);
    layout->addWidget(this->multiLabelCanvas);

    mainWidget->setLayout(layout);
    this->setWidget(mainWidget);

    this->colorButtonLayout = new QHBoxLayout(mainWidget);
    layout->addLayout(this->colorButtonLayout);

    layout->addWidget(new QCheckBox("Checkbox"));
}
