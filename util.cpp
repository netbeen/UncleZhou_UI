#include "util.h"

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
