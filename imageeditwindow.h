#ifndef IMAGEEDITWINDOW_H
#define IMAGEEDITWINDOW_H

#include <QToolBar>
#include <QMainWindow>
#include <QAction>
#include <QScrollArea>
#include <QStatusBar>
#include <QMenuBar>


#include <vector>

#include "util.h"
#include "swatch.h"
#include "layerdock.h"
#include "canvas.h"
#include "layermanager.h"
#include "navigatordock.h"
#include "tooloptiondock.h"
#include "tooloptionframe.h"

class ImageEditWindow  : public QMainWindow
{
    Q_OBJECT
public:
    ImageEditWindow(config::editPosition editPosition, config::editLevel editLevel, QWidget* parent);

private:
    QToolBar* toolbar;

    std::vector<QAction*> toolActionVector;


    //初始化函数
    void initWindowLayout(config::editLevel editLevel);
    void initActions(config::editLevel editLevel);

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

    ToolOptionFrame* moveToolOptionFrame;   //以下是每个工具的frame选项，用于被选中的时候发送至ToolOptionDock
    ToolOptionFrame* pencilToolOptionFrame;
    ToolOptionFrame* eraserToolOptionFrame;
    ToolOptionFrame* selectionToolOptionFrame;
    ToolOptionFrame* bucketToolOptionFrame;
    ToolOptionFrame* zoomInToolOptionFrame;
    ToolOptionFrame* zoomOutToolOptionFrame;

    Swatch* foreSwatch;
    Swatch* backSwatch;

    LayerDock* layerDock;
    NavigatorDock* navigatorDock;
    ToolOptionDock* toolOptionDock;
    Canvas* canvas;
    QScrollArea* scrollArea;

    //后台控制变量
    LayerManager* layerManager;


signals:
    void sendFrameToToolOptionDock(QFrame* inputFrame);

};

#endif // IMAGEEDITWINDOW_H
