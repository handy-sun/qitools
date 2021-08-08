#ifndef AUDIODATAPLAY_H
#define AUDIODATAPLAY_H

#include <QObject>
#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QBuffer>
#include <QByteArray>
#include <QTimer>

QT_BEGIN_NAMESPACE
class QAudioOutput;
QT_END_NAMESPACE

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

    void setAudioFormat(const QAudioFormat &format);// 设置音频格式(有缺省格式)，需在设置数据前设置格式
    void setAudioData(const QByteArray &ba) { m_baBuf = ba; } // PushMode时设置完整的数据
    void appendAudioData(const QByteArray &ba); // PullMode时定期填充新的数据

    void startPlay();
    void stopPlay();
    void resumePlay();
    void suspendPlay();
    void seekTime(int t = -1); // 定位播放时间

private:
    PlayMode            m_playMode;
    QAudio::State       m_state;
    QAudioFormat        m_format;

    QAudioDeviceInfo    m_outputDeviceInfo;
    QAudioOutput       *m_audioOutput;
    QBuffer             m_bufferDevice;
    QByteArray          m_baBuf;
    QIODevice          *m_filledIODevice;

    QTimer              m_timerPull;
    qint64              m_playPosition;
    qreal               m_linearVolume;

    void startAudio();
    void resetAudio();

public Q_SLOTS:
    void slot_setVolume(qreal vol);

private Q_SLOTS:
    void onTimerPull();
    void onStateChanged(QAudio::State state);
};

#endif // AUDIODATAPLAY_H
