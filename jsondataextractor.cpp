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
#include <iostream>
#include <thread>

#include "requester.h"

JsonDataExtractor::JsonDataExtractor()
{

}

std::vector<Match> JsonDataExtractor::getUpcomingMatchesByTournamentID(int ID)
{
    std::string URL{TOURNAMENTS_URL};
    URL += std::to_string(ID);

    std::vector<Match> upcomingMatches;

    Requester requester;
    if(std::string matchesResponce; requester.getJsonDataFromURL(URL, matchesResponce) == 200)
    {
        QString qstrResponce = QString::fromUtf8(matchesResponce.c_str());
        QJsonDocument jsonResponce = QJsonDocument::fromJson(qstrResponce.toUtf8());
        QJsonObject jsonObject = jsonResponce.object().value("data").toObject();
        QJsonArray jsonArrayMatches = jsonObject.value("matches").toArray();

        std::vector<std::thread> threads;

        for(const QJsonValue &match : jsonArrayMatches)
        {
            threads.push_back(std::thread(&JsonDataExtractor::addMatchToVectorOfMatches, this, std::ref(upcomingMatches), match));
        }

        for(auto &thread: threads)
        {
            thread.join();
        }
    }
    std::sort(upcomingMatches.begin(), upcomingMatches.end(),[](const Match &m1, const Match &m2){
        return m1.getTime() <= m2.getTime();
    });
    return upcomingMatches;
}

void JsonDataExtractor::addMatchToVectorOfMatches(std::vector<Match> &matches, QJsonValue match)
{
    QDateTime matchTime = QDateTime::fromString(match["scheduled_at"].toString(), Qt::ISODate).toLocalTime();

    if(matchTime > QDateTime::currentDateTime())
    {
        int matchID = match["id"].toInt();
        std::optional<Match> matchObj = getMatchByID(matchID);

        if(matchObj)
        {
            matches.push_back(matchObj.value());
        }
    }
}

std::optional<Match> JsonDataExtractor::getMatchByID(int ID)
{
    std::string URL{MATCH_URL};
    URL += std::to_string(ID);

    Requester requester;
    if(std::string matchResponce; requester.getJsonDataFromURL(URL, matchResponce) == 200)
    {
        QString qstrResponce = QString::fromUtf8(matchResponce.c_str());
        QJsonDocument jsonResponce = QJsonDocument::fromJson(qstrResponce.toUtf8());
        QJsonObject jsonObject = jsonResponce.object().value("data").toObject();
        QJsonArray jsonArrayMarkets = jsonObject.value("markets").toArray();

        auto it = std::find_if(jsonArrayMarkets.constBegin(), jsonArrayMarkets.constEnd(), [](const QJsonValue &market ){
            return (market["name"].toString() == "1x2" || market["name"].toString() == "Winner") && !market["is_live"].toBool();
        });

        if(it != jsonArrayMarkets.constEnd())
        {
            QJsonArray jsonArrayCompetitors = jsonObject.value("competitors").toArray();

            if(!jsonArrayCompetitors.size())
                return std::nullopt;

            std::map<QString, QString> teamsNames;
            teamsNames.insert({COMPETITOR_1, jsonArrayCompetitors.at(0)["name"].toString()});
            teamsNames.insert({COMPETITOR_2, jsonArrayCompetitors.at(1)["name"].toString()});

            QJsonValue jsonValueOdds = *it;
            QJsonArray jsonOutcomes = jsonValueOdds["outcomes"].toArray();
            if(!jsonOutcomes.size())
                return std::nullopt;

            auto itName1 = std::find_if(jsonOutcomes.constBegin(), jsonOutcomes.constEnd(), [this](const QJsonValue &outcome) {
                return outcome["name"] == COMPETITOR_1;
            });

            auto itName2 = std::find_if(jsonOutcomes.constBegin(), jsonOutcomes.constEnd(), [this](const QJsonValue &outcome) {
                return outcome["name"] == COMPETITOR_2;
            });

            auto itDraw = std::find_if(jsonOutcomes.constBegin(), jsonOutcomes.constEnd(), [this](const QJsonValue &outcome) {
                return outcome["name"] == DRAW;
            });

            size_t name1index = std::distance(jsonOutcomes.constBegin(), itName1);
            size_t name2index = std::distance(jsonOutcomes.constBegin(), itName2);
            size_t nameDraw = std::distance( jsonOutcomes.constBegin(), itDraw);

            QString name1 = teamsNames.find(COMPETITOR_1)->second;
            QString name2 = teamsNames.find(COMPETITOR_2)->second;

            double koefDraw = jsonOutcomes.at(nameDraw)["odds"].toDouble();
            double koef1 = jsonOutcomes.at(name1index)["odds"].toDouble();
            double koef2 = jsonOutcomes.at(name2index)["odds"].toDouble();

            QDateTime matchTime = QDateTime::fromString(jsonObject["scheduled_at"].toString(), Qt::ISODate).addSecs(3*60*60);
            return Match(ID, name1, name2, koef1, koef2, koefDraw, matchTime);
        }
    }
    return std::nullopt;
}


