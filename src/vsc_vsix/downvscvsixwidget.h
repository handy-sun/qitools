#ifndef DOWNVSCVSIXWIDGET_H
#define DOWNVSCVSIXWIDGET_H

#include <QWidget>
#include <QDir>

namespace Ui {
class DownVscVsixWidget;
}

class NetworkControl;

class DownVscVsixWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DownVscVsixWidget(QWidget *parent = Q_NULLPTR);
    ~DownVscVsixWidget();

private:
    Ui::DownVscVsixWidget *ui;

    NetworkControl        *m_netDownloadCtrl;
    QDir                   m_dowoloadDir;
    QString                m_defaultFileName;
    quint64                m_fileSize;

    QString getSuitableDecimalMeasure(quint64 bytes, double *outSize);

Q_SIGNALS:
//    void sig_startRequeset(const QUrl &requestedUrl);
    void sig_startRequeset(const QString &filePath, const QUrl &requestedUrl);
    void sig_cancelDownload();

public Q_SLOTS:
    void slot_downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void slot_eventMessge(const QString &msg, bool isEnable = false);

private Q_SLOTS:
    void fillInFileName();
    void getFileSize();
    void on_pushButtonDownload_clicked();
    void on_pushButtonGenVsixUrl_clicked();

    void on_pushButtonGenVscUrl_clicked();
    void on_pushButtonOpenDir_clicked();
    void on_pushButtonCancel_clicked();
};

#endif // DOWNVSCVSIXWIDGET_H
