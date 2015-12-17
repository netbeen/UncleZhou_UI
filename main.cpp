#include "mainwindow.h"
#include <QApplication>
#include "Classification/MyClassification.h"
#include "util.h"

#include "graphcut/graphcut.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<std::vector<int>>("std::vector<int>");
    Util::init();

    MainWindow w;
    w.show();

    return a.exec();
}
