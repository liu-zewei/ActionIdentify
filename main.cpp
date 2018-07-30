#include "mainappwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainAppWidget w;
    w.show();

    return a.exec();
}
