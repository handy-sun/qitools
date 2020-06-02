#ifndef PROPERTYMANAGER_H
#define PROPERTYMANAGER_H

#include <functional>

class QString;
template <class key, class T> class QHash;

class PropertyManager
{
public:
    PropertyManager();

private:
    int m_argumentType; // 0：值类型 1：const T &
    QHash<QString, std::function<QString(const QString &type,
                                         const QString &name,
                                         const QString &value,
                                         const QString &notify)> >
    m_propertyType;

};

#endif // PROPERTYMANAGER_H
