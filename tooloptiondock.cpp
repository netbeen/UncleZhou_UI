#include "tooloptiondock.h"

#include "QDebug"


ToolOptionDock::ToolOptionDock(QWidget* parent) : QDockWidget(parent)
{
     this->setMinimumSize(300, 200);
     this->setStyleSheet("background-color: #696969; padding: 0px;");
     this->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
     this->setContentsMargins(0, 0, 0, 0);

    this->currentDisplayFrame = new QFrame(this);
    this->setWidget(this->currentDisplayFrame);

}

void ToolOptionDock::setFrame(QFrame* inputFrame){
    this->currentDisplayFrame = inputFrame;
    this->setWidget(this->currentDisplayFrame);
}
