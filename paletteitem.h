#ifndef PALETTEITEM_H
#define PALETTEITEM_H

#include <QWidget>

class PaletteItem : public QWidget
{
public:
    PaletteItem(QColor inputColor,QWidget* parent);

protected:
    virtual void paintEvent(QPaintEvent* event) override;

private:
    const QColor color;

signals:

};

#endif // PALETTEITEM_H
