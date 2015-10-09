#include "tooloptionframe.h"

ToolOptionFrame::ToolOptionFrame(QString title,QWidget* parent) : QFrame(parent)
{
    this->mainLayout = new QGridLayout(this);

    this->titleLabel = new QLabel(title,this);
    this->titleLabel->setStyleSheet("font-size:30px;");

    this->mainLayout->addWidget(this->titleLabel,0,0,1,2,Qt::AlignHCenter);

    this->setLayout(this->mainLayout);
    this->hide();
}

