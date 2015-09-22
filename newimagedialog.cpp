#include "newimagedialog.h"
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QWidget>


/**
 * @brief NewImageDialog::NewImageDialog
 * @param parent
 */
NewImageDialog::NewImageDialog(QDialog* parent) : QDialog(parent)
{
    this->initDialogLayout();

}



/**
 * @brief NewImageDialog::initDialogLayout
 */
void NewImageDialog::initDialogLayout(){
    this->setWindowTitle("Create new guidance");

    QGridLayout* gridLayout = new QGridLayout(this);
    this->setLayout(gridLayout);

    QComboBox* presetComboBox = new QComboBox(this);
    presetComboBox->addItem("32*32");
    presetComboBox->addItem("64*64");
    presetComboBox->addItem("128*128");

    QLineEdit* widthEdit = new QLineEdit(this);
    widthEdit->setText("32");

    QLineEdit* heightEdit = new QLineEdit(this);
    heightEdit->setText("32");

    QComboBox* channelComboBox = new QComboBox(this);
    channelComboBox->addItem("RGB");

    QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    gridLayout->addWidget(new QLabel("Preset",this),0,0);
    gridLayout->addWidget(new QLabel("Width",this),1,0);
    gridLayout->addWidget(new QLabel("Height",this),2,0);
    gridLayout->addWidget(new QLabel("Channel",this),3,0);
    gridLayout->addWidget(presetComboBox,0,1);
    gridLayout->addWidget(widthEdit,1,1);
    gridLayout->addWidget(heightEdit,2,1);
    gridLayout->addWidget(channelComboBox,3,1);
    gridLayout->addWidget(dialogButtonBox,4,0,1,2);

}
