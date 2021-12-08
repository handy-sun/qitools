#ifndef QITOOLSWINDOW_H
#define QITOOLSWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QSystemTrayIcon;
QT_END_NAMESPACE

namespace Core {

namespace Internal {
class QiToolsWindow_Ui;
}

class QiToolsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QiToolsWindow(QWidget *parent = nullptr);
    ~QiToolsWindow() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    Internal::QiToolsWindow_Ui  *ui;
    QSystemTrayIcon             *m_sysTrayIcon;
    QHash<QString, QWidget *>   m_stackedHash;

private Q_SLOTS:
    void onListWidgetPressed(int r);

};

}

//QString readStyleSheetFile(const QString &rcFile);

#endif // QITOOLSWINDOW_H
