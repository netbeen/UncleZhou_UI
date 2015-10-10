#include "tooloptionframe.h"

/**
 * @brief ToolOptionFrame::ToolOptionFrame
 * @brief 工具选项卡的构造函数
 * @param title 表示选项卡的标题
 * @param parent 选项卡的父对象
 * @return 没有返回值
 */
ToolOptionFrame::ToolOptionFrame(QString title,QWidget* parent) : QFrame(parent)
{
    this->mainLayout = new QGridLayout(this);

    this->titleLabel = new QLabel(title,this);
    this->titleLabel->setStyleSheet("font-size:30px;");

    this->mainLayout->addWidget(this->titleLabel,0,0,1,2,Qt::AlignHCenter);

    this->setLayout(this->mainLayout);
    this->hide();
}

