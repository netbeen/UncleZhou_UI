#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QImage>
#include <QPixmap>

#include "layermanager.h"
#include "util.h"

class Canvas : public QWidget
{
    Q_OBJECT
public:
    Canvas(QWidget* parent);
    void init(config::editPosition editPosition);

protected:
    virtual void paintEvent(QPaintEvent* e);

private:
    QImage surfaceImage;
    QPixmap surfacePixmap;

    LayerManager* layerManager;

    void receiveDisplayLayerChanged(int index);

signals:


};

#endif // CANVAS_H
