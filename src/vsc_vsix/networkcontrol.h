#ifndef NETWORKCONTROL_H
#define NETWORKCONTROL_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>

QT_BEGIN_NAMESPACE
class QFile;
class QAuthenticator;
class QNetworkReply;
QT_END_NAMESPACE

class NetworkControl : public QObject
{
    Q_OBJECT
public:
    explicit NetworkControl(QObject *parent = nullptr);
    ~NetworkControl();

    bool canWriteInFile(const QString &fileName);

private:
    bool m_isHttpRequestAborted;
    QUrl url;
    QNetworkAccessManager *m_netAcManager;
    QNetworkReply *m_netReply;
    QFile *m_writtenFile;

Q_SIGNALS:
    void sig_requesetFileInfo(const QByteArray &ba);
    void sig_downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void sig_eventMessge(const QString &msg, bool isEnable = false);

public Q_SLOTS:
    void slot_getFileInfo(const QUrl &inputUrl, const QByteArray &headerNameBa);
    void slot_startRequese(const QString &filePath, const QUrl &requestedUrl);
    void slot_cancelDownload();

private Q_SLOTS:
    void httpFinished();
    void httpReadyRead();

};

#endif // NETWORKCONTROL_H
