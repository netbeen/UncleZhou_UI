#include "paletteitem.h"

#include <QPainter>

PaletteItem::PaletteItem(QColor inputColor, QWidget* parent) : QWidget(parent),color(inputColor)
{

}

void PaletteItem::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setPen(QPen(Qt::gray, 2, Qt::SolidLine, Qt::FlatCap));
    painter.setBrush(this->color);
    painter.drawRect(rect());
}

QColor PaletteItem::getColor() const{
    return this->color;
}


void PaletteItem::mouseReleaseEvent(QMouseEvent *event){
    emit this->colorSelected(this->color);
}
