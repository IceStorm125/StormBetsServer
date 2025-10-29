#ifndef MATCHDTO_H
#define MATCHDTO_H

#include <string>
#include <map>
#include <vector>

#include "dto.h"
#include "match.h"

class MatchDTO : public DTO
{
public:
    std::vector<Match> getAllMatches();

private:


};



#endif // MATCHDTO_H
