#include "qitoolswindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QiToolsWindow w;
    w.show();

    return a.exec();
}
