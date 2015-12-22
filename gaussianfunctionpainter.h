#ifndef GAUSSIANFUNCTIONPAINTER_H
#define GAUSSIANFUNCTIONPAINTER_H

#include <QImage>

class GaussianFunctionPainter
{
public:
    GaussianFunctionPainter(const int x0, const int y0, const float sigmaX, const float sigmaY);
    void draw(QImage& referenceImage, const QColor presetColor);
    void setSigma(const float sigmaX, const float sigmaY);
    void setInverse(bool newBool);

private:
    int x0;
    int y0;
    float sigmaX;
    float sigmaY;

    bool isInverse;
};

#endif // GAUSSIANFUNCTIONPAINTER_H
