#ifndef PROCESSING_H
#define PROCESSING_H

#include <vector>
#include <string>
#include "match.h"

class Processing
{
public:
    Processing(long userID_);

    enum Status
    {
        START,
        CHOOSING_MATCH,
        CHOOSING_WINNER,
        CHOOSING_AMOUNT,
        ACCEPTING,
        CHECK_CURRENT_BETS,
        CHECK_COINS
    };

    enum Result
    {
        W1,
        X,
        W2
    };

    int getUserID() const;
    void setUserID(int value);

    int getStatus() const;
    void setStatus(int value);

    int getResult() const;
    void setResult(int value);

    int getAmount() const;
    void setAmount(int value);

    std::vector<Match> getUserMatches() const;
    void setUserMatches(const std::vector<Match> &value);

    Match getMatch() const;
    void setMatch(const Match &value);

    double getKoef() const;
    void setKoef(double value);

    std::string toPrint();
    void reset();



private:
    int userID;
    int status;
    std::vector<Match> userMatches;
    Match match;
    int result;
    double koef;
    int amount;
};

#endif // PROCESSING_H
