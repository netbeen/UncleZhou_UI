#include "navigatordock.h"

#include <QVBoxLayout>


NavigatorDock::NavigatorDock(QWidget* parent) : QDockWidget(parent)
{
    this->setMinimumSize(200, 200);
    this->setStyleSheet("background-color: #696969; padding: 0px;");
    this->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    this->setContentsMargins(0, 0, 0, 0);

    this->layerManager = LayerManager::getInstance();


    QWidget* mainWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(mainWidget);

    this->navigatorCanvas = new NavigatorCanvas(this);
    layout->addWidget(navigatorCanvas);

    mainWidget->setLayout(layout);
    this->setWidget(mainWidget);

}

void NavigatorDock::navigatorUpdate(){
    this->navigatorCanvas->update();
}
