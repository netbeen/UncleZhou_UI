#include "multilabelpreivewdock.h"
#include <QVBoxLayout>

MultiLabelPreivewDock::MultiLabelPreivewDock(QWidget* parent):QDockWidget(parent){
    this->setMinimumSize(300, 100);
    this->setStyleSheet("background-color: #696969; padding: 0px;");
    this->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    this->setContentsMargins(0, 0, 0, 0);


    QWidget* mainWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(mainWidget);

    this->multiLabelCanvas = new ReadonlyCanvas("multiLabelClassificationResult.png",this);
    layout->addWidget(this->multiLabelCanvas);

    mainWidget->setLayout(layout);
    this->setWidget(mainWidget);
}

