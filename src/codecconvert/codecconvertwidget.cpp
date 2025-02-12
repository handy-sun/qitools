#include "codecconvertwidget.h"
#include "ui_codecconvertwidget.h"
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QTextCodec>
#include <QTimer>
#include <util/encodings/encodings.h>
#include <compact_enc_det/compact_enc_det.h>

using namespace CodecConvert;

CodecConvertWidget::CodecConvertWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CodecConvertWidget)
{
    ui->setupUi(this);

    ui->comboBoxCodec->addItem("UTF-8");
    ui->comboBoxCodec->addItem("GBK");
    ui->comboBoxCodec->addItem("System");
    ui->comboBoxCodec->addItem("UTF-16");
    ui->comboBoxCodec->addItem("GB2312");

    ui->lineEditDir->setReadOnly(true);
    ui->checkBoxWithBom->setChecked(true);
    ui->lineEditDir->setText(qApp->applicationDirPath());

    connect(ui->radioButtonSettingPath, &QRadioButton::toggled, [=](bool checked) {
        ui->lineEditDir->setEnabled(checked);
        ui->PushButtonSetDir->setEnabled(checked);
    });

    ui->radioButtonSettingPath->setChecked(true);
}

CodecConvertWidget::~CodecConvertWidget()
{
    delete ui;
}

QString CodecConvertWidget::getCodeString(const QByteArray &ba)
{
    bool is_reliable;
    int bytes_consumed;

    Encoding encoding = CompactEncDet::DetectEncoding(
        ba.constData(), ba.size(),
        nullptr, nullptr, nullptr,
        UNKNOWN_ENCODING,
        UNKNOWN_LANGUAGE,
        CompactEncDet::WEB_CORPUS,
        false,
        &bytes_consumed,
        &is_reliable);

    const char *encName = EncodingName(encoding);
    qDebug("encoding:%-20s(%2d), reliable:%s, consumeda=%d", encName, encoding, (is_reliable ? " true" : "false"),  bytes_consumed);
    if (encoding == UTF8)
    {
        if (ba.startsWith("\xEF\xBB\xBF"))
        {
            return "UTF-8 with BOM";
        }
        return "UTF-8";
    }
    return encName;
}

void CodecConvertWidget::on_pushButtonOpen_clicked()
{
    QStringList _fileList = QFileDialog::getOpenFileNames(this, "Open file", "", "All(*.*)");

    if (_fileList.isEmpty())
        return;

    ui->listWidget->clear();
    for (const auto &targetFilePath : _fileList)
    {
        QFile file(targetFilePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;

        ui->listWidget->addItem(QFileInfo(file).fileName() + " : " + getCodeString(file.readAll()));
    }
    m_fileList = _fileList;
}

void CodecConvertWidget::on_pushButtonConvert_clicked()
{
    // QApplication::setOverrideCursor(Qt::WaitCursor);
    int count = 0;
    for (const auto &targetFilePath : m_fileList)
    {
        QFile file(targetFilePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;

        QTextStream rwStream(&file);
        rwStream.setGenerateByteOrderMark(false);  // without BOM
        QString _content = rwStream.readAll();
        file.close();

        if (ui->radioButtonSettingPath->isChecked())
        {
            QDir dir(ui->lineEditDir->text());
            file.setFileName(dir.absoluteFilePath(QFileInfo(file).fileName()));
        }

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            continue;

        rwStream.setDevice(&file);
        rwStream.setCodec(ui->comboBoxCodec->currentText().toStdString().c_str());
        rwStream.setGenerateByteOrderMark(ui->checkBoxWithBom->isChecked());
        rwStream << _content;
        file.close();
        ++count;
    }
    // QApplication::restoreOverrideCursor();
    ui->labelSavedMsg->setText(tr("Saved %1 file(s) succeeded").arg(count));
    QTimer::singleShot(10000, ui->labelSavedMsg, &QLabel::clear);
}

void CodecConvertWidget::on_PushButtonSetDir_clicked()
{
    auto savedPath = QFileDialog::getExistingDirectory(this, tr("Choose the save path"));
    ui->lineEditDir->setText(savedPath);
}
