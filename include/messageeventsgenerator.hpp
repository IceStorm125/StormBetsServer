#ifndef MESSAGEEVENTSGENERATOR_H
#define MESSAGEEVENTSGENERATOR_H

#include <vector>
#include <iostream>
#include <sstream>
#include <type_traits>

#include "event.h"

template<typename T, typename = std::enable_if_t<std::is_base_of_v<Event, T>>>
class MessageEventsGenerator
{
public:

    MessageEventsGenerator()
    {
    }

    void generateMessage(std::vector<T> &matches, std::string &outMessage, std::vector<std::string> &outNumsForKB)
    {
        std::stringstream matchesToSend;
        outNumsForKB.reserve(matches.size());
        for(size_t i = 0; i < matches.size(); ++i)
        {
            matchesToSend << (i + 1) << ". " << matches.at(i).toPrint();
            outNumsForKB.push_back(std::to_string(i + 1));
        }
        outMessage = matchesToSend.str();
    }
};



#endif // MESSAGEEVENTSGENERATOR_H
