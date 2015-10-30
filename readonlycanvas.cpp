#include "readonlycanvas.h"
#include <QPainter>

ReadonlyCanvas::ReadonlyCanvas(QString filename, QWidget* parent) :QWidget(parent), filename(filename){

}

void ReadonlyCanvas::paintEvent(QPaintEvent* e){

    QImage image = QImage(filename);
    if(image.isNull()){
        return;
    }
    QPixmap  pixmap = QPixmap::fromImage(image);
    pixmap = pixmap.scaled(this->size(), Qt::KeepAspectRatio);

    QPoint topLeftPoint = QPoint(0,0);
    topLeftPoint.setX((this->width()-pixmap.width())/2);
    topLeftPoint.setY((this->height()-pixmap.height())/2);

    QPainter painter(this);     //声明绘制器
    painter.drawPixmap(topLeftPoint, pixmap);   //绘制，制定左上角，绘制pixmap
}
