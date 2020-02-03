
#include "spikingnetwork.cpp"
#include "simulation.cpp"

#include "mainwindow.h"
#include <QApplication>
#include<unistd.h>
#include "dialog.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w(argv[1]);
    w.show();
    return a.exec();
}

