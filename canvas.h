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

protected:
    virtual void paintEvent(QPaintEvent* e) override;
    virtual void mousePressEvent(QMouseEvent *e) override;

private:
    QImage surfaceImage;
    QPixmap surfacePixmap;

    LayerManager* layerManager;

    float scaleFactor;      //缩放因子
    QPoint topLeftPoint;     //绘图起始的左上角的坐标点
    config::operationType operationType;

    void receiveDisplayLayerChanged(int index);

    bool isContained(const QPoint testPoint) const;
    void paint(QPoint center, int radius, QColor color);   //绘图函数，对图像进行实际修改
    QPoint mapToPixmap(QPoint screenPoint);

signals:


};

#endif // CANVAS_H
