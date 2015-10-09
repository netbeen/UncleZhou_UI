#ifndef TOOLOPTIONDOCK_H
#define TOOLOPTIONDOCK_H

#include <QDockWidget>
#include <QFrame>

class ToolOptionDock : public QDockWidget
{
public:
    ToolOptionDock(QWidget* parent);
    void setFrame(QFrame* inputFrame);

private:
    QFrame* currentDisplayFrame;

signals:

};

#endif // TOOLOPTIONDOCK_H
