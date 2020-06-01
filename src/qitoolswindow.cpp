#include "qitoolswindow.h"
#include "ui_qitoolswindow.h"

QiToolsWindow::QiToolsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QiToolsWindow)
{
    ui->setupUi(this);
}

QiToolsWindow::~QiToolsWindow()
{
    delete ui;
}
