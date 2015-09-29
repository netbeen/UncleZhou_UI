#include "util.h"

#include <cmath>

/**
 * @brief Util::guidanceChannel
 * @brief 静态的vector，用于保存引导图片的类型
 */
std::vector<QString> Util::guidanceChannel = std::vector<QString>();


/**
 * @brief Util::init
 * @brief 为util类进行初始化（初始化guidanceChannel这个vector）
 * @param 没有参数
 * @return 没有返回值
 */
void Util::init(){
    Util::guidanceChannel.push_back("LabelChannel");
    Util::guidanceChannel.push_back("FeatureChannel");
    Util::guidanceChannel.push_back("GradientChannel");
}

float Util::calcL2Distance(const QPoint point1, const QPoint point2){
    return std::sqrt(std::pow(point1.x() - point2.x(),2)+std::pow(point1.y() - point2.y(),2));
}
