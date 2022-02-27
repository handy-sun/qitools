#include "audiowidget.h"
#include "ui_audiowidget.h"
#include "audiodataplay.h"
#include "wavfile.h"
#include "minimp3/exmp3.h"

#include <QAudioFormat>
#include <QAudioDecoder>
#include <QTimer>
#include <QDebug>
#include <QStyle>
#include <QMouseEvent>
#include <QPainter>
#include <QDir>
#include <QFileDialog>
#include <QTextCodec>
#include <QElapsedTimer>

static Q_DECL_CONSTEXPR const int TotalHeadSize = sizeof(CombinedHeader) + sizeof(DATAHeader);

using namespace Audio;

AudioWidget::AudioWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AudioWidget)
    , m_audioPlayer(new AudioDataPlay(AudioDataPlay::PushMode, this))
    , m_te(new TestStream)
    , m_duration(0)
    , m_playbackState(0)
    , m_whichImage(0)
{
    ui->setupUi(this);
    ui->btnPlay->setIcon(QIcon(style()->standardIcon(QStyle::SP_MediaPlay)));
    ui->btnStop->setIcon(QIcon(style()->standardIcon(QStyle::SP_MediaStop)));
    ui->btnVol->setIcon(QIcon(style()->standardIcon(QStyle::SP_MediaVolume)));
    ui->btnOpenFile->setIcon(QIcon(style()->standardIcon(QStyle::SP_DialogOpenButton)));
    ui->btnLoadImage->setIcon(QIcon(style()->standardIcon(QStyle::SP_ComputerIcon)));
    ui->btnLoadImage->setToolTip(QStringLiteral("加载要作为新封面的图片"));
    ui->btnSaveNew->setIcon(QIcon(style()->standardIcon(QStyle::SP_DriveFDIcon)));
    ui->btnSaveNew->setToolTip(QStringLiteral("保存为带新封面的文件"));
    ui->btnSaveCover->setIcon(QIcon(style()->standardIcon(QStyle::SP_DialogSaveButton)));
    ui->btnSaveCover->setToolTip(QStringLiteral("保存封面"));
    ui->sliderProgress->setRange(0, 0);
    ui->sliderProgress->setTracking(false);
    ui->sliderProgress->installEventFilter(this);

    m_te->moveToThread(&m_thread);
    m_thread.start();
    connect(&m_thread, &QThread::finished, m_te, &TestStream::deleteLater);
    connect(this, &AudioWidget::sig_preLoad, m_te, &TestStream::load);
    connect(m_te, &TestStream::sig_readyData, this, &AudioWidget::slot_readyData);
    connect(m_te, &TestStream::sig_duration, this, &AudioWidget::slot_setDuration);
    connect(m_audioPlayer, &AudioDataPlay::sig_playedUSecs, this, &AudioWidget::slot_playedUSecs);
    connect(m_audioPlayer, &AudioDataPlay::sig_stateChanged, this, &AudioWidget::slot_stateChanged);

    connect(ui->sliderProgress, &QSlider::sliderReleased, this, [=](){
        if (m_audioPlayer->state() != QAudio::ActiveState)
            m_audioPlayer->startPlay();
        m_audioPlayer->seekTime(ui->sliderProgress->value());
    });
}

AudioWidget::~AudioWidget()
{
    m_audioPlayer->stopPlay();
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
    if (watched == ui->sliderProgress)
    {
        if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseMove)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            QSlider *slider = qobject_cast<QSlider *>(watched);
            qreal range = slider->maximum() - slider->minimum();
            qreal totalWH;
            qreal ctrlWH;
            qreal mousePosF;
            if (Qt::Horizontal == slider->orientation())
            {
                totalWH = slider->width();
                ctrlWH = slider->minimumSizeHint().width();
                mousePosF = slider->invertedAppearance() ? totalWH - mouseEvent->localPos().x()
                                                         : mouseEvent->localPos().x();
            }
            else
            {
                totalWH = slider->height();
                ctrlWH = slider->minimumSizeHint().height();
                mousePosF = slider->invertedAppearance() ? totalWH - mouseEvent->localPos().y()
                                                         : mouseEvent->localPos().y();
            }

            if (totalWH <= ctrlWH)
                goto otherFilter;

            int accuratePos = slider->minimum() +
                              range * (mousePosF - ctrlWH / 2.0) / (totalWH - ctrlWH);

            if (accuratePos != slider->sliderPosition()
                && accuratePos >= slider->minimum()
                && accuratePos <= slider->maximum())
            {
                slider->setValue(accuratePos);
#ifdef SLIDERDOWN_WHEN_DRAGGING
                if (mouseEvent->type() == QEvent::MouseMove && mouseEvent->buttons() & Qt::LeftButton)
                {
                    ui->labelPlayedTime->setText(QTime(0, 0).addSecs(accuratePos).toString("m:ss"));
                    slider->setSliderDown(true);
                }
#endif
            }
        }
    }
