#include "mainwindow.h"


#include <QApplication>
#include <stdlib.h>
#include <QDebug>
#include <QStyleFactory>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("Fusion"));

    qDebug() << system("/home/bigleb32/diplomprog/lsnrctl_status.sh");


    MainWindow w;



    w.showMaximized();

    return a.exec();
}
