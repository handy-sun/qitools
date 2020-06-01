#ifndef QITOOLSWINDOW_H
#define QITOOLSWINDOW_H

#include <QMainWindow>

namespace Ui {
class QiToolsWindow;
}

class QiToolsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QiToolsWindow(QWidget *parent = 0);
    ~QiToolsWindow();

private:
    Ui::QiToolsWindow *ui;
};

#endif // QITOOLSWINDOW_H
