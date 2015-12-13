#include "palettedock.h"
#include <QGridLayout>
#include "paletteitem.h"
#include <vector>

/**
 * @brief PaletteDock::PaletteDock
 * @brief 调色板Dock构造函数
 * @param parent 父对象
 * @return 没有返回值
 */
PaletteDock::PaletteDock(QWidget* parent) : QDockWidget(parent)
{
    this->setMinimumSize(300, 100);
    this->setStyleSheet("background-color: #696969; padding: 0px;");
    this->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    this->setContentsMargins(0, 0, 0, 0);

    QWidget* mainWidget = new QWidget(this);
    QGridLayout* gridLayout = new QGridLayout(mainWidget);

    std::vector<int> enumColor = std::vector<int>{0,127,255};

    for(int i = 0; i < 3 ; i++){
        for(int j = 0; j < 3; j ++){
            for(int k = 0; k < 3; k++){
                PaletteItem* paletteItem = new PaletteItem(QColor(enumColor.at(i),enumColor.at(j),enumColor.at(k)),this);
                QObject::connect(paletteItem, &PaletteItem::colorSelected, this, &PaletteDock::colorSelected);
                gridLayout->addWidget(paletteItem,i,j*3+k);
            }
        }
    }

    mainWidget->setLayout(gridLayout);
    this->setWidget(mainWidget);
}

