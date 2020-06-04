#ifndef PROPERTYMANAGER_H
#define PROPERTYMANAGER_H

#include <functional>
#include <QHash>

class PropertyManager
{
public:
    PropertyManager();

    inline void setArgumentType(int type) { m_argumentType = type; }
    inline void setPrefix(const QString &prefix) { m_prefix = prefix; }
    QString generateCode(const QString &source, bool isInline = false);

private:
    int         m_argumentType; // 0：值类型 1：const T &
    QString     m_prefix;
    QHash<QString, std::function<QString(const QString &type,
                                         const QString &name,
                                         const QString &value,
                                         const QString &notify)> >
    m_propertyType;

};

#endif // PROPERTYMANAGER_H
