#ifndef JSONDATAEXTRACTOR_H
#define JSONDATAEXTRACTOR_H

#include <vector>
#include <optional>

#include "requester.h"
#include "match.h"
class JsonDataExtractor
{
public:
    JsonDataExtractor();

    const std::string TOURNAMENTS_URL{"https://www.rivalry.com/api/v1/tournaments/"};
    const std::string MATCH_URL{"https://www.rivalry.com/api/v1/matches/"};

    const QString COMPETITOR_1{"{$competitor1}"};
    const QString COMPETITOR_2{"{$competitor2}"};
    const QString DRAW{"draw"};

    std::vector<Match> getUpcomingMatchesByTournamentID(int ID);
    std::optional<Match> getMatchByID(int ID);

private:
    Requester requester;

};

#endif // JSONDATAEXTRACTOR_H
