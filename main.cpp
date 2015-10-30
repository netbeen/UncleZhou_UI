#include "mainwindow.h"
#include <QApplication>

#include "util.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<std::vector<int>>("std::vector<int>");
    Util::init();

    MainWindow w;
    w.show();

    return a.exec();
}
