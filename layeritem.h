#ifndef LAYERITEM_H
#define LAYERITEM_H

#include <QObject>
#include <QString>
#include <QImage>

class LayerItem : public QObject
{
    Q_OBJECT
public:
    explicit LayerItem(QString layerName, QImage image, QObject *parent);

    const QString layerName;
    QImage image;

private:


signals:

};

#endif // LAYERITEM_H
