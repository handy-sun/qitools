#include <QAudioFormat>
#include <QAudioDecoder>
#include <QTimer>
#include "audiowidget.h"
#include "ui_audiowidget.h"
#include "audiodataplay.h"
#include "wavfile.h"
#include "exmp3.h"
#include "downvscvsixwidget.h"
#include "stable.h"

static Q_DECL_CONSTEXPR const int TotalHeadSize = sizeof(CombinedHeader) + sizeof(DATAHeader);

AudioWidget::AudioWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AudioWidget)
    , m_audioPlayer(new AudioDataPlay(AudioDataPlay::PullMode, this))
    , m_timer(new QTimer(this))
    , m_te(new TestStream)
    , m_contentPos(0)
    , m_duration(0)
    , m_playTime(0)
    , m_bytesPerSec(0)
    , m_playbackState(0)
{
    ui->setupUi(this);
    ui->btnPlay->setIcon(QIcon(style()->standardIcon(QStyle::SP_MediaPlay)));
    ui->btnStop->setIcon(QIcon(style()->standardIcon(QStyle::SP_MediaStop)));
    ui->hSliderProgress->installEventFilter(this);
    m_timer->setTimerType(Qt::PreciseTimer);
    connect(m_timer, &QTimer::timeout, this, &AudioWidget::onTimerPull);

    m_te->moveToThread(&m_thread);
    m_thread.start();
    connect(&m_thread, &QThread::finished, m_te, &TestStream::deleteLater);
    connect(this, &AudioWidget::sig_playbackStateChanged, m_te, &TestStream::slot_playbackStateChanged);
//    connect(this, &AudioWidget::sig_stopGet, m_te, &TestStream::slot_stopGet);
    connect(this, &AudioWidget::sig_preLoad, m_te, &TestStream::load);
    connect(m_te, &TestStream::sig_data, this, &AudioWidget::slot_handleData);
    connect(m_te, &TestStream::sig_duration, this, &AudioWidget::slot_setDuration);
//    QTimer::singleShot(20, [=](){ Q_EMIT sig_preLoad(); });
    // 滑动条
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
    effect->setOffset(4, 4);
    effect->setColor(QColor(0, 0, 0, 50));
    effect->setBlurRadius(10);
    ui->hSliderProgress->setGraphicsEffect(effect);
}

AudioWidget::~AudioWidget()
{
    m_thread.quit();
    m_thread.wait();
    delete ui;
}

void AudioWidget::changeStateIcon(int state)
{
    if (m_playbackState == state)
        return;

    m_playbackState = state;
    if (m_playbackState == 1)
    {
        ui->btnPlay->setIcon(QIcon(style()->standardIcon(QStyle::SP_MediaPause)));
    }
    else if (m_playbackState == 2 || m_playbackState == 0)
    {
        ui->btnPlay->setIcon(QIcon(style()->standardIcon(QStyle::SP_MediaPlay)));
    }
}

bool AudioWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->hSliderProgress)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseMove)
        {
            QSlider *slider = static_cast<QSlider *>(watched);
            int range = slider->maximum() - slider->minimum();
            int pos = slider->minimum() + range * ((double)mouseEvent->x() / slider->width());
            if (pos != slider->sliderPosition())
            {
                slider->setValue(pos);
                return true;
            }
        }
    }
    return QObject::eventFilter(watched, event);
}

void AudioWidget::slot_setDuration(qint32 d)
{
    m_duration = d;
    m_playTime = 0;
    ui->hSliderProgress->setRange(0, m_duration);
    ui->labelProgress->setText("00:00/" + QTime(0, 0).addSecs(m_duration).toString("mm:ss"));
    ui->hSliderProgress->setValue(m_playTime);
}

void AudioWidget::slot_handleData(const QByteArray &ba, int sign)
{
    if (sign == 0) // 第一包包含wav头信息
    {
        Q_ASSERT(ba.size() > 0);

        CombinedHeader ch;
        DATAHeader dataH;
        memcpy(&ch, ba.constData(), sizeof(CombinedHeader));
        memcpy(&dataH, ba.constData() + sizeof(CombinedHeader), sizeof(DATAHeader));
        QAudioFormat _format;
        _format.setSampleRate(static_cast<int>(ch.wave.sampleRate));
        _format.setSampleSize(ch.wave.bitsPerSample);
        _format.setChannelCount(ch.wave.numChannels);
        _format.setCodec("audio/pcm");
        _format.setByteOrder(QAudioFormat::LittleEndian);
        _format.setSampleType(ch.wave.bitsPerSample == 8 ? QAudioFormat::UnSignedInt : QAudioFormat::SignedInt);

        m_playTime = 0;
        m_audioPlayer->setAudioFormat(_format);
        m_audioPlayer->startPlay();
        m_audioPlayer->appendAudioData(ba.mid(TotalHeadSize));
        changeStateIcon(1);
    }
    else if (sign == 1)
    {
        changeStateIcon(1);
        m_audioPlayer->appendAudioData(ba);
        ++m_playTime;
    }
    else if (sign == 2) // 播放结束
    {
        changeStateIcon(0);
        m_audioPlayer->stopPlay();
        slot_setDuration(m_duration);
    }
    ui->hSliderProgress->setValue(m_playTime);
    ui->labelProgress->setText(QTime(0, 0).addSecs(m_playTime).toString("mm:ss") + "/"
                               + QTime(0, 0).addSecs(m_duration).toString("mm:ss"));
}

