#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include "curl/curl.h"

class Client
{
public:
    virtual ~Client() {};

    virtual int getJsonDataFromURL(const std::string &URL, std::string &outResponce) = 0;



};

#endif // CLIENT_H