otherFilter:
    return QObject::eventFilter(watched, event);
}

void AudioWidget::paintEvent(QPaintEvent *)
{
    const QImage &targetImage = m_whichImage == 0 ? m_coverImage : m_shallAddImage;
    const QRect availableRect = ui->labelImage->geometry();
    if (targetImage.isNull())
        return;

    QPainter p(this);
    QRect imgRect = targetImage.rect();
    if (imgRect.height() > availableRect.height())
        imgRect.setSize(QSize(imgRect.width() * availableRect.height() / imgRect.height(), availableRect.height()));

    imgRect.moveCenter(availableRect.center());

    p.drawImage(imgRect, targetImage);
    p.end();
}

void AudioWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_F8:
        m_whichImage = 1 - m_whichImage;
        update();
        break;
    default:
        break;
    }
}

void AudioWidget::slot_setDuration(qint32 d)
{
    m_duration = d;
    ui->sliderProgress->setRange(0, m_duration);
    ui->labelTotalTime->setText(QTime(0, 0).addSecs(m_duration).toString("m:ss"));
    ui->labelPlayedTime->setText("0:00");
    ui->sliderProgress->setValue(0);
}

void AudioWidget::slot_readyData(const QByteArray &header, const QByteArray &audioData)
{
    Q_ASSERT(audioData.size() > 0);

    CombinedHeader ch;
    DATAHeader dataH;
    memcpy(&ch, header.constData(), sizeof(CombinedHeader));
    memcpy(&dataH, header.constData() + sizeof(CombinedHeader), sizeof(DATAHeader));

    QAudioFormat _format;
    _format.setSampleRate(static_cast<int>(ch.wave.sampleRate));
    _format.setSampleSize(ch.wave.bitsPerSample);
    _format.setChannelCount(ch.wave.numChannels);
    _format.setCodec("audio/pcm");
    _format.setByteOrder(QAudioFormat::LittleEndian);
    _format.setSampleType(ch.wave.bitsPerSample == 8 ? QAudioFormat::UnSignedInt : QAudioFormat::SignedInt);

    m_audioPlayer->resetAudio();
    m_audioPlayer->setAudioFormat(_format);
    m_audioPlayer->setAudioData(audioData);
//    ui->labelTotalTime->setText(QTime(0, 0).addSecs(m_duration).toString("m:ss"));
    ui->btnPlay->setEnabled(true);

    QApplication::beep();
}

void AudioWidget::slot_stateChanged(QAudio::State state)
{
    switch (state)
    {
    case QAudio::ActiveState:
        changeStateIcon(1);
        break;
    case QAudio::SuspendedState:
        changeStateIcon(2);
        break;
    case QAudio::StoppedState:
        changeStateIcon(0);
        break;
    default:
        break;
    }
}

void AudioWidget::slot_playedUSecs(int USecs)
{
    int playedSecs = USecs / 1e6;
    if (!ui->sliderProgress->isSliderDown())
    {
        ui->sliderProgress->blockSignals(true);
        ui->sliderProgress->setValue(playedSecs);
        ui->sliderProgress->blockSignals(false);
        ui->labelPlayedTime->setText(QTime(0, 0).addSecs(playedSecs).toString("m:ss"));
    }
}

void AudioWidget::on_btnPlay_clicked()
{
    if (m_duration == 0 || m_audioPlayer->playMode() != AudioDataPlay::PushMode)
        return;

    switch (m_playbackState)
    {
    case 0:
        m_audioPlayer->startPlay();
        break;
    case 1:
        m_audioPlayer->suspendPlay();
        break;
    case 2:
        m_audioPlayer->resumePlay();
        break;
    }
}

