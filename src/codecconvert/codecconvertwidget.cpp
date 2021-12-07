#include "codecconvertwidget.h"
#include "ui_codecconvertwidget.h"
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QTextCodec>
#include <QTimer>

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
    QTextCodec::ConverterState state;
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");

    codec->toUnicode(ba.constData(), ba.size(), &state);

    if (state.invalidChars == 0 || state.invalidChars == 6)
        return "Unicode";

    return "GBK";
}

void CodecConvertWidget::on_pushButtonOpen_clicked()
{
    QStringList _fileList = QFileDialog::getOpenFileNames(this, QStringLiteral("打开文件"), "", "All(*.*)");

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
    //    QApplication::setOverrideCursor(Qt::WaitCursor);
    int count = 0;
    for (const auto &targetFilePath : m_fileList)
    {
        QFile file(targetFilePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;

        QTextStream rwStream(&file);
        rwStream.setGenerateByteOrderMark(false);  // 不带bom的utf8 读
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
    //    QApplication::restoreOverrideCursor();
    ui->labelSavedMsg->setText(QStringLiteral("成功保存了 %1 个文件").arg(count));
    QTimer::singleShot(10000, ui->labelSavedMsg, &QLabel::clear);
}

void CodecConvertWidget::on_PushButtonSetDir_clicked()
{
    auto savedPath = QFileDialog::getExistingDirectory(this, QStringLiteral("选择保存路径"));
    ui->lineEditDir->setText(savedPath);
}
