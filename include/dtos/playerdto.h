#ifndef PLAYERDTO_H
#define PLAYERDTO_H
#include <QSqlQuery>
#include <QSqlDatabase>

#include "dto.h"

class PlayerDTO : public DTO
{
public:
    PlayerDTO(int chatID_);

    int getCoins();
    bool updateCoins(int coins);
    bool add(const std::string &firstName, const std::string &lastName);

private:
    int chatID;

    const int START_COINS = 5000;
};

#endif // PLAYERDTO_H
