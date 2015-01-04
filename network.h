#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QtNetwork>
#include "common.h"

class Network : public QObject
{
    Q_OBJECT
public:
    explicit Network(QObject *parent = 0);
    ~Network();
signals:
    void contentGetted(QByteArray *content);
    void musicChannelsFetched(QList<KChannel> *channelList);
    void mediaInfoListFetched(QList<KMediaInfo> *mediaInfoList);
    void mediaInfoFetched(KMediaInfo);
    void imageDataFetched(QByteArray *imageData);
    void lyricsDataFetched(QByteArray *imageData);
public slots:
    void getContentOfURL(QString url);
    void fetchChannels(QString url);
    void fetchMediaInfoList(QString url);
    void fetchSongInfo(QString url);
    void fetchImage(QString url);
    void fetchLyrics(QString url);
    void handleGetFinished();
    void handleFetchChannelsFinished();
    void handleFetchMediaInfoListFinished();
    void handleFetchSongInfoFinished();
    void handleFetchImageFinished();
    void handleFetchLyricsFinished();
private:
    QNetworkAccessManager *networkAccessManager;

};

#endif // NETWORK_H
