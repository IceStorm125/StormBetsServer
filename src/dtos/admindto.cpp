#include "dtos/playerdto.h"
#include "dtos/admindto.h"
#include "dbconnection.h"

#include <QDebug>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>

#include <fmt/core.h>

#include "messagestosend.h"



AdminDTO::AdminDTO()
{

}

std::vector<int> AdminDTO::getAllMatchesIdWithResult()
{
    QString cmd = "SELECT id FROM matches WHERE match_result_id IS NOT NULL";
    QSqlDatabase db = DBconnection::connection();
    QSqlQuery query(db);
    query.prepare(cmd);
    exec(query);

    std::vector<int> ids;
    while (query.next())
    {
        QSqlRecord record = query.record();
        int id = record.value(0).toInt();
        ids.push_back(id);
    }

    return ids;
}

void AdminDTO::registerResults(const TgBot::Bot &bot)
{
    const std::vector<int> matchIDs = getAllMatchesIdWithResult();
    for (const auto& id : matchIDs)
    {
        updateCoins(id, bot);
    }
}

bool AdminDTO::updateCoins(int matchID, const TgBot::Bot &bot)
{
    QString selectBetsCmd("SELECT id, amount, koef, match_result_id, player_id FROM bets WHERE match_id = :matchID AND paid IS NULL");
    QSqlDatabase db = DBconnection::connection();
    QSqlQuery selectBetsQuery(db);
    selectBetsQuery.prepare(selectBetsCmd);
    selectBetsQuery.bindValue(":matchID", matchID);
    exec(selectBetsQuery);
    
    while(selectBetsQuery.next())
    {
        QSqlRecord record = selectBetsQuery.record();
        int bet_id = record.value(0).toInt();
        int amount = record.value(1).toInt();
        double koef = record.value(2).toDouble();
        int match_result_id_from_bets = record.value(3).toInt();
        int player_id = record.value(4).toInt();

        QString selectIDcmd = "SELECT match_result_id, team1, team2 FROM matches WHERE id = :matchID";
        QSqlQuery selectIDquery(db);
        
        selectIDquery.prepare(selectIDcmd);
        selectIDquery.bindValue(":matchID", matchID);
        exec(selectIDquery);

        if(selectIDquery.next())
        {
            QString updatePaidStatus = "UPDATE bets SET bets.paid = 1 WHERE id = :betID";
            QSqlQuery updatePaidStatusQuery(db);
            updatePaidStatusQuery.prepare(updatePaidStatus);
            updatePaidStatusQuery.bindValue(":betID", bet_id);
            exec(updatePaidStatusQuery);

            QSqlRecord record = selectIDquery.record();
            int match_result_id = record.value(0).toInt();
            QString team1 = record.value(1).toString();
            QString team2 = record.value(2).toString();

            if(match_result_id_from_bets == match_result_id)
            {
                QString cmd("UPDATE players SET players.coins = players.coins + :coins WHERE players.id=:id");
                QSqlQuery query(db);
                query.prepare(cmd);
                query.bindValue(":coins", round(amount * koef));
                query.bindValue(":id", player_id);
                exec(query);

                bot.getApi().sendMessage(player_id, fmt::format("{} vs {}({})\nBet: {} on {}({})\nResult: {}\nCoins change: +{}", team1.toStdString(), team2.toStdString(), 
                matchResultIDToName[match_result_id], amount, matchResultIDToName[match_result_id_from_bets], koef, Emojis::CHECK_MARK, round(amount * koef)));
            }
            else
            {
                bot.getApi().sendMessage(player_id, fmt::format("{} vs {}({})\nBet: {} on {}({})\nResult: {}\nCoins change: -{}", team1.toStdString(), team2.toStdString(), 
                matchResultIDToName[match_result_id], amount, matchResultIDToName[match_result_id_from_bets], koef, Emojis::CROSS_MARK, amount));
            }
        }     
    }

    return true;
}
