#include <QAudioFormat>
#include <QTimer>
#include "audiowidget.h"
#include "ui_audiowidget.h"
#include "audiodataplay.h"
#include "wavfile.h"
#include "exmp3.h"
#include "stable.h"

static
//Q_DECL_CONSTEXPR const
constexpr int TotalHeadSize = sizeof(CombinedHeader) + sizeof(DATAHeader);

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
{
    ui->setupUi(this);
    ui->btnPlay->setIcon(QIcon(style()->standardPixmap(QStyle::SP_MediaPlay)));
    ui->btnStop->setIcon(QIcon(style()->standardPixmap(QStyle::SP_MediaStop)));

    m_timer->setTimerType(Qt::PreciseTimer);
    connect(m_timer, &QTimer::timeout, this, &AudioWidget::onTimerPull);

    m_te->moveToThread(&m_thread);
    m_thread.start();
    connect(&m_thread, &QThread::finished, m_te, &TestStream::deleteLater);
    connect(this, &AudioWidget::sig_startGet, m_te, &TestStream::slot_startGet);
    connect(this, &AudioWidget::sig_stopGet, m_te, &TestStream::slot_stopGet);
    connect(this, &AudioWidget::sig_preLoad, m_te, &TestStream::load);
    connect(m_te, &TestStream::sig_data, this, &AudioWidget::slot_handleData);
    connect(m_te, &TestStream::sig_duration, this, &AudioWidget::slot_setDuration);
    //initialize();
//    QTimer::singleShot(20, [=](){ Q_EMIT sig_preLoad(); });
}

AudioWidget::~AudioWidget()
{
    m_thread.quit();
    m_thread.wait();
    delete ui;
}

bool AudioWidget::initialize()
{
    WavFile wavFile;
    if (wavFile.open(QStringLiteral("澤野弘之 - DRAGON RISES.wav")))
    {
        if (wavFile.fileFormat().sampleType() == QAudioFormat::SignedInt
            && wavFile.fileFormat().sampleSize() == 16
            && wavFile.fileFormat().byteOrder() == QAudioFormat::LittleEndian)
        {
            m_baContent = wavFile.readAll();
            qtout << "fileSize:" << wavFile.size();
            m_baContent.remove(0, wavFile.headerLength());
            m_audioPlayer->setAudioFormat(wavFile.fileFormat());
            m_bytesPerSec = wavFile.fileFormat().sampleRate() * wavFile.fileFormat().channelCount() * wavFile.fileFormat().sampleSize() / 8;
            m_readBuffer.close();
            m_readBuffer.setBuffer(&m_baContent);
            m_readBuffer.open(QIODevice::ReadOnly);
//            if (m_audioPlayer->playMode() == AudioDataPlay::PushMode)
//                m_audioPlayer->setAudioData(m_baContent);
        }
        wavFile.close();
        return true;
    }
    else
        return false;
}

void AudioWidget::slot_setDuration(qint32 d)
{
    m_duration = d;
    ui->horizontalSliderProgress->setRange(0, m_duration);
    ui->labelProgress->setText("00:00/" + QTime(0, 0).addSecs(m_duration).toString("mm:ss"));
}

void AudioWidget::slot_handleData(const QByteArray &ba, int sign)
{
    if (sign == 0)
    {
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
    }
    else if (sign == 1)
    {
        m_audioPlayer->appendAudioData(ba);
        ++m_playTime;
    }
    ui->horizontalSliderProgress->setValue(m_playTime);
//    QTime timeEla;
    ui->labelProgress->setText(QTime(0, 0).addSecs(m_playTime).toString("mm:ss") + "/"
                               + QTime(0, 0).addSecs(m_duration).toString("mm:ss"));
}

void AudioWidget::onTimerPull()
{
    QByteArray baBlock = m_readBuffer.read(m_bytesPerSec * m_timer->interval() / 1000);
    m_audioPlayer->appendAudioData(baBlock);
    if (m_readBuffer.pos() > m_baContent.size() - m_bytesPerSec / 25)
    {
        m_timer->stop();
        m_audioPlayer->stopPlay();
        m_readBuffer.seek(0);
    }
}

