#ifndef NAVIGATORDOCK_H
#define NAVIGATORDOCK_H

#include <QDockWidget>

#include "layermanager.h"
#include "navigatorcanvas.h"

class NavigatorDock : public QDockWidget
{
public:
    NavigatorDock(QWidget* parent);
    void navigatorUpdate();

private:
    LayerManager* layerManager;
    NavigatorCanvas* navigatorCanvas;

signals:

};

#endif // NAVIGATORDOCK_H
