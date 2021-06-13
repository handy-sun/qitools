#include "qitoolswindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qSetMessagePattern("%{message} [%{file}:%{line} - %{qthreadptr} | %{time MMdd-h:mm:ss.zzz}]");
    QiToolsWindow w;
    w.resize(960, 600);
    w.show();

    return a.exec();
}
