﻿#ifndef NETWORKCONTROL_H
#define NETWORKCONTROL_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QDir>

QT_BEGIN_NAMESPACE
class QFile;
//class QSslError;
class QAuthenticator;
class QNetworkReply;
QT_END_NAMESPACE

class NetworkControl : public QObject
{
    Q_OBJECT
public:
    explicit NetworkControl(QObject *parent = Q_NULLPTR);
    ~NetworkControl();

    bool canWriteInFile(const QString &fileName);

private:
    bool httpRequestAborted;
    QUrl url;
    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    QFile *m_writtenFile;

Q_SIGNALS:
    void sig_downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void sig_eventMessge(const QString &msg, bool isEnable = false);

public Q_SLOTS:
    void slot_startRequese(const QString &filePath, const QUrl &requestedUrl);
    void slot_cancelDownload();

private Q_SLOTS:
    void httpFinished();
    void httpReadyRead();

};

#endif // NETWORKCONTROL_H
