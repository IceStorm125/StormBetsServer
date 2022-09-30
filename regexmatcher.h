#ifndef REGEXMATCHER_H
#define REGEXMATCHER_H

#include <string>
#include <regex>

class RegexMatcher
{
public:
    RegexMatcher();

    static bool isStringPositiveNumber(const std::string &str);
};

#endif // REGEXMATCHER_H
