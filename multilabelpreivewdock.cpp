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
MultiLabelPreivewDock::MultiLabelPreivewDock(QWidget* parent):QDockWidget(parent),activeButtonIndex(-1),isMultiLabelChecked(false){

    this->colorVector = std::vector<cv::Vec3b>();
    this->colorButtonVector = std::vector<QPushButton*>();
    this->layerManager = LayerManager::getInstance();

    this->initDockLayout();
}

/**
 * @brief MultiLabelPreivewDock::buttonClickedSlot
 */
void MultiLabelPreivewDock::buttonClickedSlot(){
    QPushButton* focusedButton = nullptr;
    for(size_t i = 0; i < this->colorButtonVector.size(); i++){
        QPushButton* buttonElem = this->colorButtonVector.at(i);
        if(buttonElem->hasFocus()){
            focusedButton = buttonElem;
            this->activeButtonIndex = i;
            emit this->sendClassificationColor(this->colorVector.at(i));
        }
        buttonElem->setChecked(false);
    }
    focusedButton->setChecked(true);
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

    //添加按钮
    for(cv::Vec3b colorElem : this->colorVector){
        QPushButton* button = new QPushButton(this);
        button->setCheckable(true);
        QObject::connect(button,&QPushButton::clicked, this, &MultiLabelPreivewDock::buttonClickedSlot);

        if(this->isMultiLabelChecked == true){
            button->setStyleSheet("background-color: rgb(192,192,192);");
            button->setEnabled(false);
        }else{
            button->setStyleSheet("background-color: rgb("+QString::number(colorElem[2],10) +","+ QString::number(colorElem[1],10)+","+ QString::number(colorElem[0],10)+");");
            button->setEnabled(true);
        }
        this->colorButtonVector.push_back(button);
        this->colorButtonLayout->addWidget(button);
    }

    //检测激活按钮的ID，若满足要求，则把对应按钮置为被激活状态
    if(this->activeButtonIndex < static_cast<int>(this->colorButtonVector.size()) && this->activeButtonIndex >= 0){
        this->colorButtonVector.at(this->activeButtonIndex)->setChecked(true);
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

void MultiLabelPreivewDock::multiLabelCheckedSlot(bool flag){
    this->isMultiLabelChecked = flag;
    this->updateColorButtonLayout();
    emit this->multiLabelCheckedSinal(flag);
}

void MultiLabelPreivewDock::initDockLayout(){
    this->setMinimumSize(300, 300);
    this->setStyleSheet("background-color: #696969; padding: 0px;");
    this->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    this->setContentsMargins(0, 0, 0, 0);

    QWidget* mainWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(mainWidget);

    this->multiLabelCanvas = new ReadonlyCanvas(Util::dirName+"/multiLabelClassificationResult.png",this);
    layout->addWidget(this->multiLabelCanvas);

    mainWidget->setLayout(layout);
    this->setWidget(mainWidget);

    this->colorButtonLayout = new QHBoxLayout(mainWidget);
    layout->addLayout(this->colorButtonLayout);

    QCheckBox* multiLabelCheckbox = new QCheckBox("Multi Label Editor",this);
    layout->addWidget(multiLabelCheckbox);
    QObject::connect(multiLabelCheckbox, &QCheckBox::toggled, this, &MultiLabelPreivewDock::multiLabelCheckedSlot);
}
