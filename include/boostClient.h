#ifndef BOOSTCLIENT_H
#define BOOSTCLIENT_H

#include "client.h"

class BoostClient : public Client
{
public:
    BoostClient();
    ~BoostClient();

    int getJsonDataFromURL(const std::string &URL, std::string &outResponce) override;
};

#endif // BOOSTCLIENT_H
