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
    std::string getStats();

private:
    int chatID;

    constexpr static int START_COINS = 5000;

    int getCountAll();
    int getCountWin();
    int getTotalSpent();
    int getTotalGain();
};

#endif // PLAYERDTO_H
