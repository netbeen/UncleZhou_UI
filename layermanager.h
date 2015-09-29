#ifndef LAYERMANAGER_H
#define LAYERMANAGER_H

#include <QObject>

#include <vector>

#include "util.h"
#include "layeritem.h"


/**
 * @brief The LayerManager class
 * @brief 图层管理器：负责保存当前编辑位置、当前所有图层状态。
 */
class LayerManager : public QObject
{
    Q_OBJECT
public:
    static LayerManager* getInstance();
    void init(config::editPosition editPosition);

    std::vector<LayerItem*> layerItemVector;

    void setDisplayLayerIndex(int index);
    int getDisplayLayerIndex();
    LayerItem* getDisplayLayerItem();


private:
    explicit LayerManager(QObject *parent = 0);

    static LayerManager* ptr2LayerManager;

    int currentDisplayLayerIndex;

signals:
    void displayLayerChanged(int index);

};

#endif // LAYERMANAGER_H
