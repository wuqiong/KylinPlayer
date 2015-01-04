#include "playermainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PlayerMainWindow w;
    w.show();
    return a.exec();
}
