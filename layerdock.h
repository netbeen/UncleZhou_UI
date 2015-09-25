#ifndef LAYERDOCK_H
#define LAYERDOCK_H

#include <QDockWidget>
#include <QRadioButton>

#include "layermanager.h"

class LayerDock : public QDockWidget
{
    Q_OBJECT
public:
    LayerDock(QWidget* parent);

private:
    LayerManager* layerManager;
    void findTheToggledButton();

    std::vector<QRadioButton*> radioButtonVector;       //用来追踪所有的按钮，并且选中第一个按钮

signals:
    void displayLayerChanged(int index);

};

#endif // LAYERDOCK_H