void AudioWidget::on_btnStop_clicked()
{
    m_audioPlayer->stopPlay();
    changeStateIcon(0);
}

void AudioWidget::on_btnOpenFile_clicked()
{
    QString defaultOpenPath = QDir::homePath() + "/Music";
#if 1
    const QString fileName = QFileDialog::getOpenFileName(
        this, QStringLiteral("打开文件"), defaultOpenPath,
        "All(*.*);;"
        "MPEG Audio Layer 3(*.mp3);;"
        "Windows PCM(*.wav);;"
        "Free Lossless Audio Codec(*.flac);;"
        "Windows Media Audio(*wma);;"
//        "Monkey's Audio(*.ape);;"
        );
#else // TEST: 直接用指定文件名
//    QString fileName = defaultOpenPath + QStringLiteral("/IVANA SPAGNA - Con il tuo nome.mp3");
//    QString fileName = defaultOpenPath + QStringLiteral("/原声母带《老鹰乐队-加州旅馆》WAV-192KHZ-64bit(precise)(ED2000.COM).wav");
    QString fileName = defaultOpenPath + QStringLiteral("/HOTEL CALIFORNIA(加州旅馆)-eagle1994墨尔本不插电版本.wav");
#endif
    if (fileName.isEmpty())
        return;

    Q_EMIT sig_preLoad(fileName);
    m_coverImage = QImage();
    m_shallAddImage = QImage();
    ui->labelSongTIT2->clear();
    ui->labelSongTPE2->clear();
    ui->btnPlay->setEnabled(false);
    /// load image from mp3
    QFile file(fileName);
    if (!file.exists() || !file.open(QIODevice::ReadOnly) || file.size() < 10)
    {
        qDebug() << file.errorString() << file.size();
        return;
    }
    auto head = file.peek(10);
    if (!head.startsWith("ID3") && !fileName.endsWith(".mp3")) // 不严谨
        return;

    on_btnStop_clicked();
    m_whichImage = 0;
    m_openedFileName = fileName;
    m_coverFormatStr = "";
    m_hashFrames.clear();
    const uchar *headerTagSize = reinterpret_cast<const uchar *>(head.constData() + 6);
    int mp3TagSize = (headerTagSize[0] << 21) | (headerTagSize[1] << 14) | (headerTagSize[2] << 7) | headerTagSize[3];
    int totalTagSize = mp3TagSize + 10; // 数据帧前的所有标签总长度
    QString title, author;
    qDebug() << "mp3_TagSize =" << mp3TagSize;
    file.seek(10);
    bool imageFlag = false;

    while (file.pos() < totalTagSize && !file.atEnd())
    {
        QSharedPointer<ID3v2Frame> sptr_frame(new ID3v2Frame);
        sptr_frame->beginPos = file.pos();
        file.read(reinterpret_cast<char *>(&sptr_frame.data()->header), sizeof(ID3v2FrameTag));
//        auto frameID = file.read(4);
//        auto _size = file.read(4);
        const uchar *ptrSize = reinterpret_cast<const uchar *>(&sptr_frame->header.size[0]);
        int frameCount = (ptrSize[0] << 24) | (ptrSize[1] << 16) | (ptrSize[2] << 8) | ptrSize[3];
//        auto flags = file.read(2);
        auto content = file.read(frameCount);
        sptr_frame->frameData = content;
        sptr_frame->frameLength = frameCount + sizeof(ID3v2FrameTag);
        QByteArray baFrameID(sptr_frame->header.frameID, 4);

        if (baFrameID == "APIC") // 读取显示歌曲封面，一般为jpg或png
        {
            Q_ASSERT_X(content.size() > 7, "Open Mp3", "APIC content's size less than 7 Bytes!");
            for (int i = 0; i < content.size() - 7; ++i)
            {
                if (content.mid(i, 2) == QByteArray::fromHex("FFD8")
                    || content.mid(i, 8) == QByteArray::fromHex("89504E470D0A1A0A"))
                {
                    if (static_cast<uchar>(content.at(i)) == 0xFF)
                        m_coverFormatStr = "jpg";
                    else
                        m_coverFormatStr = "png";

                    content.remove(0, i);
                    m_coverImage.loadFromData(content);

                    imageFlag = true;
                    break;
                }
            }
            update();
        }
        else if (baFrameID == "TIT2") // 标题
        {
            if (content.mid(0, 3) == QByteArray::fromHex("01FFFE"))
            {
                content.remove(0, 3);
                title = QTextCodec::codecForName("UTF-16LE")->toUnicode(content);
            }
        }
        else if (baFrameID == "TPE1")// 作者
        {
            if (content.mid(0, 3) == QByteArray::fromHex("01FFFE"))
            {
                content.remove(0, 3);
                author = QTextCodec::codecForName("UTF-16LE")->toUnicode(content);
            }
        }
        else if (baFrameID == QByteArray::fromHex("00000000")) // padding(空白预留) 部分，简单标记下
        {
            if (frameCount == 0)
            {
                qstrcpy(sptr_frame->header.frameID, "0000");
                sptr_frame->frameLength = totalTagSize - sptr_frame->beginPos;
                file.seek(totalTagSize);
            }
        }

        m_hashFrames.insert(QString(sptr_frame->header.frameID), sptr_frame);
    }

    if (imageFlag)
    {
        qDebug() << "image size:" << m_coverImage.width() << m_coverImage.height() << m_coverImage.format();
        ui->labelImage->setToolTip(QString("%1x%2").arg(m_coverImage.width()).arg(m_coverImage.height()));
    }
    else
        m_coverImage = QImage();

    ui->labelSongTIT2->setText(title);
    ui->labelSongTPE2->setText(author);
    file.close();
    update();
}

