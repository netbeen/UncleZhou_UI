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

    void initDialogLayout();

signals:

};

#endif // NEWIMAGEDIALOG_H
