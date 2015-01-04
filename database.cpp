#include "database.h"
#include <QDebug>


#define DATABASE_CREATE_SQL "CREATE DATABASE \
    `KylinPlayer` DEFAULT CHARACTER SET utf8;"

#define TABLE_CREATE_SQL \
    "CREATE TABLE FavoriteSong "\
    "(id INTEGER PRIMARY KEY, song_id NUMERIC, song_name TEXT, album_name TEXT, "\
    "singer_name TEXT, song_url TEXT, artcover_url TEXT, lrc_url TEXT,UNIQUE(song_id));"

#define INSERT_SONG_SQL "REPLACE INTO FavoriteSong \
    (song_id, song_name, album_name, singer_name, \
    song_url, artcover_url, lrc_url) \
     VALUES (?, ?, ?, ?, ?, ?, ?)"

#define SELECT_SONG_SQL "SELECT song_id, song_name, album_name, \
     singer_name,song_url,artcover_url,lrc_url \
     FROM FavoriteSong;"


Database::Database(QObject *parent) :
    QObject(parent)
{
}

bool Database::initDataBase()
{
    qDebug() << QSqlDatabase::drivers();
    this->database = QSqlDatabase::addDatabase("QSQLITE");
    if(!this->database.isValid())
    {
        return false;
    }
    qDebug() << "isValid";
    QString dbPath = QDir::homePath() + "/.KylinPlayer.db3";
    dbPath = QDir::toNativeSeparators(dbPath);
    this->database.setDatabaseName(dbPath);//database name
//    this->database.setConnectOptions("CLIENT_SSL=1;CLIENT_IGNORE_SPACE=1");//use SSL
    if(this->database.open())
    {
        qDebug()<<"open "<<this->database.lastError().driverText();
    }else
    {
        qDebug()<<"open failed";
        return false;
    }
    qDebug()<< this->database.databaseName();
    qDebug()<< this->database.tables();
    QSqlQuery query(this->database);
#if 0
    query.exec(DATABASE_CREATE_SQL);
    query.exec("use KylinPlayer;");
#endif
    bool ret = query.exec(TABLE_CREATE_SQL);

    return ret;
}

bool Database::addFavoriteSong(KMediaInfo songInfo)
{
    QSqlQuery query(this->database);
    query.prepare(INSERT_SONG_SQL);
    query.addBindValue(songInfo.id);
    query.addBindValue(songInfo.songName);
    query.addBindValue(songInfo.albumName);
    query.addBindValue(songInfo.artistName);
    query.addBindValue(songInfo.mp3Url);
    query.addBindValue(songInfo.songPicUrl);
    query.addBindValue(songInfo.lrcUrl);
    bool ret =  query.exec();
    qDebug() << "qurey exec:" << ret ;
    query.finish();
    return ret;
}

QList<KMediaInfo> *Database::getFavoriteSong()
{
    QSqlQuery query(this->database);
    QList<KMediaInfo> *mediaInfoList = new QList<KMediaInfo>();
    bool ret = query.exec(SELECT_SONG_SQL);
    if(!ret)
    {
        return NULL;
    }
    while(query.next())
    {
        KMediaInfo info;
        info.id = query.value(0).toInt();
        info.songId = QString("%1").arg(info.id);
        info.songName = query.value(1).toString();
        info.albumName = query.value(2).toString();
        info.artistName = query.value(3).toString();
        info.mp3Url = query.value(4).toString();
        info.songPicUrl = query.value(5).toString();
        info.lrcUrl = query.value(6).toString();
        mediaInfoList->append(info);
    }
    query.finish();
    return mediaInfoList;
}
