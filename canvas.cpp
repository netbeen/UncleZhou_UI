#include "canvas.h"

#include <QPainter>
#include <QPoint>
#include <QDebug>
#include <QSize>

#include <assert.h>

Canvas::Canvas(QWidget* parent) : QWidget(parent), scaleFactor(1.0),operationType(config::None)
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
//void Canvas::init(config::editPosition editPosition){
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


//}

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

    this->topLeftPoint.setX((this->width() - imageSize.width()) / 2);
    this->topLeftPoint.setY((this->height() - imageSize.height()) / 2);

    painter.drawPixmap(this->topLeftPoint, this->surfacePixmap);   //绘制，制定左上角，绘制pixmap

}


void Canvas::receiveDisplayLayerChanged(){
    //this->surfaceImage = this->layerManager->layerItemVector.at(index)->image;
    this->surfaceImage = this->layerManager->getDisplayLayerItem()->image;
    this->surfacePixmap = QPixmap::fromImage(this->surfaceImage);
    this->update();
    //qDebug() << "Canvas::receiveDisplayLayerChanged index=" << index;
}


void Canvas::mousePressEvent(QMouseEvent *e){
    if(e->buttons() & Qt::LeftButton && this->isContained(e->pos())){
        switch (this->operationType) {
            case config::None:
                return;
            case config::Pencil:
                this->paint(this->mapToPixmap(e->pos()),10,QColor(255,0,0));
                break;
        }
    }
}


void Canvas::mouseMoveEvent(QMouseEvent *e){
    if( e->buttons() & Qt::LeftButton && this->isContained(e->pos())){
        switch (this->operationType) {
            case config::None:
                return;
            case config::Pencil:
                this->paint(this->mapToPixmap(e->pos()),10,QColor(255,0,0));
                break;
        }
    }
}


bool Canvas::isContained(const QPoint testPoint) const{
    assert(this->surfaceImage.isNull() == false);
    QSize canvasSize = QSize(this->surfaceImage.width(), this->surfaceImage.height());
    QRect canvasRect = QRect(this->topLeftPoint, canvasSize);
    return canvasRect.contains(testPoint);
}

void Canvas::setOperationType(config::operationType inputOperationType){
    this->operationType = inputOperationType;
}


void Canvas::paint(const QPoint center, const int radius,const QColor color){
    LayerItem* currentDisplayLayerItem = this->layerManager->getDisplayLayerItem();
    //currentDisplayLayerItem->image.setPixel(center.x(), center.y(),color.rgb());

    for(int x = center.x()-radius; x < center.x()+radius; x++){
        for(int y = center.y()-radius; y < center.y()+radius; y++){
            if(Util::calcL2Distance(center, QPoint(x,y)) < radius){
                currentDisplayLayerItem->image.setPixel(x, y,color.rgb());
            }
        }
    }

    this->surfacePixmap = QPixmap::fromImage(currentDisplayLayerItem->image);
    this->update();
}




/**
 * @brief Canvas::mapToPixmap
 * @brief 将canvas中的一个点映射至实际图像的某一个像素上
 * @param screenPoint   canvas坐标中的点
 * @return 返回实际像素点的位置
 */
QPoint Canvas::mapToPixmap(QPoint screenPoint){
    QPoint finalPosition;
    finalPosition.setX(screenPoint.x() - this->topLeftPoint.x()/this->scaleFactor);
    finalPosition.setY(screenPoint.y() - this->topLeftPoint.y()/this->scaleFactor);
    return finalPosition;
}



