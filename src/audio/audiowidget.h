#ifndef AUDIOWIDGET_H
#define AUDIOWIDGET_H

#include <QWidget>
#include <QBuffer>
#include <QThread>
#include <QAudio>
#include "struct_id3v2.h"

class AudioDataPlay;
class QAudioDecoder;

namespace Ui {
class AudioWidget;
}

class TestStream;

class AudioWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AudioWidget(QWidget *parent = nullptr);
    ~AudioWidget() Q_DECL_OVERRIDE;

private:
    Ui::AudioWidget *ui;
    AudioDataPlay   *m_audioPlayer;
    QByteArray       m_imageFileByteData;
    QString          m_openedFileName;
//    QBuffer          m_readBuffer;
    QThread          m_thread;
    QString          m_coverFormatStr;
    QImage           m_coverImage;
    QImage           m_shallAddImage;
    TestStream      *m_te;
//    quint32          m_contentPos;
    qint32           m_duration;

    int              m_playbackState; // 0 stop 1 play 2 suspend
    int              m_whichImage; // 0 m_coverImage 1 m_shallAddImage

    QHash<QString, QSharedPointer<ID3v2Frame> > m_hashFrames;

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
    void slot_readyData(const QByteArray &header, const QByteArray &audioData);
    void slot_stateChanged(QAudio::State state);
    void slot_playedUSecs(int USecs);

private Q_SLOTS:
//    void onTimerPull();
    void on_btnPlay_clicked();
    void on_btnStop_clicked();
    void on_btnOpenFile_clicked();
    void on_sliderVol_valueChanged(int value);

    void on_btnLoadImage_clicked();
    void on_btnSaveNew_clicked();
    void on_btnSaveCover_clicked();

};

class TestStream : public QObject
{
    Q_OBJECT

Q_SIGNALS:
    void sig_duration(qint32 d);
    void sig_readyData(const QByteArray &header, const QByteArray &audioData);

public Q_SLOTS:
    void load(const QString &fileName);
};

#endif // AUDIOWIDGET_H