void AudioWidget::onTimerPull()
{
//    QByteArray baBlock = m_readBuffer.read(m_bytesPerSec * m_timer->interval() / 1000);
//    m_audioPlayer->appendAudioData(baBlock);
//    if (m_readBuffer.pos() > m_baContent.size() - m_bytesPerSec / 25)
//    {
//        m_timer->stop();
//        m_audioPlayer->stopPlay();
//        m_readBuffer.seek(0);
//    }
}

void AudioWidget::on_btnPlay_clicked()
{
    if (!m_audioPlayer->isAudioValid())
        return;
//    if (m_audioPlayer->playMode() == AudioDataPlay::PushMode)
//    {
//        m_audioPlayer->startPlay();
//    }
//    else
    {
//        QByteArray baBlock = m_readBuffer.read(m_bytesPerSec);
//        m_timer->start(1000);
//        m_audioPlayer->startPlay();
//        m_audioPlayer->appendAudioData(baBlock);        
        if (m_playbackState == 1)
        {
            changeStateIcon(2);
            Q_EMIT sig_playbackStateChanged(m_playbackState);
            m_audioPlayer->suspendPlay();
        }
        else
        {
            int lastState = m_playbackState;
            changeStateIcon(1);
            Q_EMIT sig_playbackStateChanged(m_playbackState);
            if (lastState == 2)
                m_audioPlayer->resumePlay();
        }
    }
}

void AudioWidget::on_btnStop_clicked()
{
    m_audioPlayer->stopPlay();
    m_playbackState = 2;
    Q_EMIT sig_playbackStateChanged(m_playbackState);
    m_audioPlayer->resetAudio();
    ui->btnPlay->setIcon(QIcon(style()->standardIcon(QStyle::SP_MediaPlay)));
}

void AudioWidget::on_btnOpenFile_clicked()
{
#if 0 // 0 直接用指定文件名
    const QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("打开文件"), "",
        "MPEG Audio Layer 3(*.mp3);;"
        "Windows Media Audio(*wma);;"
        "Windows PCM(*.wav);;"
        "Free Lossless Audio Codec(*.flac);;"
//        "Monkey's Audio(*.ape);;"
        "All(*.*)"
        );
#else
    const QString fileName = qApp->applicationDirPath() + "/AmpedUp.wma"; // TEST
#endif
    if (fileName.isEmpty())
        return;

    Q_EMIT sig_preLoad(fileName);
    ui->lineEdit->setText(fileName);
}

/** ---------- @class TestStream ----------- */
TestStream::TestStream()
    : m_timer(new QTimer(this))
    , m_bytesPerSec(0)
{
    connect(m_timer, &QTimer::timeout, this, &TestStream::onTimer);
}

