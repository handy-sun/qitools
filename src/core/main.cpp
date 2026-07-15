#include "stable.h"
#include "qitoolswindow.h"

using namespace Core;

static QString s_logFile;

namespace {

constexpr qreal kBaseDpi = 96.0;

qreal screenScaleFactor(const QScreen *screen)
{
    if (!screen)
        return 1.0;

    const qreal dpi = screen->logicalDotsPerInch();
    if (dpi <= 0.0)
        return 1.0;

    return qBound<qreal>(1.0, dpi / kBaseDpi, 2.0);
}

QSize scaledSize(const QSize &baseSize, const QScreen *screen)
{
    const qreal scale = screenScaleFactor(screen);
    return QSize(qRound(baseSize.width() * scale), qRound(baseSize.height() * scale));
}

QSize defaultWindowSize(const QScreen *screen)
{
    QSize size = scaledSize(QSize(1280, 720), screen);
    if (!screen)
        return size;

    const QSize available = screen->availableGeometry().size();
    const QSize maxSize(qRound(available.width() * 0.9), qRound(available.height() * 0.9));
    const QSize minSize = QSize(960, 540).boundedTo(maxSize);
    size.setWidth(qMin(size.width(), maxSize.width()));
    size.setHeight(qMin(size.height(), maxSize.height()));
    return size.expandedTo(minSize).boundedTo(maxSize);
}

void configureHighDpiScaling()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
}

} // namespace

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
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    QFile file(s_logFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
        return;

    QString formatMsg = qFormatLogMessage(type, context, msg);

    QTextStream textStream(&file);
    //textStream.setCodec(QTextCodec::codecForName("UTF-8"));
    textStream << formatMsg << "\r\n";
    file.flush();
    file.close();
}

QString ensureCfgDir(const QString &cfgDirName)
{
    // QDir dotConfigDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
    QString dotConfPath = QDir::homePath() + "/.config";
    qDebug() << dotConfPath;
    QDir dotConfigDir = dotConfPath;
    if (dotConfigDir.exists(cfgDirName))
    {
        if (dotConfigDir.cd(cfgDirName))
        {
            qDebug() << "exists qitoolsConfigDir:" << dotConfigDir.path();
            return dotConfigDir.path();
        }
        else
        {
            qWarning() << dotConfigDir.path() << "cannot cd:" << cfgDirName;
            return QString();
        }
    }
    else if (dotConfigDir.mkpath(cfgDirName))
    {
        qDebug() << "dotConfigDir:" << dotConfigDir.path();
        if (dotConfigDir.cd(cfgDirName))
        {
            qDebug() << "mkpath qitoolsConfigDir:" << dotConfigDir.path();
            return dotConfigDir.path();
        }
    }
    return QString();
}

int main(int argc, char *argv[])
{
    configureHighDpiScaling();

    QApplication a(argc, argv);
    QFont appFont = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
    appFont.setPointSizeF(qMax(g_kMinimumApplicationFontPointSize, appFont.pointSizeF()));
    a.setFont(appFont);
    a.setApplicationVersion(VERSION_STRING);
//    setvbuf(stdout, nullptr, _IONBF, 1024);

    QString qiToolsConfDir = ensureCfgDir("qitools");
    if (qiToolsConfDir.isEmpty())
    {
        qWarning() << "Access config directory failed!";
        return 1;
    }

    QString qiToolsConfIni = qiToolsConfDir + "/qitools.ini";
    QSettings ini(qiToolsConfIni, QSettings::IniFormat);
//    qSetMessagePattern("%{message} [%{file}:%{line} - %{qthreadptr} | %{time MM.dd hh:mm:ss.zzz}]");
    if (ini.value("Preference/logoutOn").toBool())
    {
        addNewSignInLog(qiToolsConfDir + "/qitools.log");
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
    QiToolsWindow w(qiToolsConfIni);
    w.setWindowTitle("qitools");
    w.setWindowIcon(QIcon(":/toolsimage.svg"));
    if (_ba.isEmpty() || !w.restoreGeometry(_ba))
        w.resize(defaultWindowSize(QGuiApplication::primaryScreen()));

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
