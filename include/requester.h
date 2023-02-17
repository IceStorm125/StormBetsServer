#ifndef REQUESTER_H
#define REQUESTER_H

#include <string>
#include "curl/curl.h"

class Requester
{
public:
    Requester();

    int getJsonDataFromURL(const std::string &URL, std::string &outResponce);

private:
    CURL* curl;

};

#endif // REQUESTER_H
