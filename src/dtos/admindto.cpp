

#include <QDebug>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>

#include <sstream>

#include "dtos/playerdto.h"
#include "dtos/admindto.h"

AdminDTO::AdminDTO()
{

}

std::string AdminDTO::getAllMatchesWithoutResult()
{
    QString cmd = "SELECT id, team1, team2, time FROM matches WHERE match_result_id IS NULL";
    QSqlQuery query;
    query.prepare(cmd);
    exec(query);

    std::stringstream ss;
    while (query.next())
    {
        QSqlRecord record = query.record();
        int id = record.value(0).toInt();
        QString team1 = record.value(1).toString();
        QString team2 = record.value(2).toString();
        QString time = record.value(3).toString();

        ss << id <<"| "<< team1.toStdString() << " vs " << team2.toStdString() << " | " << time.toStdString() << "\n";
    }

    return ss.str();
}

bool AdminDTO::updateResult(int matchID, const std::string &result)
{
    QString cmd("UPDATE matches SET matches.match_result_id = (SELECT id FROM match_results WHERE match_results.name = :result) WHERE id = :matchID");
    QSqlQuery query;
    query.prepare(cmd);
    query.bindValue(":matchID", matchID);
    query.bindValue(":result", QString::fromStdString(result));
    exec(query);

    return updateCoins(matchID);
}

bool AdminDTO::updateCoins(int matchID)
{
    QString selectBetsCmd("SELECT amount, koef, match_result_id, player_id FROM bets WHERE match_id = :matchID");
    QSqlQuery selectBetsQuery;
    selectBetsQuery.prepare(selectBetsCmd);
    selectBetsQuery.bindValue(":matchID", matchID);
    exec(selectBetsQuery);

    while(selectBetsQuery.next())
    {
        QSqlRecord record = selectBetsQuery.record();
        int amount = record.value(0).toInt();      
        double koef = record.value(1).toDouble();
        int match_result_id_from_bets = record.value(2).toInt();
        long player_id = record.value(3).toLongLong();

        QString selectIDcmd = "SELECT match_result_id FROM matches WHERE id = :matchID";
        QSqlQuery selectIDquery;
        selectIDquery.prepare(selectIDcmd);
        selectIDquery.bindValue(":matchID", matchID);
        exec(selectIDquery);

        if(selectIDquery.next())
        {
            QSqlRecord record = selectIDquery.record();
            int match_result_id = record.value(0).toInt();

            if(match_result_id_from_bets == match_result_id)
            {
                PlayerDTO dto(player_id);
                dto.updateCoins(dto.getCoins() + round(amount * koef));
            }
        }
    }

    return true;
}
