#include <QAudioOutput>
#include <QCoreApplication>
#include <QDebug>
#include "audiodataplay.h"

AudioDataPlay::AudioDataPlay(AudioDataPlay::PlayMode mode, QObject *parent)
    : QObject(parent)
    , m_playMode(mode)
    , m_state(QAudio::StoppedState)
    , m_format(QAudioFormat())
    , m_outputDeviceInfo(QAudioDeviceInfo::defaultOutputDevice())
    , m_audioOutput(nullptr)
    , m_filledIODevice(nullptr)
    , m_playPosition(0)
    , m_linearVolume(1.0)
{
    QAudioFormat _format;
    _format.setSampleRate(44100);
    _format.setSampleSize(16);
    _format.setChannelCount(1);
    _format.setCodec("audio/pcm");
    _format.setByteOrder(QAudioFormat::LittleEndian);
    _format.setSampleType(QAudioFormat::SignedInt);
    setAudioFormat(_format);
    connect(&m_timerPull, &QTimer::timeout, this, &AudioDataPlay::onTimerPull);
}

int AudioDataPlay::bytesReadyRead() const
{
    if (!m_audioOutput)
        return -1;

    if (m_playMode == PlayMode::PullMode)
    {
        return m_baBuf.size() - m_bufferDevice.pos();
    }
    else if (m_playMode == PlayMode::PushMode)
    {
        return m_audioOutput->bytesFree();
    }
}

void AudioDataPlay::setAudioFormat(const QAudioFormat &format)
{
    if (m_outputDeviceInfo.isFormatSupported(format))
    {
        m_format = format;
    }
    else
    {
        qWarning() << "Default format not supported - trying to use nearest";
        m_format = m_outputDeviceInfo.nearestFormat(format);
    }
}

void AudioDataPlay::appendAudioData(const QByteArray &ba)
{
    if (m_playMode == PlayMode::PullMode)
    {
        //qDebug() << m_bufferDevice.pos() << m_baBuf.size() - m_bufferDevice.pos();
        if (m_baBuf.size() > m_format.bytesForDuration(20 * 1.0e6))
        {
            m_baBuf.remove(0, m_bufferDevice.pos());
            m_bufferDevice.seek(0);
        }
        m_baBuf.append(ba);
    }
}

void AudioDataPlay::startPlay()
{
    if (m_audioOutput)
    {
        if (QAudio::SuspendedState == m_state)
        {
#ifdef Q_OS_WIN
            // The Windows backend seems to internally go back into ActiveState while still returning SuspendedState,
            // so to ensure that it doesn't ignore the resume() call, we first re-suspend
            m_audioOutput->suspend();
#endif
            m_audioOutput->resume();
        }
        else
        {
            startAudio();
        }
    }
    else
    {
        m_audioOutput = new QAudioOutput(m_outputDeviceInfo, m_format, this);
        connect(m_audioOutput, &QAudioOutput::stateChanged, this, &AudioDataPlay::onStateChanged);
//        connect(m_audioOutput, &QAudioOutput::notify, this, [=](){ qDebug() << m_bufferDevice.pos() << m_baBuf.size(); });
        m_audioOutput->setNotifyInterval(200);
        startAudio();
    }
}

void AudioDataPlay::stopPlay()
{
    if (m_audioOutput)
    {
        m_timerPull.stop();
        m_audioOutput->stop();
        m_bufferDevice.close();
        QCoreApplication::instance()->processEvents();
        m_playPosition = 0;
    }
    resetAudio();
}

void AudioDataPlay::resumePlay()
{
    if (m_audioOutput)
    {
        if (m_playMode == PlayMode::PushMode)
            m_audioOutput->resume();
        else if (m_playMode == PlayMode::PullMode)
            m_timerPull.start();
    }
}

void AudioDataPlay::suspendPlay()
{
    if (m_audioOutput)
    {
        if (m_playMode == PlayMode::PushMode)
            m_audioOutput->suspend();
        else if (m_playMode == PlayMode::PullMode)
            m_timerPull.stop();
    }
}

void AudioDataPlay::seekTime(int t)
{
    if (t < 0)
    {
        if (m_bufferDevice.isOpen())
        {
            m_bufferDevice.seek(0);
            m_baBuf.clear();
        }
        return;
    }
    if (m_playMode == PlayMode::PushMode)
    {
        int pos = m_audioOutput->format().bytesForDuration(t * 1e6);
        if (pos < m_baBuf.size())
            m_bufferDevice.seek(pos);
    }
}

void AudioDataPlay::startAudio()
{
    m_bufferDevice.close();
    m_bufferDevice.setBuffer(&m_baBuf);
    m_bufferDevice.open(QIODevice::ReadOnly);

    if (m_playMode == PlayMode::PushMode)
    {
        m_audioOutput->start(&m_bufferDevice);
    }
    else if (m_playMode == PlayMode::PullMode)
    {
        m_filledIODevice = m_audioOutput->start();
        if (m_filledIODevice)
        {
            m_timerPull.start(40);
        }
        else
        {
            qDebug() << "QIODevice(m_filledIODevice) = QObject(0x0)" << m_audioOutput->state();
            m_bufferDevice.close();
            m_bufferDevice.open(QIODevice::ReadWrite);
            m_audioOutput->reset();
            m_audioOutput->start(&m_bufferDevice);
        }
    }
    m_audioOutput->setVolume(m_linearVolume);
}

void AudioDataPlay::resetAudio()
{
    if (m_audioOutput)
    {
        m_audioOutput->disconnect();
        delete m_audioOutput;
        m_audioOutput = nullptr;
    }
    m_baBuf.clear();
}

void AudioDataPlay::slot_setVolume(qreal vol)
{
    qreal linearVolume = QAudio::convertVolume(vol, QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
    m_linearVolume = linearVolume;
    if (m_audioOutput)
        m_audioOutput->setVolume(m_linearVolume);
}

void AudioDataPlay::onTimerPull()
{
    if (m_audioOutput->state() == QAudio::StoppedState)
        return;

    QByteArray buffer(32768, 0);
    int chunks = m_audioOutput->bytesFree() / m_audioOutput->periodSize();
    while (chunks)
    {
        const qint64 len = m_bufferDevice.read(buffer.data(), m_audioOutput->periodSize());
        if (len)
            m_filledIODevice->write(buffer.data(), len);

        if (len != m_audioOutput->periodSize())
            break;

        --chunks;
    }
}

void AudioDataPlay::onStateChanged(QAudio::State state)
{
    if (QAudio::IdleState == state)
    {
        if (!m_baBuf.isEmpty() && m_bufferDevice.pos() == m_baBuf.size())
            stopPlay();
//        qtout << "onStateChanged: stop" << m_bufferDevice.pos() << m_baBuf.size();
    }
    else
    {
        if (QAudio::StoppedState == state)
        {
            // Check error
            if (QAudio::NoError != m_audioOutput->error())
            {
                resetAudio();
                return;
            }
        }
    }
    m_state = state;
}
