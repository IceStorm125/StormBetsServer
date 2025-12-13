#ifndef MESSAGEEVENTSGENERATOR_H
#define MESSAGEEVENTSGENERATOR_H

#include <vector>
#include <string>

#include "match.h"

class MessageEventsGenerator {
public:
    static std::string generateAllMatches(std::vector<Match> &events,
                                          std::vector<std::string> &outNumsForKB);
};


#endif // MESSAGEEVENTSGENERATOR_H
