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

    long getUserID() const;
    void setUserID(long value);

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

    std::string getPrintedBet();
    void reset();

private:
    long userID;
    int status;
    std::vector<Match> userMatches;
    Match match;
    int result;
    int amount;



};

#endif // PROCESSING_H
