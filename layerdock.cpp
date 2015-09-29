#include "layerdock.h"
#include <assert.h>
#include <QString>
#include <QDebug>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QButtonGroup>

LayerDock::LayerDock(QWidget* parent) : QDockWidget(parent)
{
    this->setMinimumSize(200, 200);
    this->setStyleSheet("background-color: #696969; padding: 0px;");
    this->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    this->setContentsMargins(0, 0, 0, 0);

    this->layerManager = LayerManager::getInstance();
    assert(this->layerManager != nullptr);


    QWidget* mainWidget = new QWidget(this);
    QVBoxLayout* radioLayout = new QVBoxLayout(mainWidget);

    for(LayerItem* item : this->layerManager->layerItemVector){
        qDebug() << item->layerName ;
        QRadioButton* radio = new QRadioButton(item->layerName);
        QObject::connect(radio, &QRadioButton::clicked, this, &LayerDock::findTheToggledButton);
        radioLayout->addWidget(radio);
        this->radioButtonVector.push_back(radio);
    }

    QObject::connect(this, &LayerDock::displayLayerChanged, this->layerManager, &LayerManager::displayLayerChanged);

    radioButtonVector.front()->setChecked(true);
    if(radioButtonVector.size() == 1){
        radioButtonVector.front()->setEnabled(false);
    }
    this->findTheToggledButton();

    mainWidget->setLayout(radioLayout);
    this->setWidget(mainWidget);

}

void LayerDock::findTheToggledButton(){
    int index = 0;
    while(this->radioButtonVector.at(index)->isChecked() == false){
        //qDebug() << "radioButtonVector at " << index << " isChecked = false.";
        index++;
        if(index == radioButtonVector.size()){
            return;
        }
    }
    this->layerManager->setDisplayLayerIndex(index);
    emit this->displayLayerChanged(index);
    //qDebug() << "radioButtonVector at " << index << " isChecked = true.";

}
