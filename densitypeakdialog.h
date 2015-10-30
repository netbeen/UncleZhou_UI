#ifndef DENSITYPEAKDIALOG_H
#define DENSITYPEAKDIALOG_H

#include <QDialog>
#include <opencv2/opencv.hpp>

#include "readonlycanvas.h"
#include "util.h"
#include "densitypeakcanvas.h"

class DensityPeakDialog : public QDialog
{
    Q_OBJECT
public:
    DensityPeakDialog(QWidget* parent);

private:
    void initDialogLayout();
    void initColorTabel();

    float scaleFactor;
    cv::Mat sourceImageMat;

    ReadonlyCanvas* previewOriginImage;
    ReadonlyCanvas* previewGuidanceImage;
    DensityPeakCanvas* densityPeakCanvas;

    void receiveSelectionCompeted(std::vector<int> selectPointIndex);

    std::vector<ClusteringPoints> v_points;
    std::vector<std::pair<int, double> > v_density_Descend;
    std::vector<QColor> colorTabel;

signals:

};

#endif // DENSITYPEAKDIALOG_H
