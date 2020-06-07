#include "qitoolswindow.h"
#include "ui_qitoolswindow.h"
#include <QFile>

QiToolsWindow::QiToolsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QiToolsWindow)
{
    ui->setupUi(this);
//    ui->treeWidget->setAutoExpandDelay(2);
//    ui->treeWidget->setIndentation(20);
//    ui->treeWidget->setHeaderHidden(true);
//    ui->treeWidget->setColumnCount(1);
    setStyleSheet(readStyleSheetFile(":/QiTools.css"));
}

QiToolsWindow::~QiToolsWindow()
{
    delete ui;
}

QString QiToolsWindow::readStyleSheetFile(const QString &rcFile) const
{
    QFile file(/*qApp->applicationDirPath() + */rcFile);
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
