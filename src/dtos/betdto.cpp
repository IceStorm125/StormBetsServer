#include "dtos/betdto.h"
#include "messagestosend.h"
#include "dbconnection.h"

#include <QDebug>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>

#include <sstream>

#include "dbconnection.h"

#include "spdlog/spdlog.h"
#include <fmt/core.h>

#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

std::string getCurrentTime() {
    static const int offset = 3;
    return QDateTime::currentDateTime().addSecs(offset*60*60).toString("yyyy-MM-dd hh:mm:ss").toStdString();
}


BetDTO::BetDTO(int chatID_)
{
    chatID = chatID_;
}

bool BetDTO::confirm(const Processing &bet)
{
    QString cmd("INSERT INTO bets(amount, koef, match_result_id, match_id, player_id, time) "
                "VALUES(:amount, :koef, :match_result_id, :match_id, :player_id, :time);");
    QSqlDatabase& db = DBconnection::connection(); 
    QSqlQuery query(db);
    query.prepare(cmd);
    query.bindValue(":amount", bet.getAmount());
    query.bindValue(":koef", bet.getKoef());
    query.bindValue(":match_result_id", getMatchResultID(bet));
    query.bindValue(":match_id", getMatchID(bet));
    query.bindValue(":player_id", chatID);
    query.bindValue(":time", QString::fromStdString(getCurrentTime()));

    return exec(query);
}

bool BetDTO::deleteBetByID(int id)
{
    QString cmd = "DELETE FROM bets WHERE id = :id;";
    QSqlDatabase& db = DBconnection::connection(); 
    QSqlQuery query(db);
    query.prepare(cmd);
    query.bindValue(":id", id);
    return exec(query);
}

int BetDTO::getBetAmountByID(int id)
{
    QString cmd = "SELECT amount FROM bets WHERE id = :id;";
    QSqlDatabase& db = DBconnection::connection(); 
    QSqlQuery query(db);
    query.prepare(cmd);
    query.bindValue(":id", id);
    exec(query);

    if (query.next())
    {
        QSqlRecord record = query.record();
        int amount = record.value(0).toInt();
        return amount;
    }
    else
    {
        spdlog::warn("getBetAmountByID" + query.lastError().text().toStdString());

    }
    return 0;
}

std::string BetDTO::playerCurrentBets()
{
    QString cmd = "SELECT b.amount, b.koef, mr.name, m.team1, m.team2, m.time FROM bets b "
                  "JOIN (matches m, match_results mr) ON (m.id = b.match_id AND mr.id = b.match_result_id) "
                  "WHERE player_id = :chatID AND m.match_result_id IS NULL "
                  "ORDER BY m.time ASC;";
    QSqlDatabase& db = DBconnection::connection(); 
    QSqlQuery query(db);
    query.prepare(cmd);
    query.bindValue(":chatID", chatID);
    exec(query);

    std::string out{""};
    while (query.next())
    {
        QSqlRecord record = query.record();
        int amount = record.value(0).toInt();
        double koef = record.value(1).toDouble();
        QString matchRes = record.value(2).toString();
        QString team1 = record.value(3).toString();
        QString team2 = record.value(4).toString();
        QDateTime time = record.value(5).toDateTime();

        out += fmt::format("{} vs {}\n"
                          "Result: {}({})\n"
                          "Bet: {} -> {}\n"
                          "Time: {}\n\n",
                          team1.toStdString(), team2.toStdString(), matchRes.toStdString(), koef, amount, static_cast<int>(amount * koef), 
                          time.toString("dd.MM.yyyy hh:mm").toStdString());
    }

    return out;
}

