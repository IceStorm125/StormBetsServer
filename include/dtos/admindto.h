#ifndef ADMINDTO_H
#define ADMINDTO_H

#include "dto.h"

class AdminDTO : public DTO
{
public:
    AdminDTO();

    std::string  getAllMatchesWithoutResult();
    bool updateResult(int matchID,const std::string &result);
    bool updateCoins(int matchID);
};

#endif // ADMINDTO_H
