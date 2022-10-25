#include "mainwindow.h"
#include "configPage.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setFixedSize(600,400);
    w.setWindowFlags(Qt::WindowMaximizeButtonHint); //neutralize the maximize botton, however do not work
    w.show();
    return a.exec();
}

