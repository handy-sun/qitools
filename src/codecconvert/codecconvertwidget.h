#ifndef CODECCONVERTWIDGET_H
#define CODECCONVERTWIDGET_H

#include <QWidget>
#include <QStringList>

namespace Ui {
class CodecConvertWidget;
}

class CodecConvertWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CodecConvertWidget(QWidget *parent = nullptr);
    ~CodecConvertWidget();

private:
    Ui::CodecConvertWidget *ui;
    QStringList             m_fileList;

    QString getCodeString(const QByteArray &ba);

private Q_SLOTS:
    void on_pushButtonOpen_clicked();
    void on_pushButtonConvert_clicked();

    void on_PushButtonSetDir_clicked();
};

#endif  // CODECCONVERTWIDGET_H
