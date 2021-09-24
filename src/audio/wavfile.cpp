#include <qendian.h>
#include "wavfile.h"

WavFile::WavFile(QObject *parent)
    : QFile(parent)
    , m_headerLength(0)
{
}

bool WavFile::open(const QString &fileName)
{
    close();
    setFileName(fileName);
    return QFile::open(QIODevice::ReadOnly) && readHeader();
}

const QAudioFormat &WavFile::fileFormat() const
{
    return m_fileFormat;
}

int WavFile::headerLength() const
{
    return m_headerLength;
}

int WavFile::writeBaseHeader(char *dst, quint32 sampleRate, quint16 bitsPerSample, quint16 channels, quint32 dataSize)
{
    const int BaseHeaderSize = sizeof(CombinedHeader) + sizeof(DATAHeader);
    CombinedHeader ch;
    DATAHeader dh;
    memset(&ch, 0, sizeof(CombinedHeader));
    memset(&dh, 0, sizeof(DATAHeader));
    // RIFF header
    qstrcpy(ch.riff.descriptor.id, "RIFF");
    ch.riff.descriptor.size = dataSize + BaseHeaderSize - 8;
    // WAVE header
    qstrcpy(ch.riff.type, "WAVE");
    qstrcpy(ch.wave.descriptor.id, "fmt");
    ch.wave.descriptor.size = 16; // 格式块长度（一般=16，若=18表示最后有2字节附加信息）
    ch.wave.audioFormat = 1; // 值＝1表示编码方式为PCMμ律编码，非压缩格式
    ch.wave.numChannels = channels;
    ch.wave.sampleRate = sampleRate;
    ch.wave.byteRate = sampleRate * channels * bitsPerSample / 8;
    ch.wave.blockAlign = channels * bitsPerSample / 8;
    ch.wave.bitsPerSample = bitsPerSample;
    // data header
    qstrcpy(dh.descriptor.id, "data");
    dh.descriptor.size = dataSize;

    memcpy(dst, &ch, sizeof(CombinedHeader));
    memcpy(dst + sizeof(CombinedHeader), &dh, sizeof(DATAHeader));

    return BaseHeaderSize;
}

bool WavFile::readHeader()
{
    seek(0);
    CombinedHeader header;
    bool result = read(reinterpret_cast<char *>(&header), sizeof(CombinedHeader)) == sizeof(CombinedHeader);
    if (result)
    {
        if ((memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0 ||
             memcmp(&header.riff.descriptor.id, "RIFX", 4) == 0) &&
             memcmp(&header.riff.type, "WAVE", 4) == 0 && memcmp(&header.wave.descriptor.id, "fmt ", 4) == 0 &&
            (header.wave.audioFormat == 1 || header.wave.audioFormat == 0))
        {
            // Read off remaining header information
            DATAHeader dataHeader;

            if (qFromLittleEndian<quint32>(header.wave.descriptor.size) > sizeof(WAVEHeader))
            {
                // Extended data available
                quint16 extraFormatBytes;
                if (peek((char *)&extraFormatBytes, sizeof(quint16)) != sizeof(quint16))
                    return false;
                const qint64 throwAwayBytes = sizeof(quint16) + qFromLittleEndian<quint16>(extraFormatBytes);
                if (read(throwAwayBytes).size() != throwAwayBytes)
                    return false;
            }

            if (read((char *)&dataHeader, sizeof(DATAHeader)) != sizeof(DATAHeader))
                return false;

            // Establish format
            if (memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0)
                m_fileFormat.setByteOrder(QAudioFormat::LittleEndian);
            else
                m_fileFormat.setByteOrder(QAudioFormat::BigEndian);

            int bps = qFromLittleEndian<quint16>(header.wave.bitsPerSample);
            m_fileFormat.setChannelCount(qFromLittleEndian<quint16>(header.wave.numChannels));
            m_fileFormat.setCodec("audio/pcm");
            m_fileFormat.setSampleRate(qFromLittleEndian<quint32>(header.wave.sampleRate));
            m_fileFormat.setSampleSize(qFromLittleEndian<quint16>(header.wave.bitsPerSample));
            m_fileFormat.setSampleType(bps == 8 ? QAudioFormat::UnSignedInt : QAudioFormat::SignedInt);
        }
        else
        {
            result = false;
        }
    }
    m_headerLength = pos();
    return result;
}
