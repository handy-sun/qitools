#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QList>
#include <QDir>
#include <QWidget>
#include <QMap>

namespace Core {

class PlugInterface;

class PluginManager
{
public:
    static PluginManager *instance();

    void loadPlugins(const QString &dirPath);

    QList<PlugInterface *> pluginList() const { return m_pluginList; }
    QMap<QString, QWidget *> stringWidgetHash() const { return m_strWidgetHash; }

protected:
    PluginManager();
    virtual ~PluginManager();

private:
    QDir m_pluginsDir;
    QList<PlugInterface *> m_pluginList;
    QMap<QString, QWidget *> m_strWidgetHash;

};

}



#endif // PLUGINMANAGER_H
