#ifndef LAYERDOCK_H
#define LAYERDOCK_H

#include <QDockWidget>
#include <QRadioButton>
#include <QCheckBox>

#include "layermanager.h"

class LayerDock : public QDockWidget
{
    Q_OBJECT
public:
    LayerDock(QWidget* parent);
    QCheckBox* isShowBackgroundCheckBox;

private:
    LayerManager* layerManager;
    void findTheToggledButton();

    std::vector<QRadioButton*> radioButtonVector;       //用来追踪所有的按钮，并且选中第一个按钮


signals:


};

#endif // LAYERDOCK_H
