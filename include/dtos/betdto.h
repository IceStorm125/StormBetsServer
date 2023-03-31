#ifndef BETDTO_H
#define BETDTO_H

#include <string>
#include <map>

#include "dto.h"
#include "processing.h"
#include "match.h"

class BetDTO : public DTO
{
public:
    BetDTO(int chatID_);


    bool confirm(const Processing &bet);
    bool deleteBetByID(int id);
    int getBetAmountByID(int id);

    std::string playerCurrentBets();
    std::string playerCurrentBetsToDelete(std::map<int, int> &betNumberToID);
    std::string playerPlayedBets(int limit);


private:
    int chatID;

    int getMatchResultID(const Processing &bet);
    int getMatchID(const Processing &bet);
    void addMatch(const Processing &bet);



};



#endif // BETDTO_H
