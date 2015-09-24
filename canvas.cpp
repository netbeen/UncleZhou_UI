#include "canvas.h"

#include <QPainter>
#include <QPoint>
#include <QDebug>
#include <QSize>

Canvas::Canvas(QWidget* parent) : QWidget(parent)
{

}

void Canvas::init(config::editPosition editPosition){
    switch(editPosition){
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
    }

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

    QSize pixSize = this->surfacePixmap.size();
    this->setMinimumSize(pixSize);      //将canvas的最小尺寸限定在图像大小，若窗口继续缩小，则外层的scrollArea会生成滚动条

    QPoint topleft;     //指定左上角点作为起始位置，计算左上角可以使得图像居中
    topleft.setX((this->width() - pixSize.width()) / 2);
    topleft.setY((this->height() - pixSize.height()) / 2);

    painter.drawPixmap(topleft, this->surfacePixmap);

}
