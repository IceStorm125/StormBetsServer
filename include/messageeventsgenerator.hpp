#ifndef MESSAGEEVENTSGENERATOR_H
#define MESSAGEEVENTSGENERATOR_H

#include <vector>
#include <iostream>
#include <sstream>
#include <type_traits>

#include "event.h"

template<typename T>
class MessageEventsGenerator
{
public:

    MessageEventsGenerator();

    void generateMessage(std::vector<T> &matches, std::string &outMessage, std::vector<std::string> &outNumsForKB);
};

template <typename T>
MessageEventsGenerator<T>::MessageEventsGenerator()
{
    static_assert(std::is_base_of<Event, T>::value, "Event class is not base of T!");
}

template <typename T>
void MessageEventsGenerator<T>::generateMessage(std::vector<T> &matches, std::string &outMessage, std::vector<std::string> &outNumsForKB)
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


#endif // MESSAGEEVENTSGENERATOR_H
