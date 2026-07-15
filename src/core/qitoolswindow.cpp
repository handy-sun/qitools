#include "qitoolswindow.h"
#include "pluginmanager.h"
#include "pluginterface.h"

#include "stable.h"

using namespace Core;

namespace {

constexpr qreal kBaseDpi = 96.0;

qreal screenScaleFactor(const QWidget *widget)
{
    const QScreen *screen = nullptr;
    if (widget && widget->windowHandle())
        screen = widget->windowHandle()->screen();
    if (!screen)
        screen = QGuiApplication::primaryScreen();

    const qreal dpi = screen ? screen->logicalDotsPerInch() : kBaseDpi;
    if (dpi <= 0.0)
        return 1.0;

    return qBound<qreal>(1.0, dpi / kBaseDpi, 2.0);
}

int scaled(int value, const QWidget *widget)
{
    return qRound(value * screenScaleFactor(widget));
}

} // namespace

class Internal::QiToolsWindow_Ui
{
public:
    QListWidget *listWidget = nullptr;
    QStackedWidget *stackedWidget = nullptr;
    QToolButton *tbtnSetting = nullptr;

    void setupUi(QMainWindow *qitoolsWindow)
    {
        QWidget *centralW = new QWidget(qitoolsWindow);
        QHBoxLayout *horizontalLayout = new QHBoxLayout(centralW);
        horizontalLayout->setSpacing(scaled(6, qitoolsWindow));
        horizontalLayout->setContentsMargins(scaled(6, qitoolsWindow), scaled(6, qitoolsWindow),
                                             scaled(6, qitoolsWindow), scaled(6, qitoolsWindow));
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        QVBoxLayout *leftVerticalLayout = new QVBoxLayout();
        leftVerticalLayout->setSpacing(scaled(3, qitoolsWindow));
        leftVerticalLayout->setContentsMargins(scaled(3, qitoolsWindow), 0,
                                               scaled(6, qitoolsWindow), scaled(6, qitoolsWindow));
        leftVerticalLayout->setObjectName(QString::fromUtf8("leftVerticalLayout"));
        listWidget = new QListWidget(centralW);
        listWidget->setMaximumWidth(scaled(167, qitoolsWindow));
        tbtnSetting = new QToolButton(centralW);
        const int buttonSize = scaled(24, qitoolsWindow);
        tbtnSetting->setMinimumSize(buttonSize, buttonSize);
        tbtnSetting->setIconSize(QSize(scaled(16, qitoolsWindow), scaled(16, qitoolsWindow)));
        tbtnSetting->setIcon(QIcon(qitoolsWindow->style()->standardIcon(QStyle::SP_FileDialogListView)));
        tbtnSetting->setToolTip(QObject::tr("settings"));
        stackedWidget = new QStackedWidget(centralW);
        stackedWidget->setObjectName("stackedWidget");
        //stackedWidget->setMinimumSize(QSize(633, 506));
//        horizontalLayout->addWidget(listWidget);

        QFont appFont = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
        appFont.setPointSizeF(qMax(g_kMinimumApplicationFontPointSize, appFont.pointSizeF()));
        listWidget->setFont(appFont);

        leftVerticalLayout->addWidget(listWidget);
        leftVerticalLayout->addWidget(tbtnSetting);
        horizontalLayout->addLayout(leftVerticalLayout);
        horizontalLayout->addWidget(stackedWidget);
        horizontalLayout->setStretch(0, 2);
        horizontalLayout->setStretch(1, 7);
        qitoolsWindow->setCentralWidget(centralW);
    }
};

QiToolsWindow::QiToolsWindow(const QString &iniPath, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Internal::QiToolsWindow_Ui)
    , m_configIniPath(iniPath)
{
    ui->setupUi(this);

    PluginManager::instance()->loadPlugins(qApp->applicationDirPath()  + "/plugins");
    const auto plList = PluginManager::instance()->pluginList();
    qDebug() << DYNAMIC_SUFFIX << plList.size();
    m_stackedHash = PluginManager::instance()->stringWidgetHash();
    for (const auto &name : m_stackedHash.keys())
    {
        ui->listWidget->addItem(name);
        ui->stackedWidget->addWidget(m_stackedHash.value(name));
    }

    connect(ui->listWidget, &QListWidget::currentRowChanged, this, &QiToolsWindow::onListWidgetPressed);

    QSettings ini(m_configIniPath, QSettings::IniFormat);
    int index = ini.value("Preference/index").toInt();
    if (index >= 0 && index < ui->stackedWidget->count())
        ui->listWidget->setCurrentRow(index);
    else
        ui->listWidget->setCurrentRow(0);
}

QiToolsWindow::~QiToolsWindow()
{
    delete ui;
}
void QiToolsWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Escape:
        close();
        break;
    case Qt::Key_F12:
        qApp->setStyleSheet("file:///" + qApp->applicationDirPath() + "/QiTools.css");
        break;
    default:
        break;
    }
}

void QiToolsWindow::closeEvent(QCloseEvent *)
{
//    if (QMessageBox::No == QMessageBox::warning(this, "warning", "exit?"))
//    {
//        event->ignore();
//        return;
//    }
    QSettings ini(m_configIniPath, QSettings::IniFormat);
    if (!ini.contains("Preference/closeAppWithWindow"))
    {
        ini.setValue("Preference/closeAppWithWindow", QVariant(0));
    }

    ini.setValue("Preference/index", QVariant(ui->stackedWidget->currentIndex()));
    ini.setValue("Preference/geometry", saveGeometry());
    if (!ini.contains("Preference/styleMode"))
        ini.setValue("Preference/styleMode", QVariant(0));

    if (ini.value("Preference/closeAppWithWindow").toInt() != 0)
        qApp->quit();
}

void QiToolsWindow::onListWidgetPressed(int r)
{
    if (r < 0 || r >= ui->listWidget->count())
        return;

    auto _str = ui->listWidget->item(r)->text();
    ui->stackedWidget->setCurrentWidget(m_stackedHash.value(_str));
}
