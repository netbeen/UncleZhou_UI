#include "layerdock.h"

LayerDock::LayerDock(QWidget* parent) : QDockWidget(parent)
{
    this->setMinimumSize(200, 200);
    this->setStyleSheet("background-color: #696969; padding: 0px;");
    this->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    this->setContentsMargins(0, 0, 0, 0);

}

