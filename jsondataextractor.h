#ifndef JSONDATAEXTRACTOR_H
#define JSONDATAEXTRACTOR_H

#include <vector>

#include "requester.h"
#include "match.h"
class JsonDataExtractor
{
public:
    JsonDataExtractor();

    const std::string TOURNAMENTS_URL{"https://www.rivalry.com/api/v1/tournaments/"};
    const std::string MATCH_URL{"https://www.rivalry.com/api/v1/matches/"};

    std::vector<Match> getUpcomingMatchesByTournamentID(int ID);
    QPair<Match, bool> getMatchByID(int ID);

private:
    Requester requester;

};

#endif // JSONDATAEXTRACTOR_H
