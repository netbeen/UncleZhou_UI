#ifndef UTIL_H
#define UTIL_H
#include <QObject>
#include <QString>
#include <vector>
#include <QPoint>

namespace config{
    enum editPosition{sourceImage, sourceGuidance,targetImage, targetGuidance};
    enum editLevel{readOnly, editable};
    enum operationType{None,Move, Pencil,  Eraser, ZoomIn, ZoomOut};
}

class Util
{
public:

    static void init();

    static std::vector<QString> guidanceChannel;

    static float calcL2Distance(const QPoint point1, const QPoint point2);



};

#endif // UTIL_H
