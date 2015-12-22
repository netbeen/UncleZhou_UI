#include "mainwindow.h"
#include <QApplication>
#include "Classification/MyClassification.h"
#include "util.h"
#include "gaussianfunctionpainter.h"

#include "graphcut/graphcut.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<std::vector<int>>("std::vector<int>");
    Util::init();

    /*GaussianFunctionPainter gauss(250,250,150,150);
    QImage image = QImage(500,500,QImage::Format_RGB888);
    image.fill(QColor(255,255,255));
    gauss.draw(image);
    image.save("test.png");
    return 0;*/

    MainWindow w;
    w.show();

    return a.exec();
}
