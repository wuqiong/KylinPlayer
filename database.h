#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QtSql>
#include <QList>
#include "common.h"
class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = 0);

signals:

public slots:
    bool initDataBase();
    bool addFavoriteSong(KMediaInfo songInfo);
    QList<KMediaInfo> *getFavoriteSong();
private:
    QSqlDatabase database;
};

#endif // DATABASE_H
