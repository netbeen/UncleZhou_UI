#ifndef LAYERMANAGER_H
#define LAYERMANAGER_H

#include <QObject>

#include <vector>

#include "util.h"
#include "layeritem.h"

class LayerManager : public QObject
{
    Q_OBJECT
public:
    static LayerManager* getInstance();
    void init(config::editPosition editPosition);

    std::vector<LayerItem*> layerItemVector;

private:
    explicit LayerManager(QObject *parent = 0);

    static LayerManager* ptr2LayerManager;

signals:

};

#endif // LAYERMANAGER_H
