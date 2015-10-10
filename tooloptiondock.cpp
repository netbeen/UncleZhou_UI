#include "tooloptiondock.h"

#include "QDebug"

/**
 * @brief ToolOptionDock::ToolOptionDock
 * @brief 工具选项卡Dock的构造函数
 * @param parent 表示Dock的父对象
 * @return 没有返回值
 */
ToolOptionDock::ToolOptionDock(QWidget* parent) : QDockWidget(parent)
{
     this->setMinimumSize(300, 200);
     this->setStyleSheet("background-color: #696969; padding: 0px;");
     this->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
     this->setContentsMargins(0, 0, 0, 0);

    this->currentDisplayFrame = new QFrame(this);
    this->setWidget(this->currentDisplayFrame);

}

/**
 * @brief ToolOptionDock::setFrame
 * @brief 接受新的选项卡，将其显示在Dock中
 * @param inputFrame 表示输入的选项卡Frame
 * @return 没有返回值
 */
void ToolOptionDock::setFrame(QFrame* inputFrame){
    this->currentDisplayFrame = inputFrame;
    this->setWidget(this->currentDisplayFrame);
}
