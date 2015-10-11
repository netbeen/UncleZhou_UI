#include "navigatorcanvas.h"

#include <QPainter>

NavigatorCanvas::NavigatorCanvas(QWidget* parent) : QWidget(parent)
{
        this->layerManager = LayerManager::getInstance();

}


void NavigatorCanvas::paintEvent(QPaintEvent* e){

    QImage image = this->layerManager->getDisplayLayerItem()->image;
    QPixmap  pixmap = QPixmap::fromImage(image);
    pixmap = pixmap.scaled(this->size(), Qt::KeepAspectRatio);

    QPoint topLeftPoint = QPoint(0,0);
    topLeftPoint.setX((this->width()-pixmap.width())/2);
    topLeftPoint.setY((this->height()-pixmap.height())/2);

    QPainter painter(this);     //声明绘制器
    painter.drawPixmap(topLeftPoint, pixmap);   //绘制，制定左上角，绘制pixmap
}