void TestStream::load(const QString &fileName)
{
    QFile file(fileName);
    if (!file.exists() || !file.open(QIODevice::ReadOnly) || file.size() < 10)
    {
        qtout << file.errorString() << file.size();
        return;
    }
    QByteArray head = file.peek(10);
    file.close();
    m_timer->stop();
    m_baContent.clear();
#ifdef Q_AUDIO_DECODER // QAudioDecoder 依赖系统自带的音频解码器，很多格式不支持，且效率一般
    QEventLoop loop;
    QSharedPointer<QAudioDecoder> decoder(new QAudioDecoder);
    connect(decoder.data(), &QAudioDecoder::bufferReady, [this, decoder]()
    {
        if (decoder->bufferAvailable())
        {
            QAudioBuffer buffer = decoder->read();
            m_baContent.append(buffer.constData<char>(), buffer.byteCount());
        }
    });
    connect(decoder.data(), &QAudioDecoder::finished, [this, decoder]()
    {
        QAudioFormat fmt = decoder->audioFormat();
        QByteArray header(TotalHeadSize, 0);
        WavFile::writeBaseHeader(header.data(), fmt.sampleRate(), fmt.sampleSize(), fmt.channelCount(), m_baContent.size());
        m_bytesPerSec = fmt.sampleRate() * fmt.channelCount() * 2;
        qtout << "format:" <<  fmt.sampleRate() << fmt.sampleSize() << fmt.channelCount();
        m_baContent.prepend(header);
    });
    connect(decoder.data(), &QAudioDecoder::finished, &loop, &QEventLoop::quit);
    connect(decoder.data(), QOverload<QAudioDecoder::Error>::of(&QAudioDecoder::error), [decoder](QAudioDecoder::Error error)
    { qtout << error << decoder->errorString();});
    connect(decoder.data(), QOverload<QAudioDecoder::Error>::of(&QAudioDecoder::error), &loop, &QEventLoop::quit);

    decoder->setSourceFilename(fileName);
    qInfo(">>> decoder start: <%s>", QFileInfo(fileName).fileName().toStdString().c_str());
    QTimer::singleShot(5000, &loop, &QEventLoop::quit); // 超时表示代表有可能解码失败，退出事件循环
    QTime tt(QTime::currentTime());
    decoder->start();
    loop.exec();

    double tmp;
    int el = tt.elapsed();
    QTime musicLen = QTime(0, 0, 0).addMSecs(decoder->duration());
    QString meas = DownVscVsixWidget::getSuitableDecMeasure(m_baContent.size(), &tmp);
    qInfo(">>> decoder finished, source_size: %.3f%s, duration: %s elapsed: %dms",
          tmp, meas.toStdString().c_str(), musicLen.toString("mm:ss.zzz").toStdString().c_str(), el);

    if (decoder->error() == QAudioDecoder::NoError && m_baContent.size() > TotalHeadSize)
    {
        Q_EMIT sig_duration(decoder->duration() / 1000); // 解析成功
        return;
    }
    else
    {
        qtout << "error message" << decoder->errorString() << m_baContent.size();
    }
#endif
    if (head.startsWith("ID3") || fileName.endsWith(".mp3"))
    {
        quint32 rate, totalCount, channels;
        QTime t(QTime::currentTime());
        qint16 *shBuf = DecodeToBuffer(fileName, &rate, &totalCount, &channels);
        //    qint16 *shBuf = DecodeToBuffer(QStringLiteral("Ring09.mp3"), &rate, &totalCount, &channels);
        qtout << "load ID3:" << rate << totalCount << channels << "elapsed:" << t.elapsed();
        m_baContent.resize(totalCount * channels * 2);
        memcpy(m_baContent.data(), shBuf, m_baContent.size());
        free(shBuf);
        m_bytesPerSec = rate * channels * 2;
        Q_EMIT sig_duration(m_baContent.size() / m_bytesPerSec);

        QByteArray header(TotalHeadSize, 0);
        WavFile::writeBaseHeader(header.data(), rate, sizeof(qint16) * 8, channels, m_baContent.size());
        m_baContent.prepend(header);

    }
    else if (head.startsWith("RIFF"))
    {
        WavFile wavFile;
        if (wavFile.open(fileName))
        {
            QDataStream out(&wavFile);
            wavFile.seek(0);
            m_baContent.resize(wavFile.size());
            out.readRawData(m_baContent.data(), wavFile.size());
            wavFile.close();
            m_bytesPerSec = wavFile.fileFormat().bytesForDuration(1e6);
            Q_EMIT sig_duration((m_baContent.size() - TotalHeadSize) / m_bytesPerSec);
            qtout << "load RIFF" << qreal(m_baContent.size() - TotalHeadSize) / m_bytesPerSec;
        }
    }
}

void TestStream::slot_playbackStateChanged(int state)
{
    switch (state)
    {
    case 0: // stop
        m_timer->stop();
        m_readBuffer.close();
        break;
    case 1: // play
        if (m_baContent.isEmpty())
            return;

        if (!m_readBuffer.isOpen())
        {
            m_readBuffer.setBuffer(&m_baContent);
            m_readBuffer.open(QIODevice::ReadOnly);
        }
        onTimer();
        m_timer->start(1000);
        break;
    case 2: // suspend
        m_timer->stop();
        break;
    default:
        break;
    }
}

void TestStream::onTimer()
{    
    if (m_readBuffer.pos() == 0)
    {
        QByteArray baBlock = m_readBuffer.read(TotalHeadSize + m_bytesPerSec);
        Q_EMIT sig_data(baBlock, 0);
        qtout << "start totalSize:" << m_baContent.size() << "firstSize:" << baBlock.size();
        return;
    }

    if (m_readBuffer.pos() < m_baContent.size() - m_bytesPerSec / 25)
    {
        QByteArray baBlock = m_readBuffer.read(m_bytesPerSec);
        Q_EMIT sig_data(baBlock, 1);
    }
    else
    {
        Q_EMIT sig_data(QByteArray(), 2);
        qtout << "end position:" << m_readBuffer.pos();
        m_timer->stop();
        m_readBuffer.close();
    }
}

void AudioWidget::on_vSliderVol_valueChanged(int value)
{
    m_audioPlayer->slot_setVolume(qreal(value) / 100.0);
}
