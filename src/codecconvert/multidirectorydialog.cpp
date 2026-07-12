#include "multidirectorydialog.h"

#include "sourcefilefilter.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QPushButton>
#include <QTreeView>
#include <QVBoxLayout>

#include <algorithm>

using namespace CodecConvert;

MultiDirectoryDialog::MultiDirectoryDialog(QWidget *parent)
    : QDialog(parent)
    , m_fileSystemModel(new QFileSystemModel(this))
    , m_treeView(new QTreeView(this))
    , m_selectionLabel(new QLabel(this))
{
    setWindowTitle(tr("Select source directories"));
    setMinimumSize(680, 520);
    resize(820, 620);

    auto *mainLayout = new QVBoxLayout(this);
    auto *instructionLabel = new QLabel(
        tr("Select one or more directories with Ctrl or Shift. Subdirectories are scanned recursively."),
        this);
    instructionLabel->setWordWrap(true);
    mainLayout->addWidget(instructionLabel);

    m_fileSystemModel->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Drives);
    const QModelIndex rootIndex = m_fileSystemModel->setRootPath(QString());
    m_treeView->setModel(m_fileSystemModel);
    m_treeView->setRootIndex(rootIndex);
    m_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_treeView->setAlternatingRowColors(true);
    m_treeView->setSortingEnabled(true);
    m_treeView->sortByColumn(0, Qt::AscendingOrder);
    m_treeView->header()->setStretchLastSection(false);
    m_treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    for (int column = 1; column < m_fileSystemModel->columnCount(); ++column)
        m_treeView->hideColumn(column);
    mainLayout->addWidget(m_treeView, 1);

    auto *filterGroup = new QGroupBox(tr("Source file types"), this);
    auto *filterLayout = new QGridLayout(filterGroup);
    const auto languageFilters = SourceFileFilter::languageFilters();
    for (int index = 0; index < languageFilters.size(); ++index)
    {
        const auto &languageFilter = languageFilters.at(index);
        auto *checkBox = new QCheckBox(languageFilter.name, filterGroup);
        checkBox->setChecked(true);
        checkBox->setToolTip("*." + languageFilter.suffixes.join(" *."));
        filterLayout->addWidget(checkBox, index / 3, index % 3);
        m_filterCheckBoxes.insert(checkBox, languageFilter.suffixes);
        connect(checkBox, &QCheckBox::toggled, this, &MultiDirectoryDialog::updateAcceptState);
    }
    mainLayout->addWidget(filterGroup);

    mainLayout->addWidget(m_selectionLabel);

    auto *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Add directories"));
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MultiDirectoryDialog::updateAcceptState);
    mainLayout->addWidget(buttonBox);

    updateAcceptState();
}

QStringList MultiDirectoryDialog::selectedDirectories() const
{
    QSet<QString> uniqueDirectories;
    const auto rows = m_treeView->selectionModel()->selectedRows(0);
    for (const QModelIndex &index : rows)
    {
        const QFileInfo fileInfo = m_fileSystemModel->fileInfo(index);
        if (!fileInfo.isDir())
            continue;

        QString path = fileInfo.canonicalFilePath();
        if (path.isEmpty())
            path = fileInfo.absoluteFilePath();
        uniqueDirectories.insert(QDir::cleanPath(path));
    }

    QStringList directories = uniqueDirectories.values();
    std::sort(directories.begin(), directories.end(), [](const QString &left, const QString &right) {
        return QString::compare(left, right, Qt::CaseInsensitive) < 0;
    });
    return directories;
}

QSet<QString> MultiDirectoryDialog::selectedSuffixes() const
{
    QSet<QString> suffixes;
    for (auto iter = m_filterCheckBoxes.constBegin(); iter != m_filterCheckBoxes.constEnd(); ++iter)
    {
        if (!iter.key()->isChecked())
            continue;
        for (const QString &suffix : iter.value())
            suffixes.insert(suffix);
    }
    return suffixes;
}

void MultiDirectoryDialog::updateAcceptState()
{
    const int directoryCount = selectedDirectories().size();
    m_selectionLabel->setText(tr("%n directory selected", "", directoryCount));

    auto *buttonBox = findChild<QDialogButtonBox *>();
    if (buttonBox)
        buttonBox->button(QDialogButtonBox::Ok)->setEnabled(
            directoryCount > 0 && !selectedSuffixes().isEmpty());
}
