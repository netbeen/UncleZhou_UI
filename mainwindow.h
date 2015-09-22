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

    QFrame* sourceImageFrame;
    QFrame* sourceGuidanceFrame;

    QImage* sourceImage;
    QImage* sourceGuidance;

    void loadSourceImage();
};

#endif // MAINWINDOW_H
