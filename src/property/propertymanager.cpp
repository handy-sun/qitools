#include "propertymanager.h"
#include <QStringList>


PropertyManager::PropertyManager()
    : m_argumentType(0)
    , m_prefix("")
    , m_pointerAlignment(0)
{
    m_propertyType["READ"] = [=](const QString &type, const QString &name, const QString &value, const QString &)->QString
    {
        return QString("%1 %2() const { return %3%4; }\n").arg(type).arg(value).arg(m_prefix).arg(name);
    };

    m_propertyType["WRITE"] = [=](const QString &type, const QString &name, const QString &value, const QString &notifyValue)->QString
    {
        if (notifyValue.isEmpty())
        {
            if (0 == m_argumentType)
            {
                return QString("void %1(%2 %3) { %4%3 = %3; }\n").
                        arg(value).arg(type).arg(name).arg(m_prefix);
            }
            if (1 == m_argumentType)
            {
                return QString("void %1(const %2 &%3) { %4%3 = %3; }\n").
                       arg(value).arg(type).arg(name).arg(m_prefix);
            }
        }
        else
        {
            if (0 == m_argumentType)
            {
                return QString("void %1(%2 %3) { if (%4%3 != %3) { %4%3 = %3; Q_EMIT %5(%3); } }\n").
                       arg(value).arg(type).arg(name).arg(m_prefix).arg(notifyValue);
            }
            if (1 == m_argumentType)
            {
                return QString("void %1(const %2 &%3) { if (%4%3 != %3) { %4%3 = %3; Q_EMIT %5(%3); } }\n").
                       arg(value).arg(type).arg(name).arg(m_prefix).arg(notifyValue);
            }
        }
        return "??";
    };

    m_propertyType["NOTIFY"] = [=](const QString &type, const QString &name, const QString &value, const QString &)->QString
    {
        if (0 == m_argumentType)
        {
            return QString("void %1(%2 %3);\n").
                   arg(value).arg(type).arg(name);
        }
        if (1 == m_argumentType)
        {
            return QString("void %1(const %2 &%3);\n").
                   arg(value).arg(type).arg(name);
        }
        return "none";
    };

    m_propertyType["RESET"] = [=](const QString &, const QString &name, const QString &value, const QString &notifyValue)->QString
    {
        return QString("void %1() { if (%2%3 != %4()) { %2%3 = %4(); Q_EMIT %5(%3); } }\n").
        arg(value).arg(m_prefix).arg(name).arg(name).arg(notifyValue);
    };
}

QString PropertyManager::generateCode(const QString &source, bool isInline)
{
    if (!source.startsWith("Q_PROPERTY(" ) || !source.endsWith(")"))
        return "error format";

    QString code;
    QString line = source;

    line.remove(0, 11);
    line.remove(line.size() - 1, 1);

    const QStringList &&elements = line.split(" ");

    if (1 == elements.size() % 2)
        return "property's count donnot equal to content's count";

    QString type;
    QString name;
    QString notifyValue;
    QString signal;
    QList<QPair<QString, QString>> datas;

    for (int index = 0; index < elements.size(); index += 2)
    {
        if (index)
        {
            const auto &key = elements[index];
            const auto &value = elements[index + 1];

            if (!m_propertyType.contains(key))
                continue;

            if (key == "NOTIFY")
            {
                notifyValue = value;
            }

            datas.push_back({ key, value });
        }
        else
        {
            type = elements[0];
            name = elements[1];
        }
    }

    if (type.isEmpty() || name.isEmpty())
        return "error: empty";

    code += "public:\n";
    for (const auto &data : datas)
    {
        if (data.first == "NOTIFY")
        {
            signal += "\nQ_SIGNALS:\n";
            signal += m_propertyType[data.first](type, name, data.second, notifyValue);
            continue;
        }
        else if (isInline)
        {
            code += "inline ";
        }
        code += m_propertyType[data.first](type, name, data.second, notifyValue);
    }
    code += QString("\nprivate:\n%1 %2%3;\n").arg(type).arg(m_prefix).arg(name);

    if (!signal.isEmpty())
    {
        code += signal;
    }

    if (m_pointerAlignment > 0)
    {
        code.replace("* ", " *");
    }
    return code;
}
