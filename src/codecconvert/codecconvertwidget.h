#ifndef CODECCONVERTWIDGET_H
#define CODECCONVERTWIDGET_H

#include <QWidget>
#include <QSet>
#include "../core/pluginterface.h"

class QTableWidgetItem;
class QShowEvent;

namespace Ui {
class CodecConvertWidget;
}

namespace CodecConvert {

class CodecConvertWidget : public QWidget, Core::PlugInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.sooncheer.QiTools.PlugInterface.049c19-1.0")
    Q_INTERFACES(Core::PlugInterface)
public:
    explicit CodecConvertWidget(QWidget *parent = nullptr);
    ~CodecConvertWidget() override;

    QString pluginName() const override { return tr("CodecConvert"); }

protected:
    void showEvent(QShowEvent *event) override;

private:
    Ui::CodecConvertWidget *ui;
    QSet<QString>           m_filePaths;
    bool                    m_initialColumnWidthsSet = false;

    QString getCodeString(const QByteArray &ba);
    QStringList sourceFileNameFilters() const;
    QStringList checkedFiles() const;
    void appendFiles(const QStringList &filePaths);
    void setInitialColumnWidths();
    void updateSelectionState();

private Q_SLOTS:
    void on_pushButtonAddFiles_clicked();
    void on_pushButtonAddDirectories_clicked();
    void on_pushButtonRemove_clicked();
    void on_checkBoxSelectAll_clicked(bool checked);
    void on_tableWidgetFiles_itemChanged(QTableWidgetItem *item);
    void on_pushButtonConvert_clicked();
    void on_PushButtonSetDir_clicked();

};

}

#endif  // CODECCONVERTWIDGET_H
