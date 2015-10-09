#ifndef TOOLOPTIONFRAME_H
#define TOOLOPTIONFRAME_H

#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QLabel>

class ToolOptionFrame : public QFrame
{
public:
    ToolOptionFrame(QString title,QWidget* parent);

    QGridLayout* mainLayout;


private:
    QLabel* titleLabel;

signals:

};

#endif // TOOLOPTIONFRAME_H
