#ifndef MULTILABELPREIVEWDOCK_H
#define MULTILABELPREIVEWDOCK_H

#include <QDockWidget>
#include "readonlycanvas.h"
#include <QHBoxLayout>
#include <opencv2/opencv.hpp>
#include <QPushButton>
#include <layermanager.h>

class MultiLabelPreivewDock : public QDockWidget
{
    Q_OBJECT
public:
    MultiLabelPreivewDock(QWidget* parent);
    ReadonlyCanvas* multiLabelCanvas;
    void receiveUpdateColorLayoutSlot();

private:
    QHBoxLayout* colorButtonLayout; //每当画布中增加一种颜色，就在这个layout中增加对应颜色的一个按钮
    std::vector<cv::Vec3b> colorVector;
    std::vector<QPushButton*> colorButtonVector;
    LayerManager* layerManager;

    void initDockLayout();
    void updateColorButtonLayout();
    void buttonClickedSlot();
    void multiLabelCheckedSlot(bool flag);

    bool isMultiLabelChecked;
    int activeButtonIndex;

signals:
    void sendClassificationColor(cv::Vec3b newColor);
    void multiLabelCheckedSinal(bool flag);

};

#endif // MULTILABELPREIVEWDOCK_H
