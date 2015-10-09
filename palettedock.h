#ifndef PALETTEDOCK_H
#define PALETTEDOCK_H

#include <QDockWidget>

class PaletteDock : public QDockWidget
{
    Q_OBJECT
public:
    PaletteDock(QWidget* parent);

signals:
    void colorSelected(QColor inputColor);

};

#endif // PALETTEDOCK_H
