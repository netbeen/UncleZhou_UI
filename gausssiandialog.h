#ifndef GAUSSSIANDIALOG_H
#define GAUSSSIANDIALOG_H

#include <QDialog>
#include <QColor>
#include <gaussianfunctionpainter.h>
#include <QImage>
#include <QDial>

class GausssianDialog : public QDialog
{
    Q_OBJECT
public:
    GausssianDialog(int xCenterCoordinate, int yCenterCoordinate, QColor color, QImage& image, QWidget* parent);

private:
    void initDialogLayout();

    int imageWidth;
    int imageHeight;

    int xCenterCoordinate;
    int yCenterCoordinate;
    QColor color;

    GaussianFunctionPainter* gaussianFunctionPainter;

    QImage* ptr2image;

    void sigmaXChangedSlot(int newSiamaX);
    void sigmaYChangedSlot(int newSiamaY);
    void isInverseSelectedSlot(bool newBool);

    QDial* xDial;
    QDial* yDial;

signals:
    void updateCanvas();
};

#endif // GAUSSSIANDIALOG_H
