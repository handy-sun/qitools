#include "qitoolswindow.h"
#include "pluginmanager.h"
#include "pluginterface.h"

#include <QFile>
#include <QKeyEvent>
#include <QDebug>
#include <QListWidget>
#include <QSettings>
#include <QStackedWidget>
#include <QBoxLayout>
#include <QApplication>
#include <QMessageBox>
#include <QMenu>

using namespace Core;

class Internal::QiToolsWindow_Ui
{
public:
    QListWidget *listWidget = nullptr;
    QStackedWidget *stackedWidget = nullptr;

    void setupUi(QMainWindow *qitoolsWindow)
    {
        QWidget *centralW = new QWidget(qitoolsWindow);
        QHBoxLayout *horizontalLayout = new QHBoxLayout(centralW);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(6, 6, 6, 6);
        listWidget = new QListWidget(centralW);
        listWidget->setMaximumWidth(167);
        QFont font;
        font.setPointSize(11);
        listWidget->setFont(font);
        stackedWidget = new QStackedWidget(centralW);
        stackedWidget->setObjectName("stackedWidget");
        //stackedWidget->setMinimumSize(QSize(633, 506));
        horizontalLayout->addWidget(listWidget);
        horizontalLayout->addWidget(stackedWidget);
        horizontalLayout->setStretch(0, 2);
        horizontalLayout->setStretch(1, 7);
        qitoolsWindow->setCentralWidget(centralW);
    }
};

QiToolsWindow::QiToolsWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Internal::QiToolsWindow_Ui)
{
    ui->setupUi(this);

    new PluginManager;
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

    QSettings ini(qApp->applicationDirPath() + "/QiTools.ini", QSettings::IniFormat);
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

void QiToolsWindow::closeEvent(QCloseEvent *event)
{
//    if (QMessageBox::No == QMessageBox::warning(this, "warning", "exit?"))
//    {
//        event->ignore();
//        return;
//    }
    event->accept();
    QSettings ini(qApp->applicationDirPath() + "/QiTools.ini", QSettings::IniFormat);
    ini.setValue("Preference/index", QVariant(ui->stackedWidget->currentIndex()));
    ini.setValue("Preference/geometry", saveGeometry());
}

void QiToolsWindow::onListWidgetPressed(int r)
{
    if (r < 0 || r >= ui->listWidget->count())
        return;

    auto _str = ui->listWidget->item(r)->text();
    ui->stackedWidget->setCurrentWidget(m_stackedHash.value(_str));
}

