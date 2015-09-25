#include "canvas.h"

#include <QPainter>
#include <QPoint>
#include <QDebug>
#include <QSize>

Canvas::Canvas(QWidget* parent) : QWidget(parent)
{
    this->layerManager = LayerManager::getInstance();
    QObject::connect(this->layerManager, &LayerManager::displayLayerChanged, this, &Canvas::receiveDisplayLayerChanged);
}


/**
 * @brief Canvas::init
 * @brief canvas初始化函数，用于初始化canvas的各项参数
 * @param editPosition 是指定canvas显示的数据来源，可选项有：原图、原引导、目标引导、目标图
 * @return 没有返回值
 */
void Canvas::init(config::editPosition editPosition){
    /*switch(editPosition){
        case config::sourceImage:
            this->surfaceImage = QImage("./sourceImage.png");
            this->surfacePixmap = QPixmap::fromImage(this->surfaceImage);

            break;
        case config::sourceGuidance:
            break;
        case config::targetGuidance:
            break;
        case config::targetImage:
            break;
    }*/


}

/**
 * @brief Canvas::paintEvent
 * @brief 重写virtual的绘制函数，系统自动调用
 * @param e
 * @return 没有返回值
 */
void Canvas::paintEvent(QPaintEvent* e){
    if(this->surfaceImage.isNull()){
        return;
    }

    QPainter painter(this);     //声明绘制器
    //painter.setRenderHint(QPainter::SmoothPixmapTransform);       //这条不知道啥用，先注释了

    QSize imageSize = this->surfacePixmap.size();
    this->setMinimumSize(imageSize);      //将canvas的最小尺寸限定在图像大小，若窗口继续缩小，则外层的scrollArea会生成滚动条

    QPoint topLeftPoint(-10,-10);     //指定左上角点作为起始位置，计算左上角可以使得图像居中
    topLeftPoint.setX((this->width() - imageSize.width()) / 2);
    topLeftPoint.setY((this->height() - imageSize.height()) / 2);

    painter.drawPixmap(topLeftPoint, this->surfacePixmap);   //绘制，制定左上角，绘制pixmap

}


void Canvas::receiveDisplayLayerChanged(int index){
    this->surfaceImage = this->layerManager->layerItemVector.at(index)->image;
    this->surfacePixmap = QPixmap::fromImage(this->surfaceImage);
    this->update();
    qDebug() << "Canvas::receiveDisplayLayerChanged index=" << index;
}
