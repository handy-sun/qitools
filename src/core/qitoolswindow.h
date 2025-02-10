#ifndef QITOOLSWINDOW_H
#define QITOOLSWINDOW_H

#include <QMainWindow>
#include "pluginmanager.h"

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
    explicit QiToolsWindow(const QString &iniPath, QWidget *parent = nullptr);
    ~QiToolsWindow() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    Internal::QiToolsWindow_Ui  *ui;
    decltype(PluginManager::instance()->stringWidgetHash()) m_stackedHash;
    QString m_configIniPath;

private Q_SLOTS:
    void onListWidgetPressed(int r);

};

}


#endif // QITOOLSWINDOW_H
