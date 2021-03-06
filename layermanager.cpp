#include "layermanager.h"

#include <QDebug>

LayerManager* LayerManager::ptr2LayerManager = nullptr;

LayerManager::LayerManager(QObject *parent) : QObject(parent),currentDisplayLayerIndex(-1)
{

}

void LayerManager::init(config::editPosition editPosition){
    this->layerItemVector.clear();
    switch (editPosition) {
        LayerItem* singleLayer;
        case config::sourceImage:
            singleLayer = new LayerItem("Source Image", QImage(Util::dirName+"/sourceImage.png"),this);
            this->layerItemVector.push_back(singleLayer);
            break;
        case config::sourceGuidance:
            for(QString elem : Util::guidanceChannel){
                LayerItem* layerItem = new LayerItem(elem, QImage(Util::dirName+"/sourceGuidance" + elem +".png"),this);
                this->layerItemVector.push_back(layerItem);
            }
            break;
        case config::targetGuidance:
            for(QString elem : Util::guidanceChannel){
                LayerItem* layerItem = new LayerItem(elem, QImage(Util::dirName+"/targetGuidance" + elem +".png"),this);
                this->layerItemVector.push_back(layerItem);
            }
            break;
        case config::targetImage:
            singleLayer = new LayerItem("Target Image", QImage(Util::dirName+"/targetImage.png"),this);
            this->layerItemVector.push_back(singleLayer);
            break;
    }
    this->currentDisplayLayerIndex = 0;
}

LayerManager* LayerManager::getInstance(){
    if(LayerManager::ptr2LayerManager == nullptr){
        LayerManager::ptr2LayerManager = new LayerManager();
    }
    return ptr2LayerManager;
}


void LayerManager::setDisplayLayerIndex(int index){
    this->currentDisplayLayerIndex = index;
    emit this->displayLayerChanged();
}

int LayerManager::getDisplayLayerIndex(){
    return this->currentDisplayLayerIndex;
}

LayerItem* LayerManager::getDisplayLayerItem(){
    return this->layerItemVector.at(this->currentDisplayLayerIndex);
}
