#include "mainwindow.h"
#include <QApplication>
#include "/home/netbeen/桌面/周叔项目/20151208周叔代码/MyBinaryClassification.h"
#include "util.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<std::vector<int>>("std::vector<int>");
    Util::init();

    MainWindow w;
    w.show();

    /*float imgscale = 1.0;
    int patchSize = 16;
    int numBins = 16;
    cv::Vec3b BK_Color(uchar(255), uchar(255), uchar(255)); //whilte
    cv::Vec3b Cur_Color(uchar(255), 0, 0); //blue

    // Read Source Image
    cv::Mat img = cv::imread("/home/netbeen/桌面/周叔项目/stone.png");

    // Read Mask
    cv::Mat mask = cv::imread("/home/netbeen/桌面/周叔项目/mask-color.png");

    // Run RFBinaryClassification
    cv::Mat outputImg;
    CMyBinaryClassification myTest;
    myTest.RandomForestBinaryClassification(img, mask, outputImg, BK_Color, Cur_Color);
    cv::imwrite("output.png",outputImg);
    return 0;*/

    return a.exec();
}
