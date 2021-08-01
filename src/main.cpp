#include "qitoolswindow.h"
#include <QApplication>

 int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setbuf(stdout, nullptr);
    qSetMessagePattern("%{message} [%{file}:%{line} - %{qthreadptr} | %{time MMdd-h:mm:ss.zzz}]");
    qApp->setStyleSheet("file:///:/QiTools.css");
    QiToolsWindow w;
    w.resize(960, 600);
    w.show();

    return a.exec();
}
// https://www.python.org/ftp/python/3.8.6/python-3.8.6-amd64.exe
