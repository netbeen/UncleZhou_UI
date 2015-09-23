#include "util.h"

std::vector<QString> Util::guidanceChannel = std::vector<QString>();

void Util::init(){
    Util::guidanceChannel.push_back("LabelChannel");
    Util::guidanceChannel.push_back("FeatureChannel");
    Util::guidanceChannel.push_back("GradientChannel");
}