void AudioWidget::on_sliderVol_valueChanged(int value)
{
    m_audioPlayer->slot_setVolume(qreal(value) / 100.0);
}

void AudioWidget::on_btnLoadImage_clicked()
{
    QString defaultOpenPath = QDir::homePath() + "/Music";
#if 1
    const QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("打开图片"), defaultOpenPath,
                                                          "Image File(*.jpg *.png);;"
                                                          "All(*.*)"
                                                          );
#else
    const QString fileName = defaultOpenPath + QStringLiteral("/IVANA SPAGNA - Con il tuo nome.mp3");
#endif
    if (fileName.isEmpty())
        return;

    QFile picFile(fileName);
    if (!picFile.exists() || !picFile.open(QIODevice::ReadOnly) || picFile.size() < 8)
    {
        qDebug() << picFile.errorString();
        return;
    }
    m_imageFileByteData = picFile.readAll();
    m_shallAddImage = QImage();
    QByteArray apicFrameHeader(4, 0);
    int ifmt = -1;
    if (m_imageFileByteData.left(2) == QByteArray::fromHex("FFD8"))
    {
        apicFrameHeader.insert(1, "image/jpeg");
        ifmt = 0;
    }
    else if (m_imageFileByteData.left(8) == QByteArray::fromHex("89504E470D0A1A0A"))
    {
        apicFrameHeader.insert(1, "image/png");
        ifmt = 1;
    }
    else
        return;

    QImage tempImg;
    tempImg.load(fileName);

    QByteArray _ba;
    QBuffer buffer(&_ba);
    buffer.open(QIODevice::WriteOnly);
    if (tempImg.width() != tempImg.height()) // 长宽不等，以较长的边为准作为正方形
    {
        int sz = qMax(tempImg.width(), tempImg.height());
        QImage zoomedSquareImg(sz, sz, tempImg.format());
        zoomedSquareImg.fill(ifmt == 0 ? Qt::white : Qt::transparent);
        QPainter pa(&zoomedSquareImg);

        if (sz == tempImg.width())
            pa.drawImage(0, (sz - tempImg.height()) / 2, tempImg);
        else if (sz == tempImg.height())
            pa.drawImage((sz - tempImg.width()) / 2, 0, tempImg);

        zoomedSquareImg.save(&buffer, ifmt == 0 ? "JPG" : "PNG");
        m_shallAddImage = qMove(zoomedSquareImg);
    }
    else
    {
        tempImg.save(&buffer, ifmt == 0 ? "JPG" : "PNG");
        m_shallAddImage = qMove(tempImg);
    }
    m_imageFileByteData = apicFrameHeader + _ba;
    m_whichImage = 1;
    update();
}

