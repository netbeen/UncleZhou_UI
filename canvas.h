#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QMouseEvent>

#include "layermanager.h"
#include "util.h"

class Canvas : public QWidget
{
    Q_OBJECT
public:
    Canvas(QWidget* parent);
    //void init(config::editPosition editPosition);
    void setOperationType(config::operationType inputOperationType);

    void receiveDisplayLayerChanged();  //slot函数，用于接受layerManager传来的图层切换信号

protected:
    virtual void paintEvent(QPaintEvent* e) override;
    virtual void mousePressEvent(QMouseEvent *e) override;
    virtual void mouseMoveEvent(QMouseEvent *e) override;

private:
    QImage surfaceImage;
    QPixmap surfacePixmap;

    LayerManager* layerManager;

    float scaleFactor;      //缩放因子
    QPoint topLeftPoint;     //绘图起始的左上角的坐标点
    QPoint moveStartPoint;
    QPoint topLeftPointBackup;  //用于移动的时候保存左上角点作为参照坐标
    config::operationType operationType;
    bool isInited;

    bool isContained(const QPoint testPoint) const;
    void paint(const QPoint center, const int radius,const QColor color);   //绘图函数，对图像进行实际修改
    void erase(const QPoint center, const int radius);  //擦除函数
    QPoint mapToPixmap(QPoint screenPoint);

signals:


};

#endif // CANVAS_H
