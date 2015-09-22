#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QLabel>
#include <QFrame>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QLabel* sourceImageLabel;
    QLabel* sourceGuidanceLabel;
    QLabel* targetGuidanceLabel;

    QFrame* sourceImageFrame;
    QFrame* sourceGuidanceFrame;
    QFrame* targetGuidanceFrame;

    QImage* sourceImage;
    QImage* sourceGuidance;
    QImage* targetGuidance;

    QAction* loadSourceImageAction;

    // 初始化函数
    void initWindowLayout();
    void initAction();

    // action入口函数
    void loadSourceImage();
};

#endif // MAINWINDOW_H
