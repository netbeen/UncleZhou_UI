#include "newimagedialog.h"
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QWidget>
#include <QImage>

#include "util.h"


/**
 * @brief NewImageDialog::NewImageDialog
 * @brief 新建图像对话框的构造函数
 * @param parent 作为qt对象树父指针
 * @return 没有返回值
 */
NewImageDialog::NewImageDialog(QDialog* parent) : QDialog(parent)
{
    this->initDialogLayout();

}



/**
 * @brief NewImageDialog::initDialogLayout
 * @brief 对于新建图像对话框的布局、控件的定义与设定。
 * @param 没有参数
 * @return 没有返回值
 */
void NewImageDialog::initDialogLayout(){
    this->setWindowTitle("Create new guidance");

    QGridLayout* gridLayout = new QGridLayout(this);
    this->setLayout(gridLayout);

    this->presetComboBox = new QComboBox(this);
    presetComboBox->addItem("64*64");
    presetComboBox->addItem("128*128");
    presetComboBox->addItem("256*256");
    QObject::connect(this->presetComboBox, static_cast<void (QComboBox::*) (int)>(&QComboBox::currentIndexChanged), this, &NewImageDialog::presetComboBoxChangedSlot );

    this->widthEdit = new QLineEdit(this);
    widthEdit->setText("64");

    this->heightEdit = new QLineEdit(this);
    heightEdit->setText("64");

    this->channelComboBox = new QComboBox(this);
    channelComboBox->addItem("RGB");

    this->dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    QObject::connect(this->dialogButtonBox, &QDialogButtonBox::accepted, this, &NewImageDialog::accept);
    QObject::connect(this->dialogButtonBox, &QDialogButtonBox::rejected, this, &NewImageDialog::reject);

    gridLayout->addWidget(new QLabel("Preset",this),0,0);
    gridLayout->addWidget(new QLabel("Width",this),1,0);
    gridLayout->addWidget(new QLabel("Height",this),2,0);
    gridLayout->addWidget(new QLabel("Channel",this),3,0);
    gridLayout->addWidget(this->presetComboBox,0,1);
    gridLayout->addWidget(this->widthEdit,1,1);
    gridLayout->addWidget(this->heightEdit,2,1);
    gridLayout->addWidget(this->channelComboBox,3,1);
    gridLayout->addWidget(this->dialogButtonBox,4,0,1,2);

}


/**
 * @brief NewImageDialog::presetComboBoxChangedSlot
 * @brief 当preset组合框被改动的时候，修正两个编辑内控件部的参数
 * @param index是组合框的选中项的索引号
 * @return 没有返回值
 */
void NewImageDialog::presetComboBoxChangedSlot(const int index){
    switch(index){
        case 0:
            this->widthEdit->setText("64");
            this->heightEdit->setText("64");
            break;
        case 1:
            this->widthEdit->setText("128");
            this->heightEdit->setText("128");
            break;
        case 2:
            this->widthEdit->setText("256");
            this->heightEdit->setText("256");
            break;
        default:
            break;
    }
}


/**
 * @brief NewImageDialog::accept
 * @brief 点击确认后的slot函数，新建1幅引导图片，然后保存成n份（n=引导图片的类型数量）
 * @param 没有参数
 * @return 没有返回值
 */
void NewImageDialog::accept(){
    QImage* newTargetGuidance = new QImage(this->widthEdit->text().toInt(), this->heightEdit->text().toInt(),QImage::Format_RGB888);
    newTargetGuidance->fill(QColor(255,255,255));
    for(QString elem : Util::guidanceChannel){
        newTargetGuidance->save("./targetGuidance"+ elem +".png");
    }
    QDialog::accept();
}
