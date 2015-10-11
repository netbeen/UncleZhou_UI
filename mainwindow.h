#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QLabel>
#include <QFrame>
#include <QStackedLayout>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QWidget* sourceImageWidget;
    QWidget* sourceGuidanceWidget;
    QWidget* targetGuidanceWidget;
    QWidget* targetImageWidget;

    QFrame* sourceImageFrame;
    QFrame* sourceGuidanceFrame;
    QFrame* targetGuidanceFrame;
    QFrame* targetImageFrame;

    QImage* sourceImage;
    QImage* sourceGuidance;
    QImage* targetGuidance;
    QImage* targetImage;

    QStackedLayout* sourceImageWidgetStackedLayout;
    QStackedLayout* sourceGuidanceWidgetStackedLayout;
    QStackedLayout* targetGuidanceWidgetStackedLayout;
    QStackedLayout* targetImageWidgetStackedLayout;

    QAction* loadSourceImageAction;
    QAction* viewSourceImageAction;

    QAction* editSourceGuidanceAction;

    QAction* viewTargetGuidanceAction;
    QAction* newImageAction;
    QAction* editTargetGuidanceAction;

    QAction* aboutAction;

    QMenu* fileMenu;
    QMenu* helpMenu;

    // 初始化函数
    void initWindowLayout();
    void initAction();

    // action入口函数
    void loadSourceImage();
    void viewSourceImage();

    void editSourceGuidance();

    void viewTargetGuidance();
    void newImage();
    void editTargetGuidance();

    void about();
};

#endif // MAINWINDOW_H
