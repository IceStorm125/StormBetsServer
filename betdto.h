#ifndef BETDTO_H
#define BETDTO_H

#include <string>
#include "dto.h"
#include "processing.h"

class BetDTO : public DTO
{
public:
    BetDTO(int chatID_);


    bool confirm(const Processing &bet);

    std::string playerCurrentBets();

private:
    int chatID;

    int getMatchResultID(const Processing &bet);
    int getMatchID(const Processing &bet);
    void addMatch(const Processing &bet);
};



#endif // BETDTO_H
