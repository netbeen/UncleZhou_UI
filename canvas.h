#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QImage>
#include <QPixmap>

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



signals:


};

#endif // CANVAS_H
