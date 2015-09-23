#include "mainwindow.h"
#include <QApplication>

#include "util.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Util::init();

    MainWindow w;
    w.show();

    return a.exec();
}
