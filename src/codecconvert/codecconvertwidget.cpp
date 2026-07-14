#include "codecconvertwidget.h"
#include "multidirectorydialog.h"
#include "sourcefilefilter.h"
#include "ui_codecconvertwidget.h"
#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QTextStream>
#include <QDebug>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QPalette>
#include <QSignalBlocker>
#include <QTableWidget>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QStringConverter>
#else
#include <QTextCodec>
#endif
#include <QTimer>
#include <util/encodings/encodings.h>
#include <compact_enc_det/compact_enc_det.h>

#include <algorithm>

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

    QFont tableFont = qApp->font();
    if (tableFont.pointSizeF() > 0)
        tableFont.setPointSizeF(tableFont.pointSizeF() + 1.0);
    else if (tableFont.pixelSize() > 0)
        tableFont.setPixelSize(tableFont.pixelSize() + 1);
    ui->tableWidgetFiles->setFont(tableFont);

    QPalette tablePalette = ui->tableWidgetFiles->palette();
    const QColor baseColor = tablePalette.color(QPalette::Base);
    tablePalette.setColor(QPalette::AlternateBase,
                          baseColor.lightness() < 128 ? baseColor.lighter(115)
                                                      : baseColor.darker(106));
    ui->tableWidgetFiles->setPalette(tablePalette);

    ui->tableWidgetFiles->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    QTimer::singleShot(0, this, [this]() {
        constexpr int encodingColumnWidth = 120;
        ui->tableWidgetFiles->setColumnWidth(
            0, qMax(200, ui->tableWidgetFiles->viewport()->width() - encodingColumnWidth));
        ui->tableWidgetFiles->setColumnWidth(1, encodingColumnWidth);
    });
    ui->tableWidgetFiles->verticalHeader()->setDefaultSectionSize(34);
    ui->tableWidgetFiles->verticalHeader()->setVisible(false);
    ui->checkBoxSelectAll->setTristate(true);
    ui->pushButtonConvert->setEnabled(false);
    ui->pushButtonRemove->setEnabled(false);

    connect(ui->radioButtonSettingPath, &QRadioButton::toggled, [=](bool checked) {
        ui->lineEditDir->setEnabled(checked);
        ui->PushButtonSetDir->setEnabled(checked);
    });
    connect(ui->tableWidgetFiles->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, [this]() {
        ui->pushButtonRemove->setEnabled(
            !ui->tableWidgetFiles->selectionModel()->selectedRows().isEmpty());
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

QStringList CodecConvertWidget::sourceFileNameFilters() const
{
    QStringList nameFilters;
    QStringList allPatterns;

    for (const auto &filter : SourceFileFilter::languageFilters())
    {
        QStringList patterns;
        for (const QString &suffix : filter.suffixes)
            patterns.append("*." + suffix);
        allPatterns.append(patterns);
        nameFilters.append(QString("%1 (%2)").arg(filter.name, patterns.join(' ')));
    }

    nameFilters.prepend(tr("Supported source files (%1)").arg(allPatterns.join(' ')));
    return nameFilters;
}

QStringList CodecConvertWidget::checkedFiles() const
{
    QStringList files;
    for (int row = 0; row < ui->tableWidgetFiles->rowCount(); ++row)
    {
        const auto *fileItem = ui->tableWidgetFiles->item(row, 0);
        if (fileItem && fileItem->checkState() == Qt::Checked)
            files.append(fileItem->data(Qt::UserRole).toString());
    }
    return files;
}

void CodecConvertWidget::appendFiles(const QStringList &filePaths)
{
    const QSignalBlocker blocker(ui->tableWidgetFiles);
    for (const QString &filePath : filePaths)
    {
        const QFileInfo fileInfo(filePath);
        if (!fileInfo.exists() || !fileInfo.isFile())
            continue;

        QString normalizedPath = fileInfo.canonicalFilePath();
        if (normalizedPath.isEmpty())
            normalizedPath = fileInfo.absoluteFilePath();
        normalizedPath = QDir::cleanPath(normalizedPath);
        if (m_filePaths.contains(normalizedPath))
            continue;

        QFile file(normalizedPath);
        const QString encoding = file.open(QIODevice::ReadOnly)
            ? getCodeString(file.readAll())
            : tr("Read failed");

        const int row = ui->tableWidgetFiles->rowCount();
        ui->tableWidgetFiles->insertRow(row);

        auto *fileItem = new QTableWidgetItem(normalizedPath);
        fileItem->setFlags(fileItem->flags() | Qt::ItemIsUserCheckable);
        fileItem->setCheckState(Qt::Checked);
        fileItem->setData(Qt::UserRole, normalizedPath);
        fileItem->setToolTip(normalizedPath);
        ui->tableWidgetFiles->setItem(row, 0, fileItem);

        auto *encodingItem = new QTableWidgetItem(encoding);
        encodingItem->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetFiles->setItem(row, 1, encodingItem);
        m_filePaths.insert(normalizedPath);
    }

    updateSelectionState();
}

void CodecConvertWidget::updateSelectionState()
{
    const int rowCount = ui->tableWidgetFiles->rowCount();
    int checkedCount = 0;
    for (int row = 0; row < rowCount; ++row)
    {
        const auto *item = ui->tableWidgetFiles->item(row, 0);
        if (item && item->checkState() == Qt::Checked)
            ++checkedCount;
    }

    ui->pushButtonConvert->setEnabled(checkedCount > 0);

    const QSignalBlocker blocker(ui->checkBoxSelectAll);
    if (checkedCount == 0)
        ui->checkBoxSelectAll->setCheckState(Qt::Unchecked);
    else if (checkedCount == rowCount)
        ui->checkBoxSelectAll->setCheckState(Qt::Checked);
    else
        ui->checkBoxSelectAll->setCheckState(Qt::PartiallyChecked);
}

void CodecConvertWidget::on_pushButtonAddFiles_clicked()
{
    const QStringList fileList = QFileDialog::getOpenFileNames(
        this, tr("Add source files"), QString(), sourceFileNameFilters().join(";;"));

    appendFiles(fileList);
}

void CodecConvertWidget::on_pushButtonAddDirectories_clicked()
{
    MultiDirectoryDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    const QStringList files = SourceFileFilter::scanDirectories(
        dialog.selectedDirectories(), dialog.selectedSuffixes());
    appendFiles(files);
    QApplication::restoreOverrideCursor();
}

void CodecConvertWidget::on_pushButtonRemove_clicked()
{
    auto rows = ui->tableWidgetFiles->selectionModel()->selectedRows();
    std::sort(rows.begin(), rows.end(), [](const QModelIndex &left, const QModelIndex &right) {
        return left.row() > right.row();
    });

    for (const QModelIndex &index : rows)
    {
        const auto *item = ui->tableWidgetFiles->item(index.row(), 0);
        if (item)
            m_filePaths.remove(item->data(Qt::UserRole).toString());
        ui->tableWidgetFiles->removeRow(index.row());
    }

    updateSelectionState();
}

void CodecConvertWidget::on_checkBoxSelectAll_clicked(bool checked)
{
    const QSignalBlocker blocker(ui->tableWidgetFiles);
    for (int row = 0; row < ui->tableWidgetFiles->rowCount(); ++row)
    {
        auto *item = ui->tableWidgetFiles->item(row, 0);
        if (item)
            item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
    }
    updateSelectionState();
}

void CodecConvertWidget::on_tableWidgetFiles_itemChanged(QTableWidgetItem *item)
{
    if (item && item->column() == 0)
        updateSelectionState();
}

void CodecConvertWidget::on_pushButtonConvert_clicked()
{
    // QApplication::setOverrideCursor(Qt::WaitCursor);
    int count = 0;
    for (const auto &targetFilePath : checkedFiles())
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        auto enc = QStringConverter::encodingForName(ui->comboBoxCodec->currentText().toUtf8().constData());
        rwStream.setEncoding(enc.value_or(QStringConverter::Utf8));
#else
        const QByteArray codecName = ui->comboBoxCodec->currentText().toUtf8();
        QTextCodec *codec = codecName.compare("System", Qt::CaseInsensitive) == 0
            ? QTextCodec::codecForLocale()
            : QTextCodec::codecForName(codecName);
        rwStream.setCodec(codec ? codec : QTextCodec::codecForName("UTF-8"));
#endif
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
