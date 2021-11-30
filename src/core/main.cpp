﻿#include <QApplication>
#include <QSettings>
#include <QDebug>
#include "qitoolswindow.h"

using namespace Core;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#if QT_VERSION > 0x050900
    a.setApplicationVersion(VERSION_STRING);
#endif
    setvbuf(stdout, nullptr, _IONBF, 1024);

    QSettings ini(qApp->applicationDirPath() + "/QiTools.ini", QSettings::IniFormat);
//    qSetMessagePattern("%{message} [%{file}:%{line} - %{qthreadptr} | %{time MMdd-h:mm:ss.zzz}]");
    qSetMessagePattern("%{message} [%{function}()=>%{line} - %{threadid} | %{time MMdd-h:mm:ss.zzz}]");
    auto _ba = ini.value("Preference/geometry").toByteArray();
    int styleMode = ini.value("Preference/styleMode").toInt();
    qApp->setStyleSheet(0 == styleMode ? "file:///:/QiTools.css" : "file:///./QiTools.css");

    //一定要在界面显示之前载入翻译文件
    auto appDir = QDir(qApp->applicationDirPath());
    const auto entryList = appDir.entryList(QDir::Files);
    for (const QString &fileName : entryList)
    {
        if (!fileName.contains(".qm"))
            continue;

        QTranslator *translator = new QTranslator(&a);
        if (translator->load(fileName))
        {
            qApp->installTranslator(translator);
            qDebug() << fileName;
        }
    }

    QiToolsWindow w;
    w.setWindowTitle("qitools");
    w.setWindowIcon(QIcon(":/toolsimage.svg"));
    if (!_ba.isEmpty())
        w.restoreGeometry(_ba);
    else
        w.resize(1280, 720);

    w.show();

    int ret = a.exec();
    return ret;
}
