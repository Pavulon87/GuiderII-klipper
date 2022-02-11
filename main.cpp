#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    int currentExitCode = 0;

    do {
        QApplication a(argc, argv);
        MainWindow w;
        w.setWindowState(Qt::WindowFullScreen);
        w.show();
        currentExitCode = a.exec();
    } while( currentExitCode == 1337 );

    return currentExitCode;

}
