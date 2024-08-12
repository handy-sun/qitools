#ifndef WAVFILE_H
#define WAVFILE_H

#include <QObject>
#include <QFile>
#include <QAudioFormat>

struct chunk
{
    char id[4];
    quint32 size;
};

struct RIFFHeader
{
    chunk descriptor;  // "RIFF" descriptor.size don't contain itself
    char type[4];      // "WAVE"
};

struct WAVEHeader
{
    chunk descriptor;
    quint16 audioFormat;
    quint16 numChannels;
    quint32 sampleRate;
    quint32 byteRate;
    quint16 blockAlign;
    quint16 bitsPerSample;
};

struct DATAHeader
{
    chunk descriptor;
};

struct CombinedHeader
{
    RIFFHeader riff;
    WAVEHeader wave;
};

class WavFile : public QFile
{
public:
    WavFile(QObject *parent = nullptr);

    using QFile::open;
    bool open(const QString &fileName);
    const QAudioFormat &fileFormat() const;
    int headerLength() const;

    static int writeBaseHeader(char *dst, quint32 sampleRate, quint16 bitsPerSample, quint16 channels, quint32 dataSize);

private:
    bool readHeader();

private:
    QAudioFormat m_fileFormat;
    int m_headerLength;
};

#endif // WAVFILE_H
