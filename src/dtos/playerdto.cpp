#include "dtos/playerdto.h"

#include <QDebug>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>

#include <fmt/core.h>
#include <messagestosend.h>
#include "dbconnection.h"


PlayerDTO::PlayerDTO(int chatID_) {
    chatID = chatID_;
}

int PlayerDTO::getCoins() {
    QString cmd("SELECT players.coins FROM players WHERE players.id=:id");
    QSqlDatabase db = DBconnection::connection();
    QSqlQuery query(db);
    query.prepare(cmd);
    query.bindValue(":id", chatID);

    exec(query);

    if (query.next()) {
        QSqlRecord record = query.record();
        int coins = record.value(0).toInt();
        return coins;
    }
    return 0;
}

bool PlayerDTO::updateCoins(int coins) {
    QString cmd("UPDATE players SET players.coins = :coins WHERE players.id=:id");
    QSqlDatabase db = DBconnection::connection();
    QSqlQuery query(db);
    query.prepare(cmd);
    query.bindValue(":coins", coins);
    query.bindValue(":id", chatID);

    return exec(query);
}

bool PlayerDTO::add(const std::string &firstName, const std::string &lastName) {
    QString cmd("INSERT INTO players VALUES(:id, :coins, :firstName, :lastName)");
    QSqlDatabase db = DBconnection::connection();
    QSqlQuery query(db);
    query.prepare(cmd);
    query.bindValue(":id", chatID);
    query.bindValue(":coins", START_COINS);
    query.bindValue(":firstName", QString::fromStdString(firstName));
    query.bindValue(":lastName", QString::fromStdString(lastName));

    return exec(query);
}

int PlayerDTO::getCountAll() {
    QString cmd1("SELECT COUNT(*) FROM bets b "
        "JOIN (matches m) ON (b.match_id = m.id) "
        "WHERE b.player_id = :id "
        "AND b.paid IS NOT NULL");
    QSqlDatabase db = DBconnection::connection();
    QSqlQuery query(db);
    query.prepare(cmd1);
    query.bindValue(":id", chatID);
    exec(query);

    if (!query.next())
        return -1;

    QSqlRecord record = query.record();
    return record.value(0).toInt();
}

int PlayerDTO::getCountWin() {
    QString cmd2("SELECT COUNT(*) FROM bets b "
        "JOIN (matches m) ON (b.match_id = m.id) "
        "WHERE b.player_id = :id "
        "AND b.paid IS NOT NULL "
        "AND b.match_result_id = m.match_result_id");
    QSqlDatabase db = DBconnection::connection();
    QSqlQuery query(db);
    query.prepare(cmd2);
    query.bindValue(":id", chatID);
    exec(query);

    if (!query.next())
        return -1;

    QSqlRecord record = query.record();
    return record.value(0).toInt();
}

int PlayerDTO::getTotalSpent() {
    QString cmd3("SELECT SUM(b.amount) FROM bets b "
        "JOIN (matches m) ON (b.match_id = m.id) "
        "WHERE b.player_id = :id "
        "AND b.paid IS NOT NULL");
    QSqlDatabase db = DBconnection::connection();
    QSqlQuery query(db);
    query.prepare(cmd3);
    query.bindValue(":id", chatID);
    exec(query);

    if (!query.next())
        return -1;

    QSqlRecord record = query.record();
    return record.value(0).toInt();
}

int PlayerDTO::getTotalGain() {
    QString cmd4("SELECT SUM(b.amount * b.koef) FROM bets b "
        "JOIN (matches m) ON (b.match_id = m.id) "
        "WHERE b.player_id = :id "
        "AND b.paid IS NOT NULL "
        "AND b.match_result_id = m.match_result_id");
    QSqlDatabase db = DBconnection::connection();
    QSqlQuery query(db);
    query.prepare(cmd4);
    query.bindValue(":id", chatID);
    exec(query);

    if (!query.next())
        return -1;

    QSqlRecord record = query.record();
    return record.value(0).toInt();
}

std::string PlayerDTO::getStats() {
    int countAll = getCountAll();

    if (countAll <= 0) {
        return "You don't have played bets";
    }

    int countWin = getCountWin();
    int totalSpent = getTotalSpent();
    int totalGain = getTotalGain();

    return fmt::format(
        "Played bets: {}\n\n"
        "{} Won bets: {}\n"
        "{} Winrate: {:.2f}%\n"
        "{} Total spent: {}\n"
        "{} Total gain: {}\n"
        "{} Total diff: {}\n",
        countAll,
        Emojis::CHECK_MARK, countWin,
        Emojis::TROPHY, (double) countWin * 100.0 / countAll,
        Emojis::MONEY_WITH_WINGS, totalSpent,
        Emojis::STAT, totalGain,
        Emojis::DOLLAR_MONEY, totalGain - totalSpent
    );
}
