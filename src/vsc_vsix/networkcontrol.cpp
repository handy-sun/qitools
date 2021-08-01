#include "networkcontrol.h"
#include <QNetworkReply>
#include <QFileInfo>
#include <QEventLoop>
//#include <QSslSocket>
#include <QTimer>
#include <QDir>

NetworkControl::NetworkControl(QObject *parent)
    : QObject(parent)
    , m_isHttpRequestAborted(false)
    , m_netAcManager(new QNetworkAccessManager(this))
    , m_netReply(Q_NULLPTR)
    , m_writtenFile(Q_NULLPTR)
{
    qDebug() << QSslSocket::supportsSsl() << QSslSocket::sslLibraryBuildVersionString();
}

NetworkControl::~NetworkControl()
{

}

bool NetworkControl::canWriteInFile(const QString &fileName)
{
    QScopedPointer<QFile> _file(new QFile(fileName));
    if (!_file->open(QIODevice::WriteOnly))
    {
        Q_EMIT sig_eventMessge(QString("Unable to save the file %1: %2.").arg(QDir::toNativeSeparators(fileName), _file->errorString()));
        return false;
    }
    m_writtenFile = _file.take();
    return true;
}

void NetworkControl::slot_getFileInfo(const QUrl &inputUrl, const QByteArray &headerNameBa)
{
    if (!inputUrl.isValid())
        return;

    const int tryTimes = 1;
    int i;

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
            Q_EMIT sig_eventMessge(headReply->errorString(), true);
            continue;
        }
        if (!timer.isActive())
        {
            Q_EMIT sig_eventMessge("Request Timeout", true); // 请求超时,未获取到文件信息;
            continue;
        }
        timer.stop();

        if (headReply->hasRawHeader(headerNameBa))
            Q_EMIT sig_requesetFileInfo(headReply->rawHeader(headerNameBa));

        headReply->deleteLater();
        headReply = Q_NULLPTR;
        return;
    }
}

void NetworkControl::slot_startRequese(const QString &filePath, const QUrl &requestedUrl)
{
    if (!canWriteInFile(filePath))
         return;

    url = requestedUrl;
    m_isHttpRequestAborted = false;
    m_netReply = m_netAcManager->get(QNetworkRequest(url));

    connect(m_netReply, &QNetworkReply::finished, this, &NetworkControl::httpFinished);
    connect(m_netReply, &QIODevice::readyRead, this, &NetworkControl::httpReadyRead);
    connect(m_netReply, &QNetworkReply::downloadProgress, this, [=](qint64 recv, qint64 total)
    { Q_EMIT sig_downloadProgress(recv, total); });

    Q_EMIT sig_eventMessge(QStringLiteral("Begin download: \"%1\"").arg(m_writtenFile->fileName()));
}

void NetworkControl::slot_cancelDownload()
{
    if (m_netReply)
        m_netReply->abort();
    m_isHttpRequestAborted = true;
}

void NetworkControl::httpFinished()
{
    QFileInfo fi;
    if (m_writtenFile)
    {
        if (!m_netReply->error())
            Q_EMIT sig_eventMessge("Download finished.\n", true);
        fi.setFile(m_writtenFile->fileName());
        m_writtenFile->close();
        delete m_writtenFile;
        m_writtenFile = Q_NULLPTR;
    }

    if (m_isHttpRequestAborted)
    {
        Q_EMIT sig_eventMessge("Aborted!", true);
        m_netReply->deleteLater();
        m_netReply = Q_NULLPTR;
        return;
    }

    if (m_netReply->error())
    {
        QFile::remove(fi.absoluteFilePath());
        Q_EMIT sig_eventMessge("Download failed: " + m_netReply->errorString() + "\n", true);
        m_netReply->deleteLater();
        m_netReply = Q_NULLPTR;
        return;
    }

    const QVariant redirectionTarget = m_netReply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    m_netReply->deleteLater();
    m_netReply = Q_NULLPTR;

    if (!redirectionTarget.isNull())
    {
        const QUrl redirectedUrl = url.resolved(redirectionTarget.toUrl());
        Q_EMIT sig_eventMessge(QString("Redirect to %1.").arg(redirectedUrl.toString()), true);

        slot_startRequese(fi.absoluteFilePath(), redirectedUrl);
    }
}

void NetworkControl::httpReadyRead()
{
    if (m_writtenFile)
        m_writtenFile->write(m_netReply->readAll());
}
