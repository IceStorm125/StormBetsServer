#include "regexmatcher.h"
#include <regex>

;

RegexMatcher::RegexMatcher()
{

}

bool RegexMatcher::isStringPositiveNumber(const std::string &str)
{
    const std::regex POSITIVE_INT {"[1-9][0-9]*"};
    return std::regex_match(str, POSITIVE_INT);
}
