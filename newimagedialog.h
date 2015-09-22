#ifndef NEWIMAGEDIALOG_H
#define NEWIMAGEDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QComboBox>
#include <QDialogButtonBox>

class NewImageDialog : public QDialog
{
    Q_OBJECT
public:
    NewImageDialog(QDialog* parent=0);

private:
    QComboBox* presetComboBox;
    QLineEdit* widthEdit;
    QLineEdit* heightEdit;
    QComboBox* channelComboBox;
    QDialogButtonBox* dialogButtonBox;

    // 初始化布局
    void initDialogLayout();

    // slot函数
    void presetComboBoxChangedSlot(const int index);
    void accept();


signals:

};

#endif // NEWIMAGEDIALOG_H
