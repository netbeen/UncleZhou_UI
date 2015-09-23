#ifndef SWATCH_H
#define SWATCH_H

#include <QWidget>
#include <QColor>

class Swatch : public QWidget
{
    Q_OBJECT
public:
    Swatch(QWidget *parent);

private:
    QColor currentColor;

signals:

};

#endif // SWATCH_H
