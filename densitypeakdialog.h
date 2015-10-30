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

    float scaleFactor;
    cv::Mat sourceImageMat;

    ReadonlyCanvas* previewOriginImage;
    ReadonlyCanvas* previewGuidanceImage;
    DensityPeakCanvas* densityPeakCanvas;

signals:

};

#endif // DENSITYPEAKDIALOG_H
