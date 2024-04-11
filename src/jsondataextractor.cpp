#include "jsondataextractor.h"

#include <map>
#include <algorithm>
#include <iostream>
#include <memory>
#include <thread>

#include "curlClient.h"
#include "boostClient.h"

#include "spdlog/spdlog.h"


JsonDataExtractor::JsonDataExtractor()
{

}

std::vector<Match> JsonDataExtractor::getUpcomingMatchesByTournamentID(int ID)
{
    std::string URL{TOURNAMENTS_URL};
    URL += std::to_string(ID);

    std::vector<Match> upcomingMatches;

    std::unique_ptr<Client> client = std::make_unique<BoostClient>();
    if(std::string responce; client->getJsonDataFromURL(URL, responce) == 200)
    {
        json j = json::parse(responce);
        auto jsonArrayMatches = j["data"]["matches"].get<std::vector<json>>();

        std::vector<std::thread> threads;

        for(const json &match : jsonArrayMatches)
        {
            threads.push_back(std::thread(&JsonDataExtractor::addMatchToVectorOfMatches, this, std::ref(upcomingMatches), std::ref(match)));
        }

        std::for_each(threads.begin(),threads.end(), std::mem_fn(&std::thread::join));
    }
    std::sort(upcomingMatches.begin(), upcomingMatches.end(),[](const Match &m1, const Match &m2){
        return m1.getTime() <= m2.getTime();
    });
    return upcomingMatches;
}

void JsonDataExtractor::addMatchToVectorOfMatches(std::vector<Match> &matches, const json &match)
{
    QDateTime matchTime = QDateTime::fromString(QString::fromStdString(match["scheduled_at"].get<std::string>()), Qt::ISODate).toLocalTime();

    if(matchTime > QDateTime::currentDateTime())
    {
        int matchID = match["id"].get<int>();
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

    std::unique_ptr<Client> client = std::make_unique<BoostClient>();
    if(std::string responce; client->getJsonDataFromURL(URL, responce) == 200)
    {
        using json = nlohmann::json;
        json jsonResponce = json::parse(responce);
        json jsonObject = jsonResponce["data"];
        json jsonArrayMarkets = jsonObject["markets"];

        auto it = std::find_if(jsonArrayMarkets.begin(), jsonArrayMarkets.end(), [](const json &market ){
            return (market["name"] == "1x2" || market["name"] == "Winner") && !market["is_live"];
        });

        if(it != jsonArrayMarkets.end())
        {
            json jsonArrayCompetitors = jsonObject["competitors"];

            if(jsonArrayCompetitors.size() == 0)
                return std::nullopt;

            std::map<QString, QString> teamsNames;
            teamsNames.insert({COMPETITOR_1, QString::fromStdString(jsonArrayCompetitors.at(0)["name"])});
            teamsNames.insert({COMPETITOR_2, QString::fromStdString(jsonArrayCompetitors.at(1)["name"])});

            json jsonValueOdds = *it;
            json jsonOutcomes = jsonValueOdds["outcomes"];
            if(jsonOutcomes.size() == 0)
                return std::nullopt;

            auto itName1 = std::find_if(jsonOutcomes.begin(), jsonOutcomes.end(), [this](const json &outcome) {
                return outcome["name"] == COMPETITOR_1.toStdString();
            });

            auto itName2 = std::find_if(jsonOutcomes.begin(), jsonOutcomes.end(), [this](const json &outcome) {
                return outcome["name"] == COMPETITOR_2.toStdString();
            });

            auto itDraw = std::find_if(jsonOutcomes.begin(), jsonOutcomes.end(), [this](const json &outcome) {
                return outcome["name"] == DRAW.toStdString();
            });

            double koefDraw = 0;
            double koef1 = 0;
            double koef2 = 0;

            size_t name1index;
            if (itName1 != jsonOutcomes.end())
            {
                name1index = std::distance(jsonOutcomes.begin(), itName1);
                koef1 = jsonOutcomes.at(name1index)["odds"].get<double>();
            }

            size_t name2index;
            if (itName2 != jsonOutcomes.end())
            {
                name2index = std::distance(jsonOutcomes.begin(), itName2);
                koef2 = jsonOutcomes.at(name2index)["odds"].get<double>();
            }

            size_t nameDraw;
            if (itDraw != jsonOutcomes.end())
            {
                nameDraw = std::distance(jsonOutcomes.begin(), itDraw);
                koefDraw = jsonOutcomes.at(nameDraw)["odds"].get<double>();
            }

            QString name1 = teamsNames.find(COMPETITOR_1)->second;
            QString name2 = teamsNames.find(COMPETITOR_2)->second;

            QDateTime matchTime = QDateTime::fromString(QString::fromStdString(jsonObject["scheduled_at"].get<std::string>()), Qt::ISODate).addSecs(3*60*60);
            return Match(ID, name1, name2, koef1, koef2, koefDraw, matchTime);
        }
    }
    return std::nullopt;
}


