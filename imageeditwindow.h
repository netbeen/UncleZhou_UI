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
#include "multilabelpreivewdock.h"
#include "gaussianfunctionpainter.h"

#include "SuperPixel/readsuperpixeldat.h"
#include <QKeyEvent>
#include "undostack.h"

class ImageEditWindow  : public QMainWindow
{
    Q_OBJECT
public:
    ImageEditWindow(config::editPosition editPosition, config::editLevel editLevel, QWidget* parent);
    MultiLabelPreivewDock* getMultiLabelPreivewDock();


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
    void brokenLineToolSlot();
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
    QAction* brokenLineAction;    //折线工具
    QAction* bucketAction;  //颜料桶工具
    QAction* zoomInAction;  //放大工具
    QAction* zoomOutAction;     //缩小工具

    ToolOptionFrame* noneToolOptionFrame;
    ToolOptionFrame* moveToolOptionFrame;   //以下是每个工具的frame选项，用于被选中的时候发送至ToolOptionDock
    ToolOptionFrame* pencilToolOptionFrame;
    ToolOptionFrame* eraserToolOptionFrame;
    ToolOptionFrame* magicEraserToolOptionFrame;
    ToolOptionFrame* brokenLineToolOptionFrame;
    ToolOptionFrame* polygonToolOptionFrame;
    ToolOptionFrame* bucketToolOptionFrame;
    ToolOptionFrame* zoomToolOptionFrame;
    ToolOptionFrame* drawVectorFieldFrame;

    Swatch* foreSwatch;
    Swatch* backSwatch;

    LayerDock* layerDock;
    NavigatorDock* navigatorDock;
    ToolOptionDock* toolOptionDock;
    PaletteDock* paletteDock;
    Canvas* canvas;
    QScrollArea* scrollArea;
    MultiLabelPreivewDock* multiLabelPreivewDock;


    QLabel* magnificationValueLabel;    //

    //后台控制变量
    LayerManager* layerManager;

    QMenu* testFunctionMenu;

    QAction* densityPeakInteractiveAction;  //打开dp算法的窗口的action以及slot
    void densityPeakInteractiveSlot();

    QAction* viewPatchDistributeAction;     //查看patch分布的窗口的action以及slot
    void viewPatchDistributeSlot();

    void setClassificationColor(cv::Vec3b newColor);
    cv::Vec3b classificationColor;      //当前的分类色
    bool isClassificationColorValid;
    bool isMultiLabelChecked;
    void setIsMultiLabelChecked(bool flag);

    QAction* classificationWithoutPopupAction;     //二分（或者多类）分类算法的action和slot
    void classificationWithoutPopupSlot();

    QAction* multiLabelClassificationAction;     //多类分类算法的action和slot
    void multiLabelClassificationSlot();


    ReadSuperPixelDat* readSuperPixelDat;
    void doMultiLabelClassificationAndSave(const cv::Mat inputImage);

    UndoStack* undoStack;

    QAction* undoAction;
    void undoSlot();

    QAction* drawGaussianAction;
    void drawGaussianSlot();    //画出高斯模糊图像
    void drawGaussianSlotPart2(int xCoordinate, int yCoordinate);
    void updateCanvasSlot();

    QAction* drawVectorFieldAction;
    void drawVectorFieldSlot();
    void calcVectorFieldSlot();     //接收到了计算vector field的信号后，进行计算。

protected:
    virtual void keyPressEvent(QKeyEvent *e) override;

signals:
    void sendFrameToToolOptionDock(QFrame* inputFrame);

};

#endif // IMAGEEDITWINDOW_H
