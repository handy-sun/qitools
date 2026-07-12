#include "sourcefilefilter.h"

#include <QDir>
#include <QFileInfo>

#include <algorithm>

using namespace CodecConvert;

namespace {

QString normalizedPath(const QFileInfo &fileInfo)
{
    const QString canonicalPath = fileInfo.canonicalFilePath();
    if (!canonicalPath.isEmpty())
        return QDir::cleanPath(canonicalPath);

    return QDir::cleanPath(fileInfo.absoluteFilePath());
}

void scanDirectory(const QString &directoryPath,
                   const QSet<QString> &suffixes,
                   QSet<QString> &seenFiles,
                   QStringList &files)
{
    const QFileInfo directoryInfo(directoryPath);
    if (!directoryInfo.exists() || !directoryInfo.isDir())
        return;

    const QDir directory(normalizedPath(directoryInfo));
    const auto entries = directory.entryInfoList(
        QDir::Dirs | QDir::Files | QDir::Readable | QDir::NoDotAndDotDot,
        QDir::Name | QDir::DirsFirst);

    for (const QFileInfo &entry : entries)
    {
        if (entry.isDir())
        {
            if (!entry.isSymLink() && !SourceFileFilter::isExcludedDirectory(entry.fileName()))
                scanDirectory(entry.absoluteFilePath(), suffixes, seenFiles, files);
            continue;
        }

        if (!entry.isFile() || !SourceFileFilter::isSupportedFile(entry.filePath(), suffixes))
            continue;

        const QString path = normalizedPath(entry);
        if (!seenFiles.contains(path))
        {
            seenFiles.insert(path);
            files.append(path);
        }
    }
}

} // namespace

QList<LanguageFilter> SourceFileFilter::languageFilters()
{
    return {
        {"C/C++", {"c", "cc", "cpp", "cxx", "h", "hh", "hpp", "hxx", "inl"}},
        {"Python", {"py", "pyi"}},
        {"Rust", {"rs"}},
        {"Nix", {"nix"}},
        {"C#", {"cs"}},
        {"Swift", {"swift"}},
        {"Objective-C", {"m", "mm"}},
        {"JavaScript", {"js", "mjs", "cjs", "jsx"}},
        {"TypeScript", {"ts", "mts", "cts", "tsx"}},
    };
}

QSet<QString> SourceFileFilter::allSuffixes()
{
    QSet<QString> suffixes;
    for (const auto &filter : languageFilters())
    {
        for (const QString &suffix : filter.suffixes)
            suffixes.insert(suffix);
    }
    return suffixes;
}

bool SourceFileFilter::isSupportedFile(const QString &filePath, const QSet<QString> &suffixes)
{
    return suffixes.contains(QFileInfo(filePath).suffix().toLower());
}

bool SourceFileFilter::isExcludedDirectory(const QString &directoryName)
{
    if (directoryName.startsWith('.'))
        return true;

    const QString normalizedName = directoryName.toLower();
    static const QSet<QString> excludedNames = {
        "build", "target", "node_modules", "venv", "__pycache__"
    };

    return excludedNames.contains(normalizedName)
        || normalizedName.startsWith("cmake-build-");
}

QStringList SourceFileFilter::scanDirectories(const QStringList &directoryPaths,
                                              const QSet<QString> &suffixes)
{
    QSet<QString> seenFiles;
    QStringList files;

    for (const QString &directoryPath : directoryPaths)
        scanDirectory(directoryPath, suffixes, seenFiles, files);

    std::sort(files.begin(), files.end(), [](const QString &left, const QString &right) {
        return QString::compare(left, right, Qt::CaseInsensitive) < 0;
    });
    return files;
}
