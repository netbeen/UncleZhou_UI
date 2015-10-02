#ifndef NAVIGATORCANVAS_H
#define NAVIGATORCANVAS_H

#include <QWidget>
#include <layermanager.h>

class NavigatorCanvas : public QWidget
{
public:
    NavigatorCanvas(QWidget* parent);


protected:
    virtual void paintEvent(QPaintEvent* e) override;

private:
    LayerManager* layerManager;

signals:

};

#endif // NAVIGATORCANVAS_H
