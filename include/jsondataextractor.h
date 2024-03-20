#ifndef JSONDATAEXTRACTOR_H
#define JSONDATAEXTRACTOR_H

#include <vector>
#include <optional>
#include <memory>

#include "match.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;


class JsonDataExtractor
{
public:
    JsonDataExtractor();

    std::vector<Match> getUpcomingMatchesByTournamentID(int ID);
    std::optional<Match> getMatchByID(int ID);

private:
    void addMatchToVectorOfMatches(std::vector<Match> &matches, const json &match);

private:

    const std::string TOURNAMENTS_URL{"https://api.rivalry.com/api/v1/tournaments/"};
    const std::string MATCH_URL{"https://api.rivalry.com/api/v1/matches/"};

    const QString COMPETITOR_1{"{$competitor1}"};
    const QString COMPETITOR_2{"{$competitor2}"};
    const QString DRAW{"draw"};

};

#endif // JSONDATAEXTRACTOR_H
