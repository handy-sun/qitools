#include <QSslSocket>
#include "downvscvsixwidget.h"
#include "ui_downvscvsixwidget.h"
#include "networkcontrol.h"
#include "stable.h"

QStringList DownVscVsixWidget::s_measures = { "B", "KB", "MB", "GB", "TB" };

DownVscVsixWidget::DownVscVsixWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DownVscVsixWidget)
    , m_netDownloadCtrl(new NetworkControl(this))
    , m_dowoloadDir(QDir(qApp->applicationDirPath()))
    , m_fileSize(0)
    , m_measureLevel(-1)
{
    ui->setupUi(this);
    ui->widgetDown->layout()->setMargin(3);
    ui->textEditWebUrl->setText("https://marketplace.visualstudio.com/items?itemName=GitHub.codespaces");
    ui->lineEditVersion->setText("1.1.0");
//    m_dowoloadDir = QDir(qApp->applicationDirPath());
    ui->labelRecvTotal->setText("");
    ui->lineEditFilePath->setText(m_dowoloadDir.absolutePath());
    ui->pushButtonShowToolBox->setCheckable(true);

    ui->textEditWebUrl->setWordWrapMode(QTextOption::WrapAnywhere);
    ui->textEditVscUrl->setWordWrapMode(QTextOption::WrapAnywhere);

    connect(this, &DownVscVsixWidget::sig_getFileInfo, m_netDownloadCtrl, &NetworkControl::slot_getFileInfo);
    connect(this, &DownVscVsixWidget::sig_startRequeset, m_netDownloadCtrl, &NetworkControl::slot_startRequese);
    connect(this, &DownVscVsixWidget::sig_cancelDownload, m_netDownloadCtrl, &NetworkControl::slot_cancelDownload);
    connect(m_netDownloadCtrl, &NetworkControl::sig_downloadProgress, this, &DownVscVsixWidget::slot_downloadProgress);
    connect(m_netDownloadCtrl, &NetworkControl::sig_eventMessge, this, &DownVscVsixWidget::slot_eventMessge);
    connect(m_netDownloadCtrl, &NetworkControl::sig_requesetFileInfo, this, &DownVscVsixWidget::slot_requesetFileInfo);

    connect(ui->pushButtonShowToolBox, &QPushButton::toggled, ui->tabWidgetTool, &QToolBox::setVisible);
    connect(ui->lineEditDownloadUrl, &QLineEdit::textChanged, this, [=]()
    {
        ui->pushButtonDownload->setEnabled(!ui->lineEditDownloadUrl->text().isEmpty());
    });
    connect(ui->lineEditDownloadUrl, &QLineEdit::textChanged, this, &DownVscVsixWidget::fillInFileName);
    connect(ui->lineEditDownloadUrl, &QLineEdit::textChanged, this, &DownVscVsixWidget::getFileSize);

    connect(ui->lineEditFilePath, &QLineEdit::textChanged, this, [=](const QString &text)
    {
        m_defaultFileName = text;
        m_defaultFileName.remove(m_dowoloadDir.absolutePath() + "/");
    });

    ui->pushButtonShowToolBox->setChecked(true);
    ui->pushButtonDownload->setEnabled(false);
    auto _font = ui->textBrowserMessage->font();
    _font.setFamily("MS Shell Dlg 2"); // Tahoma
    _font.setPointSize(13);
    ui->textBrowserMessage->setFont(_font);
#ifndef QT_NO_SSL
//    qInfo() << "isSupportsSSL:" << QSslSocket::supportsSsl() << QSslSocket::sslLibraryBuildVersionString();
    ui->textBrowserMessage->append(QStringLiteral("是否找到支持的SSL库: %1\n支持的SSL版本信息: <%2>")
                                   .arg(QSslSocket::supportsSsl() ? "true" : "false").arg(QSslSocket::sslLibraryBuildVersionString()));
#endif
    ui->textBrowserMessage->append(QStringLiteral("-------- 显示历次下载的信息 --------"));
}

