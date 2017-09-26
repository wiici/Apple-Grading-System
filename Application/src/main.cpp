#include "include/MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication App(argc, argv);
    MainWindow Window("Application");
    Window.show();

    return App.exec();
}
