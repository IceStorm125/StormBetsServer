#include "dtos/matchdto.h"

#include <QDebug>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>


std::vector<Match> MatchDTO::getAllMatches()
{
    QString cmd = "SELECT m.id, m.api_id, t.title, m.team1, m.team2, m.koef_W1, m.koef_X, m.koef_W2, m.time FROM matches m "
                   "JOIN tournaments t ON t.id = m.tournament_id "
                   "WHERE m.match_result_id IS NULL "
                   "ORDER BY m.time;";
    QSqlQuery query;
    query.prepare(cmd);
    exec(query);

    std::vector<Match> matches;

    while (query.next())
    {
        QSqlRecord record = query.record();
        int id = record.value(0).toInt();
        QString api_id = record.value(1).toString();
        QString title = record.value(2).toString();
        QString team1 = record.value(3).toString();
        QString team2 = record.value(4).toString();
        double koef_W1 = record.value(5).toDouble();
        double koef_X = record.value(6).toDouble();
        double koef_W2 = record.value(7).toDouble();
        QDateTime time = record.value(8).toDateTime();
        Match match(id, api_id, title, team1, team2, koef_W1, koef_X, koef_W2, time);
        matches.push_back(match);
    }
    return matches;
}