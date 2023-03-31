#ifndef PROCESSING_H
#define PROCESSING_H

#include <vector>
#include <map>
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
        CHECK_COINS,
        DELETTING_BET
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

    std::map<int, int> getMatchNumberToID() const;
    void setMatchNumberToID(const std::map<int, int> &value);

    std::string toPrint();
    void reset();



    int getMatchToDeleteNumber() const;
    void setMatchToDeleteNumber(int value);

private:
    int userID;
    int status;
    std::vector<Match> userMatches;
    Match match;
    int result;
    double koef;
    int amount;
    int matchToDeleteNumber;
    std::map<int, int> matchNumberToID;

};

#endif // PROCESSING_H
