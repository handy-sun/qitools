#include <QAudioOutput>
#include "audiodataplay.h"

AudioDataPlay::AudioDataPlay(AudioDataPlay::PlayMode mode, QObject *parent)
    : QObject(parent)
    , m_playMode(mode)
    , m_format(QAudioFormat())
    , m_audioOutputDevice(QAudioDeviceInfo::defaultOutputDevice())
    , m_audioOutput(nullptr)
    , m_filledIODevice(nullptr)
    , m_playPosition(0)
    , m_linearVolume(1.0)
{
    m_format.setSampleRate(44100);
    m_format.setSampleSize(16);
    m_format.setChannelCount(1);
    m_format.setCodec("audio/pcm");
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    m_format.setSampleType(QAudioFormat::SignedInt);

    connect(&m_timerPull, &QTimer::timeout, this, &AudioDataPlay::onTimerPull);
}

void AudioDataPlay::appendAudioData(const QByteArray &ba)
{
    if (m_playMode == PullMode && m_filledIODevice)
        m_baBuf.append(ba);
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
        m_audioOutput = new QAudioOutput(m_audioOutputDevice, m_format, this);
        //QObject::connect(m_audioOutput, &QAudioOutput::stateChanged, this, &AudioDataPlay::onStateChanged);
        m_audioOutput->setNotifyInterval(100);
        startAudio();
    }
}

void AudioDataPlay::stopPlay()
{
    if (m_audioOutput)
    {
        m_timerPull.stop();
        m_audioOutput->stop();
        m_bufferDevice.seek(0);
        m_bufferDevice.close();
        QCoreApplication::instance()->processEvents();
        m_playPosition = 0;
        if (m_playMode == PlayMode::PullMode)
            m_baBuf.clear();
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
        m_timerPull.start(40);
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
}

void AudioDataPlay::slot_setVolume(qreal vol)
{
    qreal linearVolume = QAudio::convertVolume(vol,
                                               QAudio::LogarithmicVolumeScale,
                                               QAudio::LinearVolumeScale);
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
    if (QAudio::IdleState == state && m_bufferDevice.pos() >= m_baBuf.size() - 1)
    {
        stopPlay();
        qtout << "onStateChanged: stop" << m_bufferDevice.pos() << m_baBuf.size();
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
