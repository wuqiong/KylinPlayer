#include "network.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>


Network::Network(QObject *parent) :
    QObject(parent)
{
    this->networkAccessManager = new QNetworkAccessManager(this);
}

Network::~Network()
{
    delete networkAccessManager;
}

void Network::getContentOfURL(QString url)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "KylinPlayer 1.0");
    QNetworkReply *reply = this->networkAccessManager->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(handleGetFinished()));
}

void Network::fetchChannels(QString url)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "KylinPlayer 1.0");
    QNetworkReply *reply = this->networkAccessManager->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(handleFetchChannelsFinished()));
}

void Network::fetchMediaInfoList(QString url)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "KylinPlayer 1.0");
    QNetworkReply *reply = this->networkAccessManager->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(handleFetchMediaInfoListFinished()));
}

void Network::fetchSongInfo(QString url)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "KylinPlayer 1.0");
    QNetworkReply *reply = this->networkAccessManager->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(handleFetchSongInfoFinished()));
}

void Network::fetchImage(QString url)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "KylinPlayer 1.0");
    QNetworkReply *reply = this->networkAccessManager->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(handleFetchImageFinished()));
}

void Network::fetchLyrics(QString url)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "KylinPlayer 1.0");
    QNetworkReply *reply = this->networkAccessManager->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(handleFetchLyricsFinished()));
}

void Network::handleGetFinished()
{
    QNetworkReply *reply = (QNetworkReply *)sender();
    if(reply->hasRawHeader("Location")){
        QString strLocation = reply->header(QNetworkRequest::LocationHeader).toString();
        qDebug() << strLocation;
        this->getContentOfURL(strLocation);
        return;
    }
    QByteArray strHttpData;
    strHttpData = reply->readAll();
    emit this->contentGetted(new QByteArray(strHttpData));
}

void Network::handleFetchChannelsFinished()
{
    QNetworkReply *reply = (QNetworkReply *)sender();
    if(reply->hasRawHeader("Location")){
        QString strLocation = reply->header(QNetworkRequest::LocationHeader).toString();
//        qDebug() << strLocation;
        this->fetchChannels(strLocation);
        return;
    }
    QByteArray strHttpData;
    strHttpData = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(strHttpData);
    if(jsonDoc.isNull() ||!jsonDoc.isArray())
    {//  null or not array format
        return;
    }
    QJsonArray	channels = jsonDoc.array();
    QList<KChannel> *channelList = new QList<KChannel>();
    for(int i = 0; i < channels.count(); i++)
    {
        QJsonValue value = channels[i];
        QString id = value.toObject().value("id").toString();
        QString title = value.toObject().value("title").toString();
//        qDebug()<<id<<"----"<<title;
        KChannel channel;
        channel.id = id;
        channel.title = title;
        channelList->append(channel);
    }
    emit this->musicChannelsFetched(channelList);
}

void Network::handleFetchMediaInfoListFinished()
{
    QNetworkReply *reply = (QNetworkReply *)sender();
    if(reply->hasRawHeader("Location")){
        QString strLocation = reply->header(QNetworkRequest::LocationHeader).toString();
        qDebug() << strLocation;
        this->fetchMediaInfoList(strLocation);
        return;
    }
    QByteArray strHttpData;
    strHttpData = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(strHttpData);
    if(jsonDoc.isNull() ||!jsonDoc.isObject())
    {//  null or not object format
        return;
    }
    QJsonObject jsonObj = jsonDoc.object();
    QJsonArray mediaArray = jsonObj.value("list").toArray();
    QList<KMediaInfo> *mediaList = new QList<KMediaInfo>();
    for(int i = 0; i < mediaArray.count(); i++)
    {
        KMediaInfo mediaInfo;
        QJsonObject jsonObj2 = mediaArray[i].toObject();
        mediaInfo.id = jsonObj2.value("id").toInt();
        mediaList->append(mediaInfo);
//        qDebug()<<"id:"<<mediaInfo.id;
    }
    emit this->mediaInfoListFetched(mediaList);
}

void Network::handleFetchSongInfoFinished()
{
    QNetworkReply *reply = (QNetworkReply *)sender();
    if(reply->hasRawHeader("Location")){
        QString strLocation = reply->header(QNetworkRequest::LocationHeader).toString();
        qDebug() << strLocation;
        this->fetchMediaInfoList(strLocation);
        return;
    }
    QByteArray strHttpData;
    strHttpData = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(strHttpData);
    if(jsonDoc.isNull() ||!jsonDoc.isObject())
    {//  null or not object format
        return;
    }
    QJsonObject jsonObj = jsonDoc.object();
    QJsonArray songArray = jsonObj.value("data").toObject().value("songList").toArray();
    if(songArray.count()<=0)
    {
        return;
    }
    QJsonObject infoObj = songArray.first().toObject();
    KMediaInfo mediaInfo;
    mediaInfo.id = infoObj.value("songId").toInt();
    mediaInfo.songName = infoObj.value("songName").toString();
    mediaInfo.artistName = infoObj.value("artistName").toString();
    mediaInfo.albumName = infoObj.value("albumName").toString();
    mediaInfo.songPicUrl = infoObj.value("songPicRadio").toString();
    mediaInfo.mp3Url = infoObj.value("songLink").toString();
    mediaInfo.lrcUrl = infoObj.value("lrcLink").toString();


    emit this->mediaInfoFetched(mediaInfo);
}

void Network::handleFetchImageFinished()
{
    QNetworkReply *reply = (QNetworkReply *)sender();
    if(reply->hasRawHeader("Location")){
        QString strLocation = reply->header(QNetworkRequest::LocationHeader).toString();
        qDebug() << strLocation;
        this->fetchMediaInfoList(strLocation);
        return;
    }
    QByteArray strHttpData;
    strHttpData = reply->readAll();
    emit this->imageDataFetched(new QByteArray(strHttpData));
}

void Network::handleFetchLyricsFinished()
{
    QNetworkReply *reply = (QNetworkReply *)sender();
    if(reply->hasRawHeader("Location")){
        QString strLocation = reply->header(QNetworkRequest::LocationHeader).toString();
        qDebug() << strLocation;
        this->fetchMediaInfoList(strLocation);
        return;
    }
    QByteArray strHttpData;
    strHttpData = reply->readAll();
    emit this->lyricsDataFetched(new QByteArray(strHttpData));
}
