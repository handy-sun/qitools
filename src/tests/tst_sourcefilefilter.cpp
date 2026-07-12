#include "sourcefilefilter.h"

#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QtTest>

using namespace CodecConvert;

namespace {

void createFile(const QString &path)
{
    QFile file(path);
    QVERIFY2(file.open(QIODevice::WriteOnly), qPrintable(file.errorString()));
    file.write("sample\n");
}

} // namespace

class SourceFileFilterTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void exposesExpectedLanguageSuffixes();
    void matchesSuffixesCaseInsensitively();
    void identifiesExcludedDirectories();
    void scansRecursivelyAndDeduplicatesFiles();
};

void SourceFileFilterTest::exposesExpectedLanguageSuffixes()
{
    const auto filters = SourceFileFilter::languageFilters();
    QMap<QString, QStringList> suffixesByLanguage;
    for (const auto &filter : filters)
        suffixesByLanguage.insert(filter.name, filter.suffixes);

    QCOMPARE(suffixesByLanguage.value("C/C++"),
             QStringList({"c", "cc", "cpp", "cxx", "h", "hh", "hpp", "hxx", "inl"}));
    QCOMPARE(suffixesByLanguage.value("Python"), QStringList({"py", "pyi"}));
    QCOMPARE(suffixesByLanguage.value("Rust"), QStringList({"rs"}));
    QCOMPARE(suffixesByLanguage.value("Nix"), QStringList({"nix"}));
    QCOMPARE(suffixesByLanguage.value("C#"), QStringList({"cs"}));
    QCOMPARE(suffixesByLanguage.value("Swift"), QStringList({"swift"}));
    QCOMPARE(suffixesByLanguage.value("Objective-C"), QStringList({"m", "mm"}));
    QCOMPARE(suffixesByLanguage.value("JavaScript"), QStringList({"js", "mjs", "cjs", "jsx"}));
    QCOMPARE(suffixesByLanguage.value("TypeScript"), QStringList({"ts", "mts", "cts", "tsx"}));
}

void SourceFileFilterTest::matchesSuffixesCaseInsensitively()
{
    const auto suffixes = SourceFileFilter::allSuffixes();

    QVERIFY(SourceFileFilter::isSupportedFile("main.CPP", suffixes));
    QVERIFY(SourceFileFilter::isSupportedFile("default.nix", suffixes));
    QVERIFY(!SourceFileFilter::isSupportedFile("package.json", suffixes));
    QVERIFY(!SourceFileFilter::isSupportedFile("README", suffixes));
}

void SourceFileFilterTest::identifiesExcludedDirectories()
{
    QVERIFY(SourceFileFilter::isExcludedDirectory(".git"));
    QVERIFY(SourceFileFilter::isExcludedDirectory(".hidden"));
    QVERIFY(SourceFileFilter::isExcludedDirectory("build"));
    QVERIFY(SourceFileFilter::isExcludedDirectory("Build"));
    QVERIFY(SourceFileFilter::isExcludedDirectory("cmake-build-debug"));
    QVERIFY(SourceFileFilter::isExcludedDirectory("node_modules"));
    QVERIFY(SourceFileFilter::isExcludedDirectory("Node_Modules"));
    QVERIFY(SourceFileFilter::isExcludedDirectory("target"));
    QVERIFY(!SourceFileFilter::isExcludedDirectory("src"));
    QVERIFY(!SourceFileFilter::isExcludedDirectory("builder"));
}

void SourceFileFilterTest::scansRecursivelyAndDeduplicatesFiles()
{
    QTemporaryDir temporaryDir;
    QVERIFY(temporaryDir.isValid());

    QDir root(temporaryDir.path());
    QVERIFY(root.mkpath("src/nested"));
    QVERIFY(root.mkpath("src/.git"));
    QVERIFY(root.mkpath("src/node_modules/pkg"));
    QVERIFY(root.mkpath("src/cmake-build-debug"));

    createFile(root.filePath("src/main.cpp"));
    createFile(root.filePath("src/nested/tool.PY"));
    createFile(root.filePath("src/nested/data.json"));
    createFile(root.filePath("src/.git/ignored.rs"));
    createFile(root.filePath("src/node_modules/pkg/ignored.ts"));
    createFile(root.filePath("src/cmake-build-debug/ignored.c"));

    const auto files = SourceFileFilter::scanDirectories(
        {root.filePath("src"), root.filePath("src/nested")},
        SourceFileFilter::allSuffixes());

    QCOMPARE(files,
             QStringList({QFileInfo(root.filePath("src/main.cpp")).canonicalFilePath(),
                          QFileInfo(root.filePath("src/nested/tool.PY")).canonicalFilePath()}));
}

QTEST_APPLESS_MAIN(SourceFileFilterTest)

#include "tst_sourcefilefilter.moc"
