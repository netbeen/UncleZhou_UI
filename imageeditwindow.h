#ifndef IMAGEEDITWINDOW_H
#define IMAGEEDITWINDOW_H

#include <QToolBar>
#include <QMainWindow>

#include "util.h"

class ImageEditWindow  : public QMainWindow
{
    Q_OBJECT
public:
    ImageEditWindow(config::editPosition editPosition, config::editLevel editLevel, QWidget* parent);

private:
    QToolBar* toolbar;

    void initWindowLayout();

signals:

};

#endif // IMAGEEDITWINDOW_H
