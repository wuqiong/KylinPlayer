#ifndef COMMON_H
#define COMMON_H
#include <QObject>
#include <QString>
#include <stdint.h>


typedef struct KChannel
{
    QString id;
    QString title;
}KChannel;

typedef struct KMediaInfo
{
    int32_t id;
    QString songId;
    QString songName;
    QString artistName;
    QString albumName;
    QString songPicUrl;
    QString lrcUrl;
    QString mp3Url;
    bool operator ==(const KMediaInfo &info)
    {
        return this->id == info.id;
    }
    KMediaInfo& operator =(const KMediaInfo &info)
    {
        this->id = info.id;
        this->albumName = info.albumName;
        this->artistName = info.artistName;
        this->lrcUrl = info.lrcUrl;
        this->mp3Url = info.mp3Url;
        this->songId = info.songId;
        this->songName = info.songName;
        this->songPicUrl = info.songPicUrl;
        return *this;
    }
}KMediaInfo;

#endif // COMMON_H


