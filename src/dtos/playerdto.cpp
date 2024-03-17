#include "dtos/playerdto.h"

#include <QDebug>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>

PlayerDTO::PlayerDTO(int chatID_)
{
    chatID = chatID_;
}

int PlayerDTO::getCoins()
{
    QString cmd("SELECT players.coins FROM players WHERE players.id=:id");
    QSqlQuery query;
    query.prepare(cmd);
    query.bindValue(":id", chatID);

    exec(query);

    if (query.next())
    {
        QSqlRecord record = query.record();
        int coins = record.value(0).toInt();
        return coins;
    }
    return 0;
}

bool PlayerDTO::updateCoins(int coins)
{
    QString cmd("UPDATE players SET players.coins = :coins WHERE players.id=:id");
    QSqlQuery query;
    query.prepare(cmd);
    query.bindValue(":coins", coins);
    query.bindValue(":id", chatID);

    return exec(query);
}

bool PlayerDTO::add(const std::string &firstName, const std::string &lastName)
{
    QString cmd("INSERT INTO players VALUES(:id, :coins, :firstName, :lastName)");
    QSqlQuery query;
    query.prepare(cmd);
    query.bindValue(":id", chatID);
    query.bindValue(":coins", START_COINS);
    query.bindValue(":firstName", QString::fromStdString(firstName));
    query.bindValue(":lastName", QString::fromStdString(lastName));

    return exec(query);
}


