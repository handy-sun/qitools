#include "qitoolswindow.h"
#include <QApplication>

 int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setvbuf(stdout, nullptr, _IONBF, 1024);
//    qSetMessagePattern("%{message} [%{file}:%{line} - %{qthreadptr} | %{time MMdd-h:mm:ss.zzz}]");
    qSetMessagePattern("%{message} [%{function}()=>%{line} - %{threadid} | %{time MMdd-h:mm:ss.zzz}]");
    qApp->setStyleSheet("file:///:/QiTools.css");
    QiToolsWindow w;
    w.resize(960, 600);
    w.show();

    return a.exec();
}
