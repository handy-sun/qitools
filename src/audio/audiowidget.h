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

struct ID3v2_header // ID3v2标签帧的头
{
    char frameID[4]; /*用四个字符标识一个帧，说明其内容*/
    char size[4]; /*帧内容的大小，不包括帧头*/
    char flags[2]; /*存放标志，只定义了6位*/
};

struct ID3v2_frame // ID3v2标签帧整体 + 自定义的辅助信息
{
    ID3v2_header header;
    QByteArray frameData;
    // 以下为自定义的辅助信息
    quint32 beginPos;// 此标签帧位于文件的位置
    quint32 frameLength;// 此标签帧的总长度
};

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
    void sig_data(int sign, int time, const QByteArray &ba);
public Q_SLOTS:
    void load(const QString &fileName);
    void slot_playbackStateChanged(int state);
    void slot_timePositioning(int second);
private Q_SLOTS:
    void onTimer();
};

class AudioWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AudioWidget(QWidget *parent = nullptr);
    ~AudioWidget() Q_DECL_OVERRIDE;

private:
    Ui::AudioWidget *ui;
    AudioDataPlay   *m_audioPlayer;
    QByteArray       m_baContent;
    QByteArray       m_imageFileByteData;
    QString          m_openedFileName;
    QBuffer          m_readBuffer;
    QThread          m_thread;
    QImage           m_coverImage;
    QImage           m_shallAddImage;
    QTimer          *m_timer;
    TestStream      *m_te;
    quint32          m_contentPos;
    qint32           m_duration;
    qint32           m_playTime;
    int              m_bytesPerSec;
    int              m_playbackState; // 0 stop 1 play 2 suspend
    int              m_whichImage; // 0 m_coverImage 1 m_shallAddImage

    QHash<QString, QSharedPointer<ID3v2_frame> > m_hashFrames;

    void changeStateIcon(int state);

protected:
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void sig_playbackStateChanged(int state);
    void sig_preLoad(const QString &fileName);
    void sig_timePositioning(int second);

public Q_SLOTS:
    void slot_setDuration(qint32 d);
    void slot_handleData(int sign, int time, const QByteArray &ba);

private Q_SLOTS:
    void onTimerPull();
    void on_btnPlay_clicked();
    void on_btnStop_clicked();
    void on_btnOpenFile_clicked();
    void on_sliderVol_valueChanged(int value);

    void on_btnLoadImage_clicked();
    void on_btnSaveNew_clicked();
};

#endif // AUDIOWIDGET_H
