#include "qitoolswindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QiToolsWindow w;
    w.resize(960, 600);
    w.show();

    return a.exec();
}
