#include "dtos/betdto.h"
#include "messagestosend.h"

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
    query.bindValue(":amount", bet.getAmount());
    query.bindValue(":koef", bet.getKoef());
    query.bindValue(":match_result_id", getMatchResultID(bet));
    query.bindValue(":match_id", getMatchID(bet));
    query.bindValue(":player_id", chatID);

    return exec(query);
}

std::string BetDTO::playerCurrentBets()
{
    QString cmd = "SELECT b.amount, b.koef, mr.name, m.team1, m.team2, m.time FROM bets b "
                  "JOIN (matches m, match_results mr) ON (m.id = b.match_id AND mr.id = b.match_result_id) "
                  "WHERE player_id = :chatID AND m.match_result_id IS NULL "
                  "ORDER BY m.time DESC "
                  "LIMIT 10;";
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
           << "Result: " << matchRes.toStdString() << "(" << koef << ")\n"
           << "Bet: " << amount << " -> " << static_cast<int>(amount * koef) << "\n"
           << "Time: " << time.toStdString() << "\n\n";
    }

    return ss.str();
}

std::string BetDTO::playerPlayedBets()
{
    QString cmd = "SELECT b.amount, b.koef, b.match_result_id, m.team1, m.team2, m.match_result_id, mr.name, br.name FROM bets b "
                  "JOIN (matches m, match_results br, match_results mr) ON (m.id = b.match_id AND br.id = b.match_result_id AND mr.id = m.match_result_id) "
                  "WHERE player_id = :chatID AND m.match_result_id IS NOT NULL "
                  "ORDER BY m.time DESC "
                  "LIMIT 10;";
    QSqlQuery query;
    query.prepare(cmd);
    query.bindValue(":chatID", chatID);
    exec(query);

    std::stringstream ss;
    ss << "Last played bets\n\n";

    while (query.next())
    {
        QSqlRecord record = query.record();
        int amount = record.value(0).toInt();
        double koef = record.value(1).toDouble();
        int betResID = record.value(2).toInt();
        QString team1 = record.value(3).toString();
        QString team2 = record.value(4).toString();
        int matchResID = record.value(5).toInt();
        QString choosedRes = record.value(6).toString();
        QString matchRes = record.value(7).toString();

        if(betResID == matchResID)
        {
            ss << team1.toStdString() << " vs " << team2.toStdString() << "(" << matchRes.toStdString() << ")" << "\n"
               << "Bet: " << amount << " on " << choosedRes.toStdString() << "(" << koef << ")\n"
               << "Result: " << Emojis::CHECK_MARK << "\n"
               << "Coins change: " << "+" << static_cast<int>(amount * koef) << "\n\n";
        }
        else
        {
            ss << team1.toStdString() << " vs " << team2.toStdString() << "(" << matchRes.toStdString() << ")" << "\n"
               << "Bet: " << amount << " on " << choosedRes.toStdString() << "(" << koef << ")\n"
               << "Result: " << Emojis::CROSS_MARK << "\n"
               << "Coins change: " << "-" << amount << "\n\n";
        }
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

