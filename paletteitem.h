#ifndef PALETTEITEM_H
#define PALETTEITEM_H

#include <QWidget>

class PaletteItem : public QWidget
{
    Q_OBJECT
public:
    PaletteItem(QColor inputColor,QWidget* parent);
    QColor getColor() const;

protected:
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

private:
    const QColor color;

signals:
    void colorSelected(QColor inputColor);

};

#endif // PALETTEITEM_H