void AudioWidget::on_btnSaveNew_clicked()
{
    if (m_imageFileByteData.isEmpty() || !m_openedFileName.endsWith(".mp3"))
        return;

    QFile file(m_openedFileName);

    if (!file.open(QIODevice::ReadOnly))
        return;

    QByteArray temp = file.readAll();
    file.close();

    if (!temp.startsWith("ID3")) // 文件开头若没有ID3v2的头，加上
    {
        if (temp.size() <= 128)
            return;

        QByteArray id3v1Tag = temp.right(128);
        if (!id3v1Tag.startsWith("TAG"))
            return;

        ID3v2Header hdr;
        memset(&hdr, 0, sizeof(hdr));
        qstrcpy(&hdr.header[0], "ID3");
        hdr.ver = 3;
        hdr.revision = 0;
        hdr.flag = 0;
        temp.prepend(reinterpret_cast<char *>(&hdr), sizeof(hdr));
    }

    uchar *headerTag = reinterpret_cast<uchar *>(temp.data() + 6);
    int mp3TagSize = (headerTag[0] << 21) | (headerTag[1] << 14) | (headerTag[2] << 7) | headerTag[3];
    int frameSize = m_imageFileByteData.size();

    ID3v2FrameTag fh;
    memset(&fh, 0, sizeof(fh));
    qstrcpy(&fh.frameID[0], "APIC");
    fh.size[0] = frameSize >> 24;
    fh.size[1] = frameSize >> 16;
    fh.size[2] = frameSize >> 8;
    fh.size[3] = frameSize;

    QByteArray insertFrame;
    insertFrame.append(reinterpret_cast<char *>(&fh), sizeof(fh));
    insertFrame.append(m_imageFileByteData);

    int newHeaderTagSize = 0;

    if (m_hashFrames.contains("APIC")) // 歌曲已有封面信息
    {
        int diffSize = insertFrame.size() - m_hashFrames["APIC"]->frameLength;
        newHeaderTagSize = mp3TagSize + diffSize;
        if (diffSize >= 0)
        {
            temp.insert(m_hashFrames["APIC"]->beginPos + m_hashFrames["APIC"]->frameLength, diffSize, 0x00);
        }
        else
        {
            temp.remove(m_hashFrames["APIC"]->beginPos + insertFrame.size(), -diffSize);
        }
        memcpy(temp.data() + m_hashFrames["APIC"]->beginPos, insertFrame.constData(), insertFrame.size());
    }
    else
    {
        newHeaderTagSize = mp3TagSize + insertFrame.size();
        if (m_hashFrames.contains("0000")) // 无封面, 有padding部分，在此之前插入图片
        {
            temp.insert(m_hashFrames["0000"]->beginPos, insertFrame);
        }
        else // 无封面, 无padding部分，在ID3v2标签帧最后插入图片
        {
            temp.insert(mp3TagSize + 10, insertFrame);
        }
    }

    temp[6] = newHeaderTagSize >> 21;
    temp[7] = newHeaderTagSize >> 14;
    temp[8] = (newHeaderTagSize >> 7) % 128;
    temp[9] = newHeaderTagSize % 128;

    QString bakName = m_openedFileName;
    bakName.insert(bakName.lastIndexOf('.'), "_BAK");
    file.rename(bakName);

    QFile fileNew(m_openedFileName);
    if (fileNew.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        fileNew.write(temp);
        fileNew.close();
        qDebug() << "save success:" << fileNew.fileName();
    }
}

void AudioWidget::on_btnSaveCover_clicked()
{
    if (m_hashFrames.contains("APIC") && !m_coverImage.isNull())
    {
        auto imgName = m_openedFileName.left(m_openedFileName.lastIndexOf('.'));
        m_coverImage.save(imgName + "." + m_coverFormatStr, m_coverFormatStr.toStdString().c_str());
    }
}

