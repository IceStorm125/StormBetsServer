#include "jsondataextractor.h"

#include <QtDebug>
#include <QString>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

#include <map>
#include <algorithm>

JsonDataExtractor::JsonDataExtractor()
{

}

std::vector<Match> JsonDataExtractor::getUpcomingMatchesByTournamentID(int ID)
{
    std::string URL{TOURNAMENTS_URL};
    URL += std::to_string(ID);

    std::vector<Match> upcomingMatches;

    if(std::string matchesResponce; requester.getJsonDataFromURL(URL, matchesResponce) == 200)
    {
        QString qstrResponce = QString::fromUtf8(matchesResponce.c_str());
        QJsonDocument jsonResponce = QJsonDocument::fromJson(qstrResponce.toUtf8());
        QJsonObject jsonObject = jsonResponce.object().value("data").toObject();
        QJsonArray jsonArrayMatches = jsonObject.value("matches").toArray();
        for(const QJsonValue &match : jsonArrayMatches)
        {
            QDateTime matchTime = QDateTime::fromString(match["scheduled_at"].toString(), Qt::ISODate).toLocalTime();
            if(matchTime > QDateTime::currentDateTime())
            {
                int matchID = match["id"].toInt();
                QPair<Match, bool> matchObj = getMatchByID(matchID);

                if(matchObj.second)
                    upcomingMatches.push_back(matchObj.first);
            }
        }
    }
    return upcomingMatches;
}

QPair<Match, bool> JsonDataExtractor::getMatchByID(int ID)
{
    std::string URL{MATCH_URL};
    URL += std::to_string(ID);

    if(std::string matchResponce; requester.getJsonDataFromURL(URL, matchResponce) == 200)
    {
        QString qstrResponce = QString::fromUtf8(matchResponce.c_str());
        QJsonDocument jsonResponce = QJsonDocument::fromJson(qstrResponce.toUtf8());
        QJsonObject jsonObject = jsonResponce.object().value("data").toObject();
        QJsonArray jsonArrayMarkets = jsonObject.value("markets").toArray();

        auto it = std::find_if(jsonArrayMarkets.constBegin(), jsonArrayMarkets.constEnd(), [](const QJsonValue &market ){
            return market["name"].toString() == "Winner" && !market["is_live"].toBool();
        });

        if(it != jsonArrayMarkets.constEnd())
        {
            QJsonArray jsonArrayCompetitors = jsonObject.value("competitors").toArray();
            std::map<QString, QString> teamsNames;
            teamsNames.insert({"{$competitor1}", jsonArrayCompetitors.at(0)["name"].toString()});
            teamsNames.insert({"{$competitor2}", jsonArrayCompetitors.at(1)["name"].toString()});

            QJsonValue jsonValueOdds = *it;
            QJsonArray jsonOutcomes = jsonValueOdds["outcomes"].toArray();

            QString name1 = teamsNames.at(jsonOutcomes.at(0)["name"].toString());
            QString name2 = teamsNames.at(jsonOutcomes.at(1)["name"].toString());

            double koef1 = jsonOutcomes.at(0)["odds"].toDouble();
            double koef2 = jsonOutcomes.at(1)["odds"].toDouble();

            QDateTime matchTime = QDateTime::fromString(jsonObject["scheduled_at"].toString(), Qt::ISODate).addSecs(3*60*60);
            return {Match(ID, name1, name2, koef1, koef2, matchTime), true};
        }
    }

    return {Match(), false};
}


