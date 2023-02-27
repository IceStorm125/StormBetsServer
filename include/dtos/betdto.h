#ifndef BETDTO_H
#define BETDTO_H

#include <string>
#include "dto.h"
#include "processing.h"
#include "match.h"

class BetDTO : public DTO
{
public:
    BetDTO(int chatID_);


    bool confirm(const Processing &bet);

    std::string playerCurrentBets();
    std::string playerPlayedBets();


private:
    int chatID;

    int getMatchResultID(const Processing &bet);
    int getMatchID(const Processing &bet);
    void addMatch(const Processing &bet);



};



#endif // BETDTO_H
