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
    void setOperationType(config::operationType inputOperationType);

    void receiveDisplayLayerChanged();  //slot函数，用于接受layerManager传来的图层切换信号
    void receiveShowBackground(bool isShow);   //slot，用于接受显示半透明背景的信号
    void setPencilRadius(int inputRadius);  //slot,用于接受参数设置铅笔半径
    void setEraserRadius(int inputRadius);  //slot ， 用于接受参数设置橡皮半径
    void setColor(QColor inputColor);   //slot， 用于接受参数设置当前颜色
    void resetScale();



protected:
    virtual void paintEvent(QPaintEvent* e) override;
    virtual void mousePressEvent(QMouseEvent *e) override;
    virtual void mouseMoveEvent(QMouseEvent *e) override;

private:
    QImage surfaceImage;
    QPixmap surfacePixmap;
    QImage backgroundImage;
    QPixmap backgroundPixmap;
    QImage alpha;

    LayerManager* layerManager;

    float scaleFactor;      //缩放因子
    QPoint topLeftPoint;     //绘图起始的左上角的坐标点
    QPoint moveStartPoint;
    QPoint topLeftPointBackup;  //用于移动的时候保存左上角点作为参照坐标
    config::operationType operationType;
    bool isInited;
    bool isShowBackground;
    int pencilRadius;
    int eraserRadius;
    QColor color;

    void setScale(float inputScaleFactor);
    bool isContained(const QPoint testPoint) const;
    void paint(const QPoint center, const int radius,const QColor color);   //绘图函数，对图像进行实际修改
    void erase(const QPoint center, const int radius);  //擦除函数
    void magicErase(const QPoint center);  //魔术擦除函数
    void bucket(const QColor color);
    void polygon(const QPoint center, const QColor color);  //多边形工具
    bool polygonStarted = false;
    QImage beforePolygonBackup;
    std::vector<QPoint> polygonPointVector;
    QPoint mapToPixmap(QPoint screenPoint);
    void setImageToTheCenter();

signals:
    void canvasUpdatedSignal();
    void scaleFactorChanged(float currentScaleFactor);


};

#endif // CANVAS_H
