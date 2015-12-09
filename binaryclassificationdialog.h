#ifndef BINARYCLASSIFICATIONDIALOG_H
#define BINARYCLASSIFICATIONDIALOG_H

#include <QDialog>
#include <layermanager.h>
#include <QRadioButton>

class BinaryClassificationDialog : public QDialog
{
    Q_OBJECT
public:
    BinaryClassificationDialog(QWidget* parent);

private:
    void initDialogLayout();
    void findTheToggledButton();
    LayerManager* layerManager;

    std::vector<cv::Vec3b> colorVector;
    std::vector<QRadioButton*> radioButtonVector;

signals:
    void sendColor(cv::Vec3b color);
};

#endif // BINARYCLASSIFICATIONDIALOG_H
