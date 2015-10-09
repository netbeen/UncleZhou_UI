#ifndef TOOLOPTIONFRAME_H
#define TOOLOPTIONFRAME_H

#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>

class ToolOptionFrame : public QFrame
{
public:
    ToolOptionFrame(QWidget* parent);

    QHBoxLayout* mainLayout;
    QLabel* titleLabel;

signals:

};

#endif // TOOLOPTIONFRAME_H
