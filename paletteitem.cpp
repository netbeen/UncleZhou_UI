#include "paletteitem.h"

#include <QPainter>

/**
 * @brief PaletteItem::PaletteItem
 * @brief 调色板条目的构造函数
 * @param inputColor 初始化时的颜色设定
 * @param parent 父对象
 * @return 没有返回值
 */
PaletteItem::PaletteItem(QColor inputColor, QWidget* parent) : QWidget(parent),color(inputColor)
{
}

/**
 * @brief PaletteItem::paintEvent
 * @brief 调色板条目自身绘制函数
 * @param event
 * @return 没有返回值
 */
void PaletteItem::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setPen(QPen(Qt::gray, 2, Qt::SolidLine, Qt::FlatCap));
    painter.setBrush(this->color);
    painter.drawRect(rect());
}


/**
 * @brief PaletteItem::getColor
 * @brief 获取当前颜色的接口
 * @return 没有返回值
 */
QColor PaletteItem::getColor() const{
    return this->color;
}


/**
 * @brief PaletteItem::mouseReleaseEvent
 * @brief 响应鼠标点击事件
 * @param event
 * @return 没有返回值
 */
void PaletteItem::mouseReleaseEvent(QMouseEvent *event){
    emit this->colorSelected(this->color);
}