void AudioWidget::on_btnPlay_clicked()
{
    if (m_audioPlayer->playMode() == AudioDataPlay::PushMode)
    {
        m_audioPlayer->startPlay();
    }
    else
    {
//        QByteArray baBlock = m_readBuffer.read(m_bytesPerSec);
//        m_timer->start(1000);
//        m_audioPlayer->startPlay();
//        m_audioPlayer->appendAudioData(baBlock);
        Q_EMIT sig_startGet();
    }
}

void AudioWidget::on_btnOpenFile_clicked()
{
    const QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("打开文件"), ""
                                                          ,"Sound Files(*.mp3 *.wav);;");
    if (fileName.isEmpty())
        return;

    Q_EMIT sig_preLoad(fileName);
    ui->lineEdit->setText(fileName);
}

void AudioWidget::on_btnStop_clicked()
{
    m_audioPlayer->stopPlay();
    Q_EMIT sig_stopGet();
}

/** ---------- @class TestStream ----------- */
TestStream::TestStream()
    : m_timer(new QTimer(this))
      , m_bytesPerSec(0)
{
    QObject::connect(m_timer, &QTimer::timeout, this, &TestStream::onTimer);
}

void TestStream::load(const QString &fileName)
{
    if (QFileInfo(fileName).suffix() == "mp3")
    {
        quint32 rate, totalCount, channels;
        QTime t(QTime::currentTime());
        qint16 *shBuf = DecodeToBuffer(fileName, &rate, &totalCount, &channels);
        //    qint16 *shBuf = DecodeToBuffer(QStringLiteral("Ring09.mp3"), &rate, &totalCount, &channels);
        qtout << "load:" << rate << totalCount << channels << "elapsed:" << t.elapsed();
        m_baContent.resize(totalCount * channels * 2);
        memcpy(m_baContent.data(), shBuf, m_baContent.size());
        free(shBuf);

        Q_EMIT sig_duration(int(m_baContent.size() / (rate * channels * 2)));

        QByteArray header(TotalHeadSize, 0);
        CombinedHeader ch;
        DATAHeader dh;
        memset(&ch, 0, sizeof(CombinedHeader));
        memset(&dh, 0, sizeof(DATAHeader));
        ch.wave.sampleRate = rate;
        ch.wave.numChannels = channels;
        ch.wave.bitsPerSample = 16;
        memcpy(header.data(), &ch, sizeof(CombinedHeader));
        dh.descriptor.size = m_baContent.size() + TotalHeadSize - 8;
        memcpy(header.data() + sizeof(CombinedHeader), &dh, sizeof(DATAHeader));
        m_baContent.prepend(header);

        m_bytesPerSec = rate * channels * 2;
        m_readBuffer.close();
        m_readBuffer.setBuffer(&m_baContent);
        m_readBuffer.open(QIODevice::ReadOnly);
    }
}

void TestStream::slot_startGet()
{
    if (m_baContent.isEmpty())
        return;

    onTimer();
    m_timer->start(1000);
}

void TestStream::slot_stopGet()
{
    m_timer->stop();
}

void TestStream::onTimer()
{
    if (m_readBuffer.pos() == 0)
    {
        QByteArray baBlock = m_readBuffer.read(TotalHeadSize + m_bytesPerSec);
        Q_EMIT sig_data(baBlock, 0);
        qtout << "start totalSize:" << m_baContent.size();
        return;
    }

    if (m_readBuffer.pos() < m_baContent.size() - m_bytesPerSec / 25)
    {
        QByteArray baBlock = m_readBuffer.read(m_bytesPerSec);
        Q_EMIT sig_data(baBlock, 1);
    }
    else
    {
        //Q_EMIT sig_data(baBlock, 2);
        qtout << "end position:" << m_readBuffer.pos();
        m_timer->stop();
        m_readBuffer.seek(0);
        m_readBuffer.close();
    }
}

void AudioWidget::on_verticalSliderVol_valueChanged(int value)
{
    m_audioPlayer->slot_setVolume(qreal(value) / 100.0);
}
