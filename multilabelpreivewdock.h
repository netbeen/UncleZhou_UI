#ifndef MULTILABELPREIVEWDOCK_H
#define MULTILABELPREIVEWDOCK_H

#include <QDockWidget>
#include "readonlycanvas.h"

class MultiLabelPreivewDock : public QDockWidget
{
public:
    MultiLabelPreivewDock(QWidget* parent);
    ReadonlyCanvas* multiLabelCanvas;
};

#endif // MULTILABELPREIVEWDOCK_H
