#ifndef STRUCT_ID3V2_H
#define STRUCT_ID3V2_H

#include <QByteArray>
#include <QtGlobal>

struct ID3v2Header // ID3v2文件头
{
    char header[3];   /*必须为“ID3”否则认为标签不存在*/
    char ver;         /*版本号ID3V2.3 就记录3*/
    char revision;    /*副版本号此版本记录为0*/
    char flag;        /*标志字节，只使用高三位，其它位为0 */
    char size[4];     /*标签大小*/
};

struct ID3v2FrameTag // ID3v2标签帧的头
{
    char frameID[4]; /*用四个字符标识一个帧，说明其内容*/
    char size[4]; /*帧内容的大小，不包括帧头*/
    char flags[2]; /*存放标志，只定义了6位*/
};

struct ID3v2Frame // ID3v2标签帧整体 + 自定义的辅助信息
{
    ID3v2FrameTag header;
    QByteArray frameData;
    // 以下为自定义的辅助信息
    quint32 beginPos;// 此标签帧位于文件的位置
    quint32 frameLength;// 此标签帧的总长度
};

#endif // STRUCT_ID3V2_H
