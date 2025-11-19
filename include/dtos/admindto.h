#ifndef ADMINDTO_H
#define ADMINDTO_H

#include <vector>
#include <map>

#include "tgbot/tgbot.h"

#include "dto.h"

class AdminDTO : public DTO
{
public:
    AdminDTO();

    void registerResults(const TgBot::Bot &bot);

private:
    std::vector<int> getAllMatchesIdWithResult();
    bool updateCoins(int matchID, const TgBot::Bot &bot);
    
    std::map<int, std::string> matchResultIDToName {
        {1, "W1"},
        {2, "X"},
        {3, "W2"}
        };
};

#endif // ADMINDTO_H
