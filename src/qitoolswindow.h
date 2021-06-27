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
    explicit QiToolsWindow(QWidget *parent = Q_NULLPTR);
    ~QiToolsWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::QiToolsWindow *ui;

};

QString readStyleSheetFile(const QString &rcFile);

#endif // QITOOLSWINDOW_H
