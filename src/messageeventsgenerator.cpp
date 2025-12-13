
#include "messageeventsgenerator.h"
#include "match.h"

#include <sstream>

std::string MessageEventsGenerator::generateAllMatches(std::vector<Match> &events,
                                                       std::vector<std::string> &outNumsForKB) {
    std::stringstream matchesToSend;
    outNumsForKB.reserve(events.size());
    for (size_t i = 0; i < events.size(); ++i) {
        matchesToSend << (i + 1) << ". " << events.at(i).toPrint();
        outNumsForKB.push_back(std::to_string(i + 1));
    }
    return matchesToSend.str();
}
