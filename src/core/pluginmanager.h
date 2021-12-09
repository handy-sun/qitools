#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QList>
#include <QDir>
#include <QWidget>

namespace Core {

class PlugInterface;

class PluginManager : public QObject
{
    Q_OBJECT

public:
    static PluginManager *instance();

    PluginManager();
    ~PluginManager() override;
    void loadPlugins(const QString &dirPath);

    QList<PlugInterface *> pluginList() const { return m_pluginList; }
    QMap<QString, QWidget *> stringWidgetHash() const { return m_strWidgetHash; }

private:
    QDir m_pluginsDir;
    QList<PlugInterface *> m_pluginList;
    QMap<QString, QWidget *> m_strWidgetHash;

};

}



#endif // PLUGINMANAGER_H
