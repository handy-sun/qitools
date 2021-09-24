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
    ~QiToolsWindow() Q_DECL_OVERRIDE;

protected:
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::QiToolsWindow *ui;

};

QString readStyleSheetFile(const QString &rcFile);

#endif // QITOOLSWINDOW_H
