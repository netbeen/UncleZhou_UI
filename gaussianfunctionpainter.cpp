#include "gaussianfunctionpainter.h"
#include <cmath>
#include <QColor>
#include <iostream>

GaussianFunctionPainter::GaussianFunctionPainter(const int x0, const int y0, const float sigmaX, const float sigmaY):x0(x0),y0(y0),sigmaX(sigmaX),sigmaY(sigmaY),isInverse(false){

}

void GaussianFunctionPainter::setSigma(const float newSigmaX, const float newSigmaY){
    this->sigmaX = newSigmaX;
    this->sigmaY = newSigmaY;
}

/**
 * @brief GaussianFunctionPainter::draw
 * @brief 绘制高斯模糊图像
 * @param referenceImage 输入输出图像
 * @return 没有返回值
 */
void GaussianFunctionPainter::draw(QImage& referenceImage, const QColor presetColor){
    const float e = 2.718281828459;
    for(int y_offset = 0; y_offset < referenceImage.height(); y_offset++){
        for(int x_offset = 0; x_offset < referenceImage.width(); x_offset++){
            const float exponent = -(std::pow(x_offset-x0,2)/(2*std::pow(sigmaX,2))+ std::pow(y_offset-y0,2)/(2*std::pow(sigmaY,2)));
            const float value = std::pow(e,exponent);
            QColor color;
            if(this->isInverse == false){
                color = QColor(255*(1-value)+presetColor.red()*value,255*(1-value)+presetColor.green()*value,255*(1-value)+presetColor.blue()*value);
            }else{
                color = QColor(255*(value)+presetColor.red()*(1-value),255*(value)+presetColor.green()*(1-value),255*(value)+presetColor.blue()*(1-value));
            }
            referenceImage.setPixel(x_offset,y_offset,color.rgb());
        }
    }
}

void GaussianFunctionPainter::setInverse(bool newBool){
    this->isInverse = newBool;
}
