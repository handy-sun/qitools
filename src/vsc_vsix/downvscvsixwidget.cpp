#include "downvscvsixwidget.h"
#include "ui_downvscvsixwidget.h"
#include "networkcontrol.h"
#include "stable.h"


DownVscVsixWidget::DownVscVsixWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DownVscVsixWidget)
    , m_netDownloadCtrl(new NetworkControl(this))
    , m_fileSize(0)
{
    ui->setupUi(this);
    ui->lineEditWebUrl->setText("https://marketplace.visualstudio.com/items?itemName=VisualStudioExptTeam.vscodeintellicode");
    ui->lineEditVersion->setText("1.2.14");
    m_dowoloadDir = QDir(qApp->applicationDirPath());
    ui->labelRecvTotal->setText("");
    ui->lineEditFilePath->setText(m_dowoloadDir.absolutePath());
    ui->pushButtonShowToolBox->setCheckable(true);

    QFont font;
    font.setPixelSize(15);
    for (auto *le : findChildren<QLineEdit *>())
        le->setFont(font);

//    m_netDownloadCtrl->moveToThread(m_thread);
//    m_thread->start();
//    connect(m_thread, &QThread::finished, m_netDownloadCtrl, &NetworkControl::deleteLater);
    connect(this, &DownVscVsixWidget::sig_startRequeset, m_netDownloadCtrl, &NetworkControl::slot_startRequese);
    connect(this, &DownVscVsixWidget::sig_cancelDownload, m_netDownloadCtrl, &NetworkControl::slot_cancelDownload);
    connect(m_netDownloadCtrl, &NetworkControl::sig_downloadProgress, this, &DownVscVsixWidget::slot_downloadProgress);
    connect(m_netDownloadCtrl, &NetworkControl::sig_eventMessge, this, &DownVscVsixWidget::slot_eventMessge);

    connect(ui->pushButtonShowToolBox, &QPushButton::toggled, ui->toolBox, &QToolBox::setVisible);
    connect(ui->lineEditDownloadUrl, &QLineEdit::textChanged, this, [=]()
    {
        ui->pushButtonDownload->setEnabled(!ui->lineEditDownloadUrl->text().isEmpty());
    });
    connect(ui->lineEditDownloadUrl, &QLineEdit::textChanged, this, &DownVscVsixWidget::getFileSize);
    connect(ui->lineEditDownloadUrl, &QLineEdit::editingFinished, this, &DownVscVsixWidget::fillInFileName);

    connect(ui->lineEditFilePath, &QLineEdit::textChanged, this, [=](const QString &text)
    {
        m_defaultFileName = text;
        m_defaultFileName.remove(m_dowoloadDir.absolutePath() + "/");
    });

    ui->pushButtonShowToolBox->setChecked(true);
    ui->pushButtonDownload->setEnabled(false);
}

DownVscVsixWidget::~DownVscVsixWidget()
{
    delete ui;
//    if (m_thread->isRunning())
//    {
//        m_thread->quit();
//        qDebug() << "thread end:" <<m_thread->wait();
//    }
    //    delete m_netDownloadCtrl;
}

QString DownVscVsixWidget::getSuitableDecimalMeasure(quint64 bytes, double *outSize)
{
    long double sizeAsDouble = bytes;
    static QStringList measures;
    if (measures.isEmpty())
        measures << QCoreApplication::translate("QInstaller", "B")
                 << QCoreApplication::translate("QInstaller", "KB")
                 << QCoreApplication::translate("QInstaller", "MB")
                 << QCoreApplication::translate("QInstaller", "GB");
    QStringListIterator it(measures);
    QString measure(it.next());

    while (sizeAsDouble >= 1024.0 && it.hasNext())
    {
        measure = it.next();
        sizeAsDouble /= 1024.0;
    }
    *outSize = sizeAsDouble;
    return measure;
}