/** ---------- @class TestStream ----------- */
void TestStream::load(const QString &fileName)
{
    QFile file(fileName);
    if (!file.exists() || !file.open(QIODevice::ReadOnly) || file.size() < 10)
    {
        qDebug() << file.errorString() << file.size();
        return;
    }
    QByteArray head = file.peek(10);
    file.close();

    QByteArray _baContent;
#ifdef USE_QAUDIODECODER // QAudioDecoder 依赖系统自带的音频解码器，很多格式不支持，且效率一般
    QEventLoop loop;
    QSharedPointer<QAudioDecoder> decoder(new QAudioDecoder);    
    connect(decoder.data(), &QAudioDecoder::bufferReady, [this, decoder, &_baContent]()
    {
        if (decoder->bufferAvailable())
        {
            QAudioBuffer buffer = decoder->read();
            _baContent.append(buffer.constData<char>(), buffer.byteCount());
        }
    });
    connect(decoder.data(), &QAudioDecoder::finished, [this, decoder, &_baContent]()
    {
        QAudioFormat fmt = decoder->audioFormat();
        QByteArray header(TotalHeadSize, 0);
        WavFile::writeBaseHeader(header.data(), fmt.sampleRate(), fmt.sampleSize(), fmt.channelCount(), _baContent.size());
        auto bytesPerSec = fmt.sampleRate() * fmt.channelCount() * 2;
        qDebug() << "format:" <<  fmt.sampleRate() << fmt.sampleSize() << fmt.channelCount();
        _baContent.prepend(header);
    });
    connect(decoder.data(), &QAudioDecoder::finished, &loop, &QEventLoop::quit);
    connect(decoder.data(), QOverload<QAudioDecoder::Error>::of(&QAudioDecoder::error), [decoder](QAudioDecoder::Error error)
    { qDebug() << error << decoder->errorString();});
    connect(decoder.data(), QOverload<QAudioDecoder::Error>::of(&QAudioDecoder::error), &loop, &QEventLoop::quit);

    decoder->setSourceFilename(fileName);
    qInfo(">>> decoder start: <%s>", QFileInfo(fileName).fileName().toStdString().c_str());
    QTimer::singleShot(5000, &loop, &QEventLoop::quit); // 超时表示代表有可能解码失败，退出事件循环
    QTime tt(QTime::currentTime());
    decoder->start();
    loop.exec();

    double tmp = static_cast<double>(_baContent.size()) / 1024.0;
    int el = tt.elapsed();
    QTime musicLen = QTime(0, 0, 0).addMSecs(decoder->duration());
//    QString meas = DownVscVsixWidget::getSuitableDecMeasure(_baContent.size(), &tmp);
    qInfo(">>> decoder finished, source_size: %.2f K, duration: %s elapsed: %dms",
          tmp, musicLen.toString("mm:ss.zzz").toStdString().c_str(), el);

    if (decoder->error() == QAudioDecoder::NoError && _baContent.size() > TotalHeadSize)
    {
        file.close();
//        Q_EMIT sig_duration(decoder->duration() / 1000); // 解析成功
        return;
    }
    else
    {
        qDebug() << "error message" << decoder->errorString() << _baContent.size();
        return;
    }
#endif
    quint32 secDuration = 0;
    QByteArray header(TotalHeadSize, 0);
    if (head.startsWith("ID3") || fileName.endsWith(".mp3"))
    {
        quint32 rate, totalCount, channels;
//        QTime t(QTime::currentTime());
        QElapsedTimer t;
        t.start();
        qint16 *shBuf = DecodeToBuffer(fileName, &rate, &totalCount, &channels);

        _baContent.resize(totalCount * channels * 2);
        memcpy(_baContent.data(), shBuf, _baContent.size());
        free(shBuf);
        auto _bytesPerSec = rate * channels * 2;
        qDebug() << "load Mp3:" << _baContent.size() << "elapsed:" << t.elapsed() << "ms";
        secDuration = _baContent.size() / _bytesPerSec;

        WavFile::writeBaseHeader(header.data(), rate, sizeof(qint16) * 8, channels, _baContent.size());
    }
    else if (head.startsWith("RIFF"))
    {
        WavFile wavFile;
        if (!wavFile.open(fileName))
            return;

        QDataStream out(&wavFile);
        wavFile.seek(0);
        _baContent.resize(wavFile.size());
        out.readRawData(header.data(), TotalHeadSize);
        out.readRawData(_baContent.data(), wavFile.size() - TotalHeadSize);
        wavFile.close();
        auto _bytesPerSec = wavFile.fileFormat().bytesForDuration(1e6);
        secDuration = _baContent.size() / _bytesPerSec;
        qDebug() << "load RIFF" << secDuration << _baContent.size();
    }
    Q_EMIT sig_readyData(header, _baContent);
    Q_EMIT sig_duration(secDuration);
}
