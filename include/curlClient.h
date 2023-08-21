#ifndef CURLCLIENT_H
#define CURLCLIENT_H

#include "client.h"

class CURLClient : public Client
{
public:
    CURLClient();
    ~CURLClient();

    int getJsonDataFromURL(const std::string &URL, std::string &outResponce) override;

private:
    CURL* curl;
};

#endif // CURLCLIENT_H
