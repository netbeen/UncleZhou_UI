#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QLabel>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QImage* sourceImage;
    QLabel* sourceImageLabel;

    void loadSourceImage();
};

#endif // MAINWINDOW_H
