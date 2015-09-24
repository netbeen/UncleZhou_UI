#ifndef SWATCH_H
#define SWATCH_H

#include <QWidget>
#include <QColor>
#include <QMouseEvent>

class Swatch : public QWidget
{
    Q_OBJECT
public:
    Swatch(QWidget *parent);
    QColor getColor() const;
    void setColor(QColor);

private:
    QColor currentColor;

    //绘制函数，用于绘制自身
    void paintEvent(QPaintEvent* event);

    //鼠标事件相应函数
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void colorSelected(QColor currentColor);

};

#endif // SWATCH_H
