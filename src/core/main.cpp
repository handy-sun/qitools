#include "stable.h"
#include "qitoolswindow.h"

using namespace Core;

static QString s_logFile;

void addNewSignInLog(const QString &configLogFile)
{
    QFile file(configLogFile);
    s_logFile = configLogFile;
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        qDebug() << file.fileName() << "is Not Exist.";
        return;
    }

    QTextStream textStream(&file);
    QString currentTime = QDateTime::currentDateTime().toString("yyyyMMdd hh:mm:ss.zzz");
    textStream << QString("-------------------- %1 --------------------\r\n").arg(currentTime);
    file.flush();
    file.close();
}

void msgOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(type)
    static QMutex mutex;
    mutex.lock();
    QFile file(s_logFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
        return;

    QString formatMsg = qFormatLogMessage(type, context, msg);

    QTextStream textStream(&file);
    //textStream.setCodec(QTextCodec::codecForName("UTF-8"));
    textStream << formatMsg << "\r\n";
    file.flush();
    file.close();
    mutex.unlock();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#if QT_VERSION > 0x050900
    a.setApplicationVersion(VERSION_STRING);
#endif
//    setvbuf(stdout, nullptr, _IONBF, 1024);

    QSettings ini(qApp->applicationDirPath() + "/QiTools.ini", QSettings::IniFormat);
//    qSetMessagePattern("%{message} [%{file}:%{line} - %{qthreadptr} | %{time MM.dd hh:mm:ss.zzz}]");
    if (ini.value("Preference/logoutOn").toBool())
    {
        addNewSignInLog(qApp->applicationDirPath() + "/qitools.log");
        qSetMessagePattern("[%{time MM/dd hh:mm:ss.zzz}][%{type}]%{message} ");
        qInstallMessageHandler(msgOutput);
    }
    else
    {
        qSetMessagePattern("%{message} [%{file}:%{line} - %{threadid} | %{time MM/dd hh:mm:ss.zzz}]");
    }
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

    qDebug() << qApp->libraryPaths();
    QiToolsWindow w;
    w.setWindowTitle("qitools");
    w.setWindowIcon(QIcon(":/toolsimage.svg"));
    if (!_ba.isEmpty())
        w.restoreGeometry(_ba);
    else
        w.resize(1280, 720);

    w.show();
    QSystemTrayIcon *m_sysTrayIcon = new QSystemTrayIcon(&w);
    m_sysTrayIcon->setIcon(QIcon(":/toolsimage.ico")); // svg 做托盘图标无法显示
    m_sysTrayIcon->show();
    QMenu *menuTray = new QMenu(&w);
    QAction *actShow = new QAction(QObject::tr("show window"), menuTray);
    QAction *actQuit = new QAction(QObject::tr("exit"), menuTray);
    menuTray->addAction(actShow);
    menuTray->addAction(actQuit);
    m_sysTrayIcon->setContextMenu(menuTray);
    m_sysTrayIcon->setToolTip(QString("qitools v") + VERSION_STRING);
    //    m_sysTrayIcon->showMessage(tr("tips"), tr("VERSION_STRING"), QSystemTrayIcon::Information, 5000);

//    connect(m_sysTrayIcon, &QSystemTrayIcon::activated, this, [=](){ });
    QObject::connect(actShow, &QAction::triggered, &w, &QiToolsWindow::show);
    QObject::connect(actQuit, &QAction::triggered, qApp, &QApplication::quit);

    int ret = a.exec();
    return ret;
}
