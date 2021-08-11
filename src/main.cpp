#include "qitoolswindow.h"
#include <QApplication>

 int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setvbuf(stdout, nullptr, _IONBF, 1024);
    QSettings ini(qApp->applicationDirPath() + "/QiTools.ini", QSettings::IniFormat);
//    qSetMessagePattern("%{message} [%{file}:%{line} - %{qthreadptr} | %{time MMdd-h:mm:ss.zzz}]");
    qSetMessagePattern("%{message} [%{function}()=>%{line} - %{threadid} | %{time MMdd-h:mm:ss.zzz}]");
    auto _ba = ini.value("Preference/geometry").toByteArray();
    int styleMode = ini.value("Preference/styleMode").toInt();
    qApp->setStyleSheet(0 == styleMode ? "file:///:/QiTools.css" : "file:///./QiTools.css");
    QiToolsWindow w;
    if (!_ba.isEmpty())
        w.restoreGeometry(_ba);
    else
        w.resize(1280, 720);
    w.show();

    return a.exec();
}
