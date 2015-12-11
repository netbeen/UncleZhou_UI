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
#include "palettedock.h"
#include "densitypeakdialog.h"
#include "viewpatchdistributedialog.h"

#include "SuperPixel/readsuperpixeldat.h"

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
    void noneToolSlot();
    void moveToolSlot();
    void pencilToolSlot();
    void eraserToolSlot();
    void magicEraserToolSlot();
    void polygonToolSlot();
    void bucketToolSlot();
    void zoomInToolSlot();
    void zoomOutToolSlot();

    void receiveScaleChanged(float inputScale);


    //Actions
    QAction* noneAction;
    QAction* moveAction;    //移动工具
    QAction* pencilAction;  //铅笔工具
    QAction* eraserAction;  //橡皮工具
    QAction* magicEraserAction;  //魔法橡皮工具
    QAction* polygonAction;   //多边形工具
    QAction* bucketAction;  //颜料桶工具
    QAction* zoomInAction;  //放大工具
    QAction* zoomOutAction;     //缩小工具

    ToolOptionFrame* noneToolOptionFrame;
    ToolOptionFrame* moveToolOptionFrame;   //以下是每个工具的frame选项，用于被选中的时候发送至ToolOptionDock
    ToolOptionFrame* pencilToolOptionFrame;
    ToolOptionFrame* eraserToolOptionFrame;
    ToolOptionFrame* magicEraserToolOptionFrame;
    ToolOptionFrame* polygonToolOptionFrame;
    ToolOptionFrame* bucketToolOptionFrame;
    ToolOptionFrame* zoomToolOptionFrame;

    Swatch* foreSwatch;
    Swatch* backSwatch;

    LayerDock* layerDock;
    NavigatorDock* navigatorDock;
    ToolOptionDock* toolOptionDock;
    PaletteDock* paletteDock;
    Canvas* canvas;
    QScrollArea* scrollArea;

    QLabel* magnificationValueLabel;    //

    //后台控制变量
    LayerManager* layerManager;

    QMenu* testFunctionMenu;

    QAction* densityPeakInteractiveAction;  //打开dp算法的窗口的action以及slot
    void densityPeakInteractiveSlot();

    QAction* viewPatchDistributeAction;     //查看patch分布的窗口的action以及slot
    void viewPatchDistributeSlot();

    QAction* binaryClassificationAction;     //二分分类算法的action和slot
    void binaryClassificationSlot();
    void getClassificationColor(cv::Vec3b newColor);
    cv::Vec3b classificationColor;


    ReadSuperPixelDat* readSuperPixelDat;

signals:
    void sendFrameToToolOptionDock(QFrame* inputFrame);

};

#endif // IMAGEEDITWINDOW_H
