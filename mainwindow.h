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
    QImage* sourceImage;
    QLabel* sourceImageLabel;
    QFrame* sourceImageFrame;

    void loadSourceImage();
};

#endif // MAINWINDOW_H
