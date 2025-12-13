#ifndef MATCHDTO_H
#define MATCHDTO_H

#include <vector>
#include <memory>

#include "dto.h"
#include "match.h"

class MatchDTO : public DTO {
public:
    std::vector<Match> getAllMatches();
};


#endif // MATCHDTO_H
