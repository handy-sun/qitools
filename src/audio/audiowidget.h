#ifndef AUDIOWIDGET_H
#define AUDIOWIDGET_H

#include <QWidget>
#include <QBuffer>
#include <QThread>
#include <QByteArray>

class AudioDataPlay;
class QAudioDecoder;

namespace Ui {
class AudioWidget;
}

// 模拟音频流发送端
class TestStream : public QObject
{
    Q_OBJECT
    QByteArray       m_baContent;
    QBuffer          m_readBuffer;
    QTimer          *m_timer;
    int              m_bytesPerSec;
public:
    explicit TestStream();
Q_SIGNALS:
    void sig_duration(qint32 d);
    void sig_data(const QByteArray &ba, int sign);
public Q_SLOTS:
    void load(const QString &fileName);
    void slot_startGet();
    void slot_stopGet();
private Q_SLOTS:
    void onTimer();
};

class AudioWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AudioWidget(QWidget *parent = nullptr);
    ~AudioWidget();

private:
    Ui::AudioWidget *ui;
    AudioDataPlay   *m_audioPlayer;
    QByteArray       m_baContent;
    QBuffer          m_readBuffer;
    QThread          m_thread;
    QTimer          *m_timer;
    TestStream      *m_te;
    quint32          m_contentPos;
    qint32           m_duration;
    qint32           m_playTime;
    int              m_bytesPerSec;

protected:
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void sig_startGet();
    void sig_stopGet();
    void sig_preLoad(const QString &fileName);

public Q_SLOTS:
    void slot_setDuration(qint32 d);
    void slot_handleData(const QByteArray &ba, int sign);


private Q_SLOTS:
    void onTimerPull();
    void on_btnPlay_clicked();
    void on_btnOpenFile_clicked();
    void on_btnStop_clicked();
    void on_vSliderVol_valueChanged(int value);
};

#endif // AUDIOWIDGET_H
