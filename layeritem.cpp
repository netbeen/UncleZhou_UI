#include "layeritem.h"

LayerItem::LayerItem(QString layerName, QImage image, QObject *parent) : QObject(parent), layerName(layerName), image(image)
{
    this->isShow = false;
}

