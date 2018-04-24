#include "mainwindow.h"
#include "Game/robocoprmgame.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();
    robocoprmgame *g = new robocoprmgame(NULL, false);
    g->show();

    return a.exec();
}
