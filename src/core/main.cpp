#include "stable.h"
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
//    qSetMessagePattern("%{message} [%{file}:%{line} - %{qthreadptr} | %{time MM.dd hh:mm:ss.zzz}]");
    qSetMessagePattern("%{message} [%{function}()=>%{line} - %{threadid} | %{time MM/dd hh:mm:ss.zzz}]");
    auto _ba = ini.value("Preference/geometry").toByteArray();
    int styleMode = ini.value("Preference/styleMode").toInt();
    qApp->setStyleSheet(0 == styleMode ? "file:///:/QiTools.css" : "file:///./QiTools.css");

    QString trdir("translations");
    auto appDir = QDir(qApp->applicationDirPath() + "/" + trdir);
    const auto entryList = appDir.entryList(QDir::Files);
    for (const QString &fileName : entryList)
    {
        if (!fileName.contains(".qm"))
            continue;

        QTranslator *translator = new QTranslator(&a);
        if (translator->load(fileName, appDir.absolutePath()))
        {
            qApp->installTranslator(translator);            
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
    QSystemTrayIcon *m_sysTrayIcon = new QSystemTrayIcon(&w);
    m_sysTrayIcon->setIcon(QIcon(":/toolsimage.ico"));
    m_sysTrayIcon->show();
    QMenu *menuTray = new QMenu(&w);
    QAction *actQuit = new QAction(qApp->tr("exit"), menuTray);
    menuTray->addAction(actQuit);
    m_sysTrayIcon->setContextMenu(menuTray);
    m_sysTrayIcon->setToolTip(qApp->tr("qitools ") + VERSION_STRING);
    //    m_sysTrayIcon->showMessage(tr("tips"), tr("VERSION_STRING"), QSystemTrayIcon::Information, 5000);

//    connect(m_sysTrayIcon, &QSystemTrayIcon::activated, this, [=](){ });
    QObject::connect(actQuit, &QAction::triggered, qApp, &QApplication::quit);

    int ret = a.exec();
    return ret;
}
