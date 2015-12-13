#include "swatch.h"

#include <QPainter>
#include <QColorDialog>

/**
 * @brief Swatch::Swatch
 * @brief 本类作为颜色指示器，显示前景色与背景色
 * @param parent 作为父指针
 * @return 没有返回值
 */
Swatch::Swatch(QWidget *parent) : QWidget(parent)
{
    this->currentColor = QColor(255,0,0);

}



QColor Swatch::getColor() const{
    return this->currentColor;
}


/**
 * @brief Swatch::paintEvent
 * @brief 自身绘制函数
 * @param event
 * @return 没有返回值
 */
void Swatch::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setPen(QPen(Qt::gray, 2, Qt::SolidLine, Qt::FlatCap));
    painter.setBrush(currentColor);
    painter.drawRect(rect());
}


/**
 * @brief Swatch::mouseReleaseEvent
 * @brief 响应鼠标点击事件
 * @param event
 * @return 没有返回值
 */
void Swatch::mouseReleaseEvent(QMouseEvent *event)
{
    /*if (event->button() == Qt::LeftButton)
    {
        currentColor = QColorDialog::getColor();
        this->update();     //更新自身绘制
        emit(this->colorSelected(this->currentColor));
    }*/
}


/**
 * @brief Swatch::setColor
 * @brief 设置当前示色器颜色的接口
 * @param inputColor 表示输入的颜色
 * @return 没有返回值
 */
void Swatch::setColor(QColor inputColor){
    this->currentColor = inputColor;
    this->update();
}
