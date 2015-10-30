#ifndef READONLYCANVAS_H
#define READONLYCANVAS_H

#include <QWidget>

class ReadonlyCanvas : public QWidget
{
    Q_OBJECT
public:
    ReadonlyCanvas(QString filename, QWidget* parent);

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    QString filename;

signals:

};

#endif // READONLYCANVAS_H
