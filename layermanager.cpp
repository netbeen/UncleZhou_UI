#include "layermanager.h"

#include <QDebug>

LayerManager* LayerManager::ptr2LayerManager = nullptr;

LayerManager::LayerManager(QObject *parent) : QObject(parent)
{

}

void LayerManager::init(config::editPosition editPosition){
    this->layerItemVector.clear();
    switch (editPosition) {
        case config::sourceImage:
        {
            LayerItem* sourceImageLayer = new LayerItem("Source Image", QImage("./sourceImage.png"),this);
            this->layerItemVector.push_back(sourceImageLayer);
            break;
        }
        case config::sourceGuidance:
            for(QString elem : Util::guidanceChannel){
                LayerItem* layerItem = new LayerItem(elem, QImage("./sourceGuidance" + elem +".png"),this);
                this->layerItemVector.push_back(layerItem);
            }
            break;
        case config::targetGuidance:
            for(QString elem : Util::guidanceChannel){
                LayerItem* layerItem = new LayerItem(elem, QImage("./targetGuidance" + elem +".png"),this);
                this->layerItemVector.push_back(layerItem);
            }
            break;
        case config::targetImage:
        {
            LayerItem* targetImageLayer = new LayerItem("Target Image", QImage("./targetImage.png"),this);
            this->layerItemVector.push_back(targetImageLayer);
            break;
        }
    }
    qDebug() << "LayerManager::init compete, size of layerItemVector = " << this->layerItemVector.size();
}

LayerManager* LayerManager::getInstance(){
    if(LayerManager::ptr2LayerManager == nullptr){
        LayerManager::ptr2LayerManager = new LayerManager();
    }
    return ptr2LayerManager;
}
