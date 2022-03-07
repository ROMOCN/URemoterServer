#include <QCoreApplication>
#include <QDebug>
#include "mainwindow.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    MainWindow *w = new MainWindow();
    return a.exec();
}
