#include "mainwindow.h"
#include "clientmanage.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();
    ClientManage w;
    w.run();
    return a.exec();
}
