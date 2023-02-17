#include "dtos/betdto.h"

#include <QDebug>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>

#include <sstream>

BetDTO::BetDTO(int chatID_)
{
    chatID = chatID_;
}

bool BetDTO::confirm(const Processing &bet)
{
    addMatch(bet);
    QString cmd("INSERT INTO bets(amount, koef, match_result_id, match_id, player_id) "
                "VALUES(:amount, :koef, :match_result_id, :match_id, :player_id)");
    QSqlQuery query;
    query.prepare(cmd);
    query.bindValue(":amount",bet.getAmount());
    query.bindValue(":koef",bet.getKoef());
    query.bindValue(":match_result_id", getMatchResultID(bet));
    query.bindValue(":match_id", getMatchID(bet));
    query.bindValue(":player_id", chatID);

    return exec(query);
}

std::string BetDTO::playerCurrentBets()
{
    QString cmd = "SELECT bets.amount, bets.koef, match_results.name, matches.team1, matches.team2, matches.time FROM bets "
                  "JOIN (matches, match_results) ON (matches.id = bets.match_id AND match_results.id = bets.match_result_id) "
                  "WHERE player_id = :chatID;";
    QSqlQuery query;
    query.prepare(cmd);
    query.bindValue(":chatID", chatID);
    exec(query);

    std::stringstream ss;
    while (query.next())
    {
        QSqlRecord record = query.record();
        int amount = record.value(0).toInt();
        double koef = record.value(1).toDouble();
        QString matchRes = record.value(2).toString();
        QString team1 = record.value(3).toString();
        QString team2 = record.value(4).toString();
        QString time = record.value(5).toString();

        ss << team1.toStdString() << " vs " << team2.toStdString() << "\n"
           << "Result: " << matchRes.toStdString() << "\n"
           << "Amount: " << amount << "\n"
           << "Koef: " << koef << "\n"
           << "Potential gain: " << static_cast<int>(amount * koef) << "\n"
           << "Time: " << time.toStdString() << "\n\n";
    }

    return ss.str();
}

int BetDTO::getMatchResultID(const Processing &bet)
{
    QString cmd = "SELECT id FROM match_results WHERE name = :name";
    QSqlQuery query;
    query.prepare(cmd);
    query.bindValue(":name", bet.getResult() == Processing::Result::W1 ? "W1" : bet.getResult() == Processing::Result::W2 ? "W2" : "X");
    exec(query);

    if (query.next())
    {
        QSqlRecord record = query.record();
        int matchResultID = record.value(0).toInt();
        return matchResultID;
    }
    else
    {
        qInfo() << query.lastError().text();
    }
    return 0;
}

int BetDTO::getMatchID(const Processing &bet)
{
    QString cmd = "SELECT id FROM matches WHERE api_id = :api_id";
    QSqlQuery query;
    query.prepare(cmd);
    query.bindValue(":api_id", bet.getMatch().getID());
    exec(query);

    if (query.next())
    {
        QSqlRecord record = query.record();
        int matchID = record.value(0).toInt();
        return matchID;
    }
    else
    {
        qInfo() << query.lastError().text();
    }
    return 0;
}

void BetDTO::addMatch(const Processing &bet)
{
    QString cmd = "INSERT INTO matches(team1, team2, api_id, time) VALUES(:team1, :team2, :api_id, :time)";
    QSqlQuery query;
    query.prepare(cmd);
    query.bindValue(":team1", bet.getMatch().getTeam1().first);
    query.bindValue(":team2", bet.getMatch().getTeam2().first);
    query.bindValue(":api_id", bet.getMatch().getID());
    query.bindValue(":time", bet.getMatch().getTime());
    exec(query);
}

