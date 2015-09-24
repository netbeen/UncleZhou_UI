#ifndef IMAGEEDITWINDOW_H
#define IMAGEEDITWINDOW_H

#include <QToolBar>
#include <QMainWindow>
#include <QAction>
#include <vector>

#include "util.h"
#include "swatch.h"
#include "layerdock.h"

class ImageEditWindow  : public QMainWindow
{
    Q_OBJECT
public:
    ImageEditWindow(config::editPosition editPosition, config::editLevel editLevel, QWidget* parent);

private:
    QToolBar* toolbar;

    std::vector<QAction*> toolActionVector;


    //初始化函数
    void initWindowLayout();
    void initActions();

    // slots函数
    void moveToolSlot();
    void pencilToolSlot();
    void eraserToolSlot();
    void selectionToolSlot();
    void bucketToolSlot();
    void zoomInToolSlot();
    void zoomOutToolSlot();


    //Actions
    QAction* moveAction;    //移动工具
    QAction* pencilAction;  //铅笔工具
    QAction* eraserAction;  //橡皮工具
    QAction* selectionAction;   //选择工具
    QAction* bucketAction;  //颜料桶工具
    QAction* zoomInAction;  //放大工具
    QAction* zoomOutAction;     //缩小工具

    Swatch* foreSwatch;
    Swatch* backSwatch;

    LayerDock* layerDock;


signals:

};

#endif // IMAGEEDITWINDOW_H
