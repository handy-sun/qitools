#include "pluginmanager.h"
#include "pluginterface.h"

#include <QtPlugin>
#include <QPluginLoader>
#include <QDebug>

using namespace Core;

static PluginManager *m_instance = nullptr;

// static
PluginManager *PluginManager::instance()
{
    return m_instance;
}

PluginManager::PluginManager()
{
    if (!m_instance)
        m_instance = this;
}

PluginManager::~PluginManager()
{

}

void PluginManager::loadPlugins(const QString &dirPath)
{
    // firstly load static plugin
    const auto staticInstances = QPluginLoader::staticInstances();
    for (QObject *staticObj : staticInstances)
    {
        auto staticPlug = qobject_cast<PlugInterface *>(staticObj);
        qDebug() << "staticPlug: %x" << staticPlug; // DO STH
        if (staticPlug)
            m_pluginList.push_back(staticPlug);
    }

    if (!QDir(dirPath).exists())
    {
        qCritical() << "Plugin Path :" << dirPath << "isn't exist.";
        return;
    }

    auto _dir = QDir(dirPath);
#ifdef QT_DEBUG
    if (_dir.cd("debug"))
#endif
    {
        m_pluginsDir = _dir;
    }

    const auto entryList = m_pluginsDir.entryList(QDir::Files, QDir::Size);
    qDebug() << entryList;
//    for (const QString &fileName : entryList)
    for (auto iter = entryList.constBegin(); iter != entryList.constEnd(); ++iter)
    {
        QString fileName = *iter;
        if (!fileName.endsWith(DYNAMIC_SUFFIX))
            continue;

        QPluginLoader loader(m_pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin)
        {
            auto dynamicPlug = qobject_cast<PlugInterface *>(plugin);
            if (dynamicPlug)
            {
                m_pluginList.push_back(dynamicPlug);
                if (plugin->inherits("QWidget"))
                {
                    auto wg = qobject_cast<QWidget *>(plugin);
                    if (wg)
                        m_strWidgetHash.insert(dynamicPlug->pluginName(), wg);
                }
            }
        }
        else
            qCritical() << "load plugin error:" << loader.errorString();
    }
}

