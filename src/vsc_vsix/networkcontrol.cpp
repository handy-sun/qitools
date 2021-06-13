#include "networkcontrol.h"
#include <QNetworkReply>
#include <QFileInfo>
#include <QDir>

NetworkControl::NetworkControl(QObject *parent)
    : QObject(parent)
    , httpRequestAborted(false)
    , reply(Q_NULLPTR)
    , m_writtenFile(Q_NULLPTR)
{
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

void NetworkControl::slot_startRequese(const QString &filePath, const QUrl &requestedUrl)
{
    if (!canWriteInFile(filePath))
         return;

    url = requestedUrl;
    httpRequestAborted = false;
    reply = qnam.get(QNetworkRequest(url));

    connect(reply, &QNetworkReply::finished, this, &NetworkControl::httpFinished);
    connect(reply, &QIODevice::readyRead, this, &NetworkControl::httpReadyRead);
    connect(reply, &QNetworkReply::downloadProgress, this, [=](qint64 recv, qint64 total)
    { Q_EMIT sig_downloadProgress(recv, total); });

    Q_EMIT sig_eventMessge("Begin download:" + m_writtenFile->fileName());
}

void NetworkControl::slot_cancelDownload()
{
    if (reply)
        reply->abort();
    httpRequestAborted = true;
}

void NetworkControl::httpFinished()
{
    QFileInfo fi;
    if (m_writtenFile)
    {
        if (!reply->error())
            Q_EMIT sig_eventMessge("Download finished.\n", true);
        fi.setFile(m_writtenFile->fileName());
        m_writtenFile->close();
        delete m_writtenFile;
        m_writtenFile = Q_NULLPTR;
    }

    if (httpRequestAborted)
    {
        Q_EMIT sig_eventMessge("Aborted!", true);
        reply->deleteLater();
        reply = Q_NULLPTR;
        return;
    }

    if (reply->error())
    {
        QFile::remove(fi.absoluteFilePath());
        Q_EMIT sig_eventMessge("Download failed: " + reply->errorString(), true);
        reply->deleteLater();
        reply = Q_NULLPTR;
        return;
    }

    const QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    reply->deleteLater();
    reply = Q_NULLPTR;

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
        m_writtenFile->write(reply->readAll());
}
