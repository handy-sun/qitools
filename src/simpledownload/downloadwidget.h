#ifndef DOWNLOADWIDGET_H
#define DOWNLOADWIDGET_H

#include <QWidget>
#include <QDir>
#include "../core/pluginterface.h"

namespace Ui {
class DownloadWidget;
}

namespace SimpleDownload {

class NetworkControl;

class DownloadWidget : public QWidget, Core::PlugInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.sooncheer.QiTools.PlugInterface.049c19-1.0")
    Q_INTERFACES(Core::PlugInterface)

public:
    explicit DownloadWidget(QWidget *parent = nullptr);
    ~DownloadWidget();

    QString pluginName() const override { return tr("SimpleDownload"); }

    static QString getSuitableDecMeasure(quint64 bytes, double *outSize);

private:
    Ui::DownloadWidget *ui;

    NetworkControl        *m_netDownloadCtrl;
    QDir                   m_dowoloadDir;
    QString                m_defaultFileName;
    quint64                m_fileSize;
    int                    m_measureLevel;
    static QStringList     s_measures;

    static int getSuitableDecLevel(quint64 bytes, double *outSize);

Q_SIGNALS:
    void sig_getFileInfo(const QUrl &inputUrl, const QByteArray &headerNameBa);
    void sig_startRequeset(const QString &filePath, const QUrl &requestedUrl);
    void sig_cancelDownload();

public Q_SLOTS:
    void slot_downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void slot_eventMessge(const QString &msg, bool isEnable = false);
    void slot_requesetFileInfo(const QByteArray &ba);

private Q_SLOTS:
    void fillInFileName();
    void getFileSize();
    void on_pushButtonDownload_clicked();
    void on_pushButtonGenVsixUrl_clicked();

    void on_pushButtonGenVscUrl_clicked();
    void on_pushButtonOpenDir_clicked();
    void on_pushButtonCancel_clicked();
};

} // namespace SimpleDownload

#endif // DOWNLOADWIDGET_H