void DownVscVsixWidget::slot_downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    ui->progressBar->setMaximum(0 == bytesTotal ? 1 : bytesTotal);
    ui->progressBar->setValue(bytesReceived);

    qreal totalMB = (long double)bytesTotal / pow(1024, 2);
    qreal downMB = (long double)bytesReceived / pow(1024, 2);

    ui->labelRecvTotal->setText(QString("(%1 MB / %2 MB)").arg(downMB, 0, 'f', 2).arg(totalMB, 0, 'f', 2));
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

void DownVscVsixWidget::fillInFileName()
{
    if (m_defaultFileName.isEmpty())
    {
        QUrl u = QUrl::fromUserInput(ui->lineEditDownloadUrl->text().trimmed());
        if (u.fileName().isEmpty())
            m_defaultFileName = QDateTime::currentDateTime().toString("Untitled_yyyy-MM-dd-hh:mm:ss");
        else
            m_defaultFileName = u.fileName();

        ui->lineEditFilePath->setText(m_dowoloadDir.absoluteFilePath(m_defaultFileName));
    }
}

void DownVscVsixWidget::getFileSize()
{
    if (0 != m_fileSize)
        return;

    const QUrl inputUrl = QUrl::fromUserInput(ui->lineEditDownloadUrl->text().trimmed());
    if (!inputUrl.isValid())
        return;

    const uint tryTimes = 3;
    uint i;

    for (i = 0; i < tryTimes; ++i)
    {
        QNetworkAccessManager manager;
        QNetworkReply *headReply = manager.head(QNetworkRequest(inputUrl));
        if (!headReply)
            continue;

        QEventLoop loop;
        QTimer timer;

        connect(headReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

        timer.start(2000);
        loop.exec();

        if (headReply->error() != QNetworkReply::NoError)
        {
            qDebug() << headReply->errorString();
            continue;
        }
        if (!timer.isActive())
        {
            qDebug() << "Request Timeout"; // 请求超时超时,未获取到文件信息;
            continue;
        }
        timer.stop();

//        if (headReply->hasRawHeader(QString("Content-Length").toUtf8()))
//        {
//            qDebug() << headReply->rawHeader(QString("Content-Length").toUtf8());
//            m_fileSize = headReply->rawHeader(QString("Content-Length").toUtf8()).toULongLong();
//        } // 另一种方法
        QVariant var = headReply->header(QNetworkRequest::ContentLengthHeader);
        if (var.isValid())
        {
            m_fileSize = var.toULongLong();
            double newSize;
            auto strm = getSuitableDecimalMeasure(m_fileSize, &newSize);
            qDebug() << newSize << strm;
        }

        headReply->deleteLater();
        headReply = Q_NULLPTR;
        return;
    }
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
    if (ui->lineEditWebUrl->text().isEmpty() || ui->lineEditVersion->text().isEmpty())
    {
        QMessageBox::warning(this, "warning", QStringLiteral("两个输入框内容不能为空。"));
        return;
    }

    QStringList spList = ui->lineEditWebUrl->text().split('=');
    QStringList allName = spList.last().split('.');
    if (spList.size() != 2 && allName.size() != 2)
    {
        qDebug("Error: website is incorrect.");
        return;
    }

    QString publisher = allName.at(0);
    QString extension = allName.at(1);
    QString version = ui->lineEditVersion->text();
    m_defaultFileName = QString("%1.%2-%3.vsix").arg(publisher).arg(extension).arg(version);
    QString downloadUrl = QString("https://%1.gallery.vsassets.io/_apis/public/gallery/publisher/%1"
                                  "/extension/%2/%3/assetbyname"
                                  "/Microsoft.VisualStudio.Services.VSIXPackage")
                          .arg(publisher).arg(extension).arg(version);

//    ui->pushButtonGenVsixUrl->setText(fileName);
    ui->lineEditFilePath->setText(m_dowoloadDir.absoluteFilePath(m_defaultFileName));
    ui->lineEditDownloadUrl->setText(downloadUrl);
}

void DownVscVsixWidget::on_pushButtonGenVscUrl_clicked()
{
    QString cnUrl = ui->lineEditWebVscUrl->text();
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
    Q_EMIT sig_cancelDownload();
    ui->progressBar->setValue(0);
    ui->labelRecvTotal->setText("");
    m_fileSize = 0;
}

