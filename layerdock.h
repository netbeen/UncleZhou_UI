#ifndef LAYERDOCK_H
#define LAYERDOCK_H

#include <QDockWidget>

#include "layermanager.h"

class LayerDock : public QDockWidget
{
public:
    LayerDock(QWidget* parent);

private:
    LayerManager* layerManager;

signals:

};

#endif // LAYERDOCK_H
