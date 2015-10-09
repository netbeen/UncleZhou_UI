#include "tooloptionframe.h"

ToolOptionFrame::ToolOptionFrame(QWidget* parent) : QFrame(parent)
{
    this->mainLayout = new QHBoxLayout(this);

    this->titleLabel = new QLabel("Null",this);
    this->titleLabel->setAlignment(Qt::AlignHCenter);
    this->titleLabel->setStyleSheet("font-size:30px;");
    this->mainLayout->addWidget(this->titleLabel);

    this->setLayout(this->mainLayout);
    this->hide();
}

