#include "qitoolswindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(qitools);

    QApplication a(argc, argv);
    QiToolsWindow w;
    w.resize(960, 600);
    w.show();

    return a.exec();
}
