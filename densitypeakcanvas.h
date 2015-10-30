#ifndef DENSITYPEAKCANVAS_H
#define DENSITYPEAKCANVAS_H

#include <QWidget>
#include "util.h"

class DensityPeakCanvas: public QWidget
{
    Q_OBJECT
public:
    DensityPeakCanvas(QWidget* parent);
    void init(const std::vector<ClusteringPoints>& v_points);

protected:
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void mousePressEvent(QMouseEvent *e) override;
    virtual void mouseMoveEvent(QMouseEvent *e) override;
    virtual void mouseReleaseEvent(QMouseEvent *e) override;


private:
    std::vector<ClusteringPoints> v_points;
    int densityMax;
    double deltaMax;
    bool inited;
    bool selectionValid;
    std::vector<int> selectPointIndex;

    QPoint selectionTopleft;
    QPoint selectionButtomDown;

    int maxDensityIndex;

signals:
    void selectCompetedSignal(std::vector<int> selectPointIndex);

};

#endif // DENSITYPEAKCANVAS_H
