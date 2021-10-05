#ifndef EXMP3_H
#define EXMP3_H

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdint.h>
#include <sys/stat.h>

#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"

#if (QT_VERSION >= 0x050000)
#include <QString>
#include <QFile>
#include <QTime>
#include <QDebug>
#include <QByteArray>

// qt - mp3解码
qint16 *DecodeToBuffer(const QString &filename, quint32 *sampleRate, quint32 *totalSampleCount, quint32 *channels)
{
    int music_size = 0;
    int alloc_samples = 1024 * 1024, num_samples = 0;
    qint16 *music_buf = (qint16 *)malloc(alloc_samples * 2 * 2);
    QFile file(filename);
    if (!file.exists() || !file.open(QIODevice::ReadOnly))
    {
        free(music_buf);
        return nullptr;
    }

    QByteArray _baBuf = file.readAll();
    unsigned char *fileBuf = reinterpret_cast<unsigned char *>(_baBuf.data());
    music_size = _baBuf.size();
    if (fileBuf != nullptr)
    {
        unsigned char *buf = fileBuf;
        mp3dec_frame_info_t info;
        mp3dec_t dec;

        mp3dec_init(&dec);
        while (true)
        {
            qint16 frame_buf[MINIMP3_MAX_SAMPLES_PER_FRAME];
            int samples = mp3dec_decode_frame(&dec, buf, music_size, frame_buf, &info);
            if (alloc_samples < num_samples + samples)
            {
                alloc_samples *= 2;
                qint16 *tmp = (qint16 *)realloc(music_buf, alloc_samples * 2 * info.channels);
                if (tmp) {
                    music_buf = tmp;
                    qDebug() << "rell:" << music_size << num_samples << alloc_samples;
                }
            }
            if (music_buf)
                memcpy(music_buf + num_samples * info.channels, frame_buf, samples * info.channels * 2);
            num_samples += samples;

            if (info.frame_bytes <= 0 || music_size <= (info.frame_bytes + 4))
                break;

            buf += info.frame_bytes;
            music_size -= info.frame_bytes;
        }


        if (alloc_samples > num_samples)
        {
            qint16 *tmp = (qint16 *)realloc(music_buf, num_samples * 2 * info.channels);
            //qDebug() << "final:" << alloc_samples << num_samples;
            if (tmp)
                music_buf = tmp;
        }

        if (sampleRate)
            *sampleRate = info.hz;
        if (channels)
            *channels = info.channels;
        if (num_samples)
            *totalSampleCount = num_samples;

        return music_buf;
    }
    if (music_buf)
        free(music_buf);
    return nullptr;
}
#endif

//读取文件buffer
char *getFileBuffer(const char *fname, int *size)
{
    FILE *fd = fopen(fname, "rb");
    if (fd == 0)
        return 0;
    struct stat st;
    char *file_buf = 0;
    if (fstat(fileno(fd), &st) < 0)
        goto doexit;
    file_buf = (char *)malloc(st.st_size + 1);
    if (file_buf != NULL) {
        if (fread(file_buf, st.st_size, 1, fd) < 1) {
            fclose(fd);
            return 0;
        }
        file_buf[st.st_size] = 0;
    }

    if (size)
        *size = st.st_size;
doexit:
    fclose(fd);
    return file_buf;
}
// mp3解码
int16_t *DecodeMp3ToBuffer(char *filename, uint32_t *sampleRate, uint32_t *totalSampleCount, unsigned int *channels)
{
    int music_size = 0;
    int alloc_samples = 1024 * 1024, num_samples = 0;
    int16_t *music_buf = (int16_t *)malloc(alloc_samples * 2 * 2);
    unsigned char *file_buf = (unsigned char *)getFileBuffer(filename, &music_size);
    if (file_buf != NULL) {
        unsigned char *buf = file_buf;
        mp3dec_frame_info_t info;
        mp3dec_t dec;

        mp3dec_init(&dec);
        for (;;) {
            int16_t frame_buf[MINIMP3_MAX_SAMPLES_PER_FRAME];
            int samples = mp3dec_decode_frame(&dec, buf, music_size, frame_buf, &info);
            if (alloc_samples < (num_samples + samples)) {
                alloc_samples *= 2;
                int16_t *tmp = (int16_t *)realloc(music_buf, alloc_samples * 2 * info.channels);
                if (tmp)
                    music_buf = tmp;
            }
            if (music_buf)
                memcpy(music_buf + num_samples * info.channels, frame_buf, samples * info.channels * 2);
            num_samples += samples;
            if (info.frame_bytes <= 0 || music_size <= (info.frame_bytes + 4))
                break;
            buf += info.frame_bytes;
            music_size -= info.frame_bytes;
        }
        if (alloc_samples > num_samples) {
            int16_t *tmp = (int16_t *)realloc(music_buf, num_samples * 2 * info.channels);
            if (tmp)
                music_buf = tmp;
        }

        if (sampleRate)
            *sampleRate = info.hz;
        if (channels)
            *channels = info.channels;
        if (num_samples)
            *totalSampleCount = num_samples;

        free(file_buf);
        return music_buf;
    }
    if (music_buf)
        free(music_buf);
    return 0;
}


#endif // EXMP3_H