DownVscVsixWidget::~DownVscVsixWidget()
{
    delete ui;
}

QString DownVscVsixWidget::getSuitableDecMeasure(quint64 bytes, double *outSize)
{
    long double sizeAsDouble = bytes;

    int level = 0;
    for (; sizeAsDouble >= 1024.0 && level < s_measures.size(); ++level)
    {
        sizeAsDouble /= 1024.0;
    }
    if (level >= s_measures.size())
        --level;

    *outSize = sizeAsDouble;
    return s_measures.at(level);
}

int DownVscVsixWidget::getSuitableDecLevel(quint64 bytes, double *outSize)
{
    long double sizeAsDouble = bytes;

    int level = 0;
    for (; sizeAsDouble >= 1024.0 && level < s_measures.size(); ++level)
    {
        sizeAsDouble /= 1024.0;
    }
    if (level >= s_measures.size())
        --level;

    *outSize = sizeAsDouble;
    return level;
}

void DownVscVsixWidget::slot_downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    ui->progressBar->setMaximum(0 == bytesTotal ? 1 : bytesTotal);
    ui->progressBar->setValue(bytesReceived);

    double suitableRecvSize;
    double suitableTotalSize;
    int recvLevel;
    int totalLevel;

    totalLevel = getSuitableDecLevel(bytesTotal, &suitableTotalSize);
    recvLevel = getSuitableDecLevel(bytesReceived, &suitableRecvSize);

    ui->labelRecvTotal->setText(QString("(%1 %2 / %3 %4)")
                                .arg(suitableRecvSize, 0, 'f', 2).arg(s_measures.at(recvLevel))
                                .arg(suitableTotalSize, 0, 'f', 2).arg(s_measures.at(totalLevel)));
}

void DownVscVsixWidget::slot_eventMessge(const QString &msg, bool isEnable)
{
    ui->textBrowserMessage->append(msg);
    ui->pushButtonDownload->setEnabled(isEnable);

    if (msg.contains("finished"))
    {
        m_fileSize = 0;
        if (ui->checkBoxRunApp->isChecked())
            QDesktopServices::openUrl(QUrl::fromLocalFile(m_dowoloadDir.absoluteFilePath(m_defaultFileName)));
    }
}

void DownVscVsixWidget::slot_requesetFileInfo(const QByteArray &ba)
{
    if (ba.isNull())
        return;

    m_fileSize = ba.toULongLong();
    double newSize;
    m_measureLevel = getSuitableDecLevel(m_fileSize, &newSize);
    ui->textBrowserMessage->append(QStringLiteral("已获取文件大小: %1 %2").arg(newSize, 0, 'f', 2).arg(s_measures.at(m_measureLevel)));
}

void DownVscVsixWidget::fillInFileName()
{
    QUrl u = QUrl::fromUserInput(ui->lineEditDownloadUrl->text().trimmed());
    if (u.fileName().isEmpty())
        m_defaultFileName = "Untitled_" + QDateTime::currentDateTime().toString("yyyy-MM-dd-hh:mm:ss");
    else
        m_defaultFileName = u.fileName();

    if (m_defaultFileName == "Microsoft.VisualStudio.Services.VSIXPackage")
        return;

    // NOTE: ui->lineEditFilePath TextChanged触发
    ui->lineEditFilePath->setText(m_dowoloadDir.absoluteFilePath(m_defaultFileName));
}

void DownVscVsixWidget::getFileSize()
{
    if (0 != m_fileSize)
        return;

    const QUrl inputUrl = QUrl::fromUserInput(ui->lineEditDownloadUrl->text().trimmed());
    Q_EMIT sig_getFileInfo(inputUrl, QString("Content-Length").toUtf8());
}

