#ifndef AUDIODATAPLAY_H
#define AUDIODATAPLAY_H

#include <QObject>
#include <QAudioFormat>
#include <QBuffer>
#include <QByteArray>
#include <QTimer>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QAudioDevice>
#include <QMediaDevices>
class QAudioSink;
#else
#include <QAudioDeviceInfo>
class QAudioOutput;
#endif

namespace Audio {

class AudioDataPlay : public QObject
{
    Q_OBJECT
public:
    enum PlayMode
    {
        PushMode = 0,
        PullMode
    };
    AudioDataPlay(PlayMode mode = PushMode, QObject *parent = nullptr);

    PlayMode playMode() const { return m_playMode; }
    QAudio::State state() const { return m_state; }
    QAudioFormat audioFormat() const { return m_format; }
    bool isAudioValid() const { return m_audioOutput != nullptr; }
    int bytesReadyRead() const;

    void setAudioFormat(const QAudioFormat &format);
    void setAudioData(const QByteArray &ba) { m_baBuf = ba; }
    void appendAudioData(const QByteArray &ba);

    void startPlay();
    void stopPlay();
    void resumePlay();
    void suspendPlay();

    void resetAudio();
    void seekTime(int t = -1);

private:
    PlayMode            m_playMode;
    QAudio::State       m_state;
    QAudioFormat        m_format;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QAudioDevice        m_outputDeviceInfo;
    QAudioSink         *m_audioOutput;
#else
    QAudioDeviceInfo    m_outputDeviceInfo;
    QAudioOutput       *m_audioOutput;
#endif
    QBuffer             m_bufferDevice;
    QByteArray          m_baBuf;
    QIODevice          *m_filledIODevice;

    QTimer              m_timerPull;
    qint64              m_playPosition;
    qreal               m_linearVolume;

    void startAudio();

Q_SIGNALS:
    void sig_playedUSecs(int USecs);
    void sig_stateChanged(QAudio::State state);

public Q_SLOTS:
    void slot_setVolume(qreal vol);

private Q_SLOTS:
    void onTimerPull();
    void onStateChanged(QAudio::State state);
};

}

#endif // AUDIODATAPLAY_H
