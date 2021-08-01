#include <QFile>
#include <QKeyEvent>
#include <QDebug>
#include <QListWidget>
#include "qitoolswindow.h"
#include "ui_qitoolswindow.h"


QiToolsWindow::QiToolsWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QiToolsWindow)
{
    ui->setupUi(this);
//    setStyleSheet(readStyleSheetFile(":/QiTools.css"));
    if (ui->listWidget->count() == ui->stackedWidget->count())
    {
        connect(ui->listWidget, &QListWidget::currentRowChanged, ui->stackedWidget, &QStackedWidget::setCurrentIndex);
    }
//    ui->stackedWidget->setFocusPolicy(Qt::StrongFocus);
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
    if (event->key() == Qt::Key_Escape)
    {
        close();
    }
//    else if (event->key() == Qt::Key_Minus)
//    {
//        showMinimized();
//    }
}

void QiToolsWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
    QSettings ini(qApp->applicationDirPath() + "/QiTools.ini", QSettings::IniFormat);
    ini.setValue("Preference/index", QVariant(ui->stackedWidget->currentIndex()));
}

QString readStyleSheetFile(const QString &rcFile)
{
    QFile file(/*qApp->applicationDirPath() + */ rcFile);
    if (!file.exists())
    {
        qDebug("File Is Not Exists.");
        return "";
    }
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        QString styleSheet = file.readAll();
        file.close();
        return styleSheet;
    }
    else
    {
        return file.errorString();
    }
}
