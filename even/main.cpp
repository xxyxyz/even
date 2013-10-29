#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName("xxyxyz");
    a.setApplicationName("even");

    MainWindow *w = new MainWindow();
    w->show();

    return a.exec();
}
