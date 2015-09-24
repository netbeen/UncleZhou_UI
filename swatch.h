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

protected:
    //绘制函数，用于绘制自身
    virtual void paintEvent(QPaintEvent* event) override;

    //鼠标事件相应函数
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QColor currentColor;



signals:
    void colorSelected(QColor currentColor);

};

#endif // SWATCH_H
