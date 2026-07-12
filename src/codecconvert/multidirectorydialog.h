#ifndef MULTIDIRECTORYDIALOG_H
#define MULTIDIRECTORYDIALOG_H

#include <QDialog>
#include <QMap>
#include <QSet>
#include <QStringList>

class QCheckBox;
class QFileSystemModel;
class QLabel;
class QTreeView;

namespace CodecConvert {

class MultiDirectoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MultiDirectoryDialog(QWidget *parent = nullptr);

    QStringList selectedDirectories() const;
    QSet<QString> selectedSuffixes() const;

private:
    QFileSystemModel *m_fileSystemModel;
    QTreeView *m_treeView;
    QLabel *m_selectionLabel;
    QMap<QCheckBox *, QStringList> m_filterCheckBoxes;

    void updateAcceptState();
};

} // namespace CodecConvert

#endif // MULTIDIRECTORYDIALOG_H
