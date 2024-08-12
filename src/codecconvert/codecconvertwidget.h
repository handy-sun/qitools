#ifndef CODECCONVERTWIDGET_H
#define CODECCONVERTWIDGET_H

#include <QWidget>
#include <QStringList>
#include "../core/pluginterface.h"

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

private:
    Ui::CodecConvertWidget *ui;
    QStringList             m_fileList;

    QString getCodeString(const QByteArray &ba);

private Q_SLOTS:
    void on_pushButtonOpen_clicked();
    void on_pushButtonConvert_clicked();
    void on_PushButtonSetDir_clicked();

};

}

#endif  // CODECCONVERTWIDGET_H
