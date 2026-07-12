#ifndef SOURCEFILEFILTER_H
#define SOURCEFILEFILTER_H

#include <QList>
#include <QSet>
#include <QString>
#include <QStringList>

namespace CodecConvert {

struct LanguageFilter
{
    QString name;
    QStringList suffixes;
};

class SourceFileFilter
{
public:
    static QList<LanguageFilter> languageFilters();
    static QSet<QString> allSuffixes();
    static bool isSupportedFile(const QString &filePath, const QSet<QString> &suffixes);
    static bool isExcludedDirectory(const QString &directoryName);
    static QStringList scanDirectories(const QStringList &directoryPaths,
                                       const QSet<QString> &suffixes);
};

} // namespace CodecConvert

#endif // SOURCEFILEFILTER_H