void DownVscVsixWidget::on_pushButtonDownload_clicked()
{
    if (ui->lineEditDownloadUrl->text().isEmpty() /*|| ui->lineEditFilePath->text().endsWith('/')*/)
    {
        QMessageBox::warning(this, "warning", QStringLiteral("下载地址为空"));
        return;
    }

    const QString strUrl = ui->lineEditDownloadUrl->text().trimmed();
    const QUrl dlUrl = QUrl::fromUserInput(strUrl);
    if (!dlUrl.isValid())
    {
        QMessageBox::information(this, "Error",  QStringLiteral("错误的 URL: %1: %2").arg(strUrl, dlUrl.errorString()));
        return;
    }

    if (m_defaultFileName.isEmpty())
        fillInFileName();

    QString filePath = ui->lineEditFilePath->text();
    if (QFile::exists(filePath))
    {
        if (QMessageBox::question(this, QStringLiteral("覆盖文件?"),
                                  QStringLiteral("该文件已存在，是否覆盖?").arg(filePath),
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
            == QMessageBox::No)
            return;
        QFile::remove(filePath);
    }

    ui->pushButtonDownload->setEnabled(false);
    Q_EMIT sig_startRequeset(filePath, dlUrl);
}

void DownVscVsixWidget::on_pushButtonGenVsixUrl_clicked()
{
    if (ui->textEditWebUrl->toPlainText().isEmpty() || ui->lineEditVersion->text().isEmpty())
    {
        QMessageBox::warning(this, "warning", QStringLiteral("两个输入框内容不能为空。"));
        return;
    }

    QStringList spList = ui->textEditWebUrl->toPlainText().split('=');
    QStringList allName = spList.last().split('.');
    if (spList.size() != 2 && allName.size() != 2)
    {
        qDebug("Error: website is incorrect.");
        return;
    }

    QString publisher = allName.at(0);
    QString extension = allName.at(1);
    QString version = ui->lineEditVersion->text();
    QString vsixName = QString("%1.%2-%3.vsix").arg(publisher).arg(extension).arg(version);
    m_defaultFileName = vsixName;
    QString downloadUrl = QString("https://%1.gallery.vsassets.io/_apis/public/gallery/publisher/%1"
                                  "/extension/%2/%3/assetbyname"
                                  "/Microsoft.VisualStudio.Services.VSIXPackage")
                          .arg(publisher).arg(extension).arg(version);

//    ui->pushButtonGenVsixUrl->setText(fileName);
    // NOTE: ui->lineEditDownloadUrl TextChanged触发
    ui->lineEditDownloadUrl->setText(downloadUrl);
    // NOTE: ui->lineEditFilePath TextChanged触发
    ui->lineEditFilePath->setText(m_dowoloadDir.absoluteFilePath(vsixName));
}

void DownVscVsixWidget::on_pushButtonGenVscUrl_clicked()
{
    QString cnUrl = ui->textEditVscUrl->toPlainText();
    cnUrl.replace("az764295.vo.msecnd.net", "vscode.cdn.azure.cn");
    m_defaultFileName = cnUrl.mid(cnUrl.lastIndexOf('/') + 1);
    ui->lineEditFilePath->setText(m_dowoloadDir.absoluteFilePath(m_defaultFileName));
    ui->lineEditDownloadUrl->setText(cnUrl);
}

void DownVscVsixWidget::on_pushButtonOpenDir_clicked()
{
    auto urlpath = QFileDialog::getExistingDirectory(this, QStringLiteral("选择保存路径"));
    m_dowoloadDir = QDir(urlpath);
    ui->lineEditFilePath->setText(m_dowoloadDir.absoluteFilePath(m_defaultFileName));
}

void DownVscVsixWidget::on_pushButtonCancel_clicked()
{
    if (QMessageBox::question(this, QStringLiteral("取消操作?"),
                              QStringLiteral("是否取消下载？ 取消后文件将删除"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
        == QMessageBox::No)
        return;

    Q_EMIT sig_cancelDownload();
    ui->progressBar->setValue(0);
    ui->labelRecvTotal->setText("");
    m_fileSize = 0;
}