std::string BetDTO::playerCurrentBetsToDelete(std::map<int, int> &betNumberToID)
{
    QString cmd = "SELECT b.amount, b.koef, mr.name, m.team1, m.team2, m.time, ROW_NUMBER() OVER( ORDER BY m.time DESC) AS 'rownumber', b.id FROM bets b "
                  "JOIN (matches m, match_results mr) ON (m.id = b.match_id AND mr.id = b.match_result_id) "
                  "WHERE player_id = :chatID AND m.match_result_id IS NULL "
                  "ORDER BY m.time ASC;";
    QSqlDatabase& db = DBconnection::connection(); 
    QSqlQuery query(db);
    query.prepare(cmd);
    query.bindValue(":chatID", chatID);
    exec(query);

    std::string out{""};

    while (query.next())
    {
        QSqlRecord record = query.record();
        int amount = record.value(0).toInt();
        double koef = record.value(1).toDouble();
        QString matchRes = record.value(2).toString();
        QString team1 = record.value(3).toString();
        QString team2 = record.value(4).toString();
        QDateTime time = record.value(5).toDateTime();
        int number = record.value(6).toInt();
        int ID = record.value(7).toInt();

        out += fmt::format("{}. {} vs {}\n"
                          "Result: {}({})\n"
                          "Bet: {} -> {}\n"
                          "Time: {}\n\n",
                          number, team1.toStdString(), team2.toStdString(), matchRes.toStdString(), koef, amount, static_cast<int>(amount * koef), 
                          time.toString("dd.MM.yyyy hh:mm").toStdString());

        betNumberToID.insert({number, ID});
    }

    return out;
}

std::string BetDTO::playerPlayedBets(int limit)
{
    QString cmd = "SELECT b.amount, b.koef, b.match_result_id, m.team1, m.team2, m.match_result_id, mr.name, br.name FROM bets b "
                  "JOIN (matches m, match_results br, match_results mr) ON (m.id = b.match_id AND br.id = b.match_result_id AND mr.id = m.match_result_id) "
                  "WHERE player_id = :chatID AND m.match_result_id IS NOT NULL "
                  "ORDER BY m.time DESC "
                  "LIMIT :limit;";
    QSqlDatabase& db = DBconnection::connection(); 
    QSqlQuery query(db);
    query.prepare(cmd);
    query.bindValue(":chatID", chatID);
    query.bindValue(":limit", limit);
    exec(query);

    
    if(!query.size())
    {
        return "You don't have played bets";
    }

    std::string out {""};
    out += "Last played bets\n\n";

    while (query.next())
    {
        QSqlRecord record = query.record();
        int amount = record.value(0).toInt();
        double koef = record.value(1).toDouble();
        int betResID = record.value(2).toInt();
        QString team1 = record.value(3).toString();
        QString team2 = record.value(4).toString();
        int matchResID = record.value(5).toInt();
        QString matchRes = record.value(6).toString();
        QString choosedRes = record.value(7).toString();

        out += fmt::format("{} vs {}({})\n"
                           "Bet: {} on {}({})\n",
                           team1.toStdString(), team2.toStdString(), matchRes.toStdString(), amount, choosedRes.toStdString(), koef);

        if(betResID == matchResID)
        {
            out += fmt::format("Result: {}\n"
                               "Coins change: +{}\n\n",
                               Emojis::CHECK_MARK, static_cast<int>(amount * koef));
        }
        else
        {
            out += fmt::format("Result: {}\n"
                               "Coins change: -{}\n\n",
                               Emojis::CROSS_MARK, amount);
        }
    }

    return out;
}

int BetDTO::getMatchResultID(const Processing &bet)
{
    QString cmd = "SELECT id FROM match_results WHERE name = :name";
    QSqlDatabase& db = DBconnection::connection(); 
    QSqlQuery query(db);
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
        spdlog::warn("getMatchResultID" + query.lastError().text().toStdString());

    }
    return 0;
}

int BetDTO::getMatchID(const Processing &bet)
{
    QString cmd = "SELECT id FROM matches WHERE api_id = :api_id";
    QSqlDatabase& db = DBconnection::connection(); 
    QSqlQuery query(db);
    query.prepare(cmd);
    query.bindValue(":api_id", bet.getMatch().getApiID());
    exec(query);

    if (query.next())
    {
        QSqlRecord record = query.record();
        int matchID = record.value(0).toInt();
        return matchID;
    }
    else
    {
        spdlog::warn("getMatchID" + query.lastError().text().toStdString());

    }
    return 0;
}

