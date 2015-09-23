#ifndef UTIL_H
#define UTIL_H
#include <QObject>
#include <QString>
#include <vector>

namespace config{
    enum editPosition{sourceImage, sourceGuidance,targetImage, targetGuidance};
    enum editLevel{readOnly, editable};
}

class Util
{
public:

    static void init();

    static std::vector<QString> guidanceChannel;



};

#endif // UTIL_H
