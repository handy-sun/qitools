#include "qitoolswindow.h"
#include "ui_qitoolswindow.h"
#include <QFile>
#include <QKeyEvent>
#include <QDebug>
#include <QListWidget>

QiToolsWindow::QiToolsWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QiToolsWindow)
{
    ui->setupUi(this);

    setStyleSheet(readStyleSheetFile(":/QiTools.css"));

    if (ui->listWidget->count() == ui->stackedWidget->count())
    {
        connect(ui->listWidget, &QListWidget::currentRowChanged,
                ui->stackedWidget, &QStackedWidget::setCurrentIndex);
    }
    ui->listWidget->setCurrentRow(1);
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
