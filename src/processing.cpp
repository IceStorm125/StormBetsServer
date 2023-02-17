#include "processing.h"

#include <sstream>

Processing::Processing(long userID_)
{
    userID = userID_;
    status = Status::START;
}

int Processing::getUserID() const
{
    return userID;
}

void Processing::setUserID(int value)
{
    userID = value;
}

int Processing::getStatus() const
{
    return status;
}

void Processing::setStatus(int value)
{
    status = value;
}


int Processing::getResult() const
{
    return result;
}

void Processing::setResult(int value)
{
    result = value;
}

int Processing::getAmount() const
{
    return amount;
}

void Processing::setAmount(int value)
{
    amount = value;
}

std::vector<Match> Processing::getUserMatches() const
{
    return userMatches;
}

void Processing::setUserMatches(const std::vector<Match> &value)
{
    userMatches = value;
}

Match Processing::getMatch() const
{
    return match;
}

void Processing::setMatch(const Match &value)
{
    match = value;
}

std::string Processing::getPrintedBet()
{
    std::stringstream ss;
    ss << match.toPrint();
    ss << "Winner: " << (result == Processing::Result::W1 ? match.getTeam1().first.toStdString() : (result == Processing::Result::W2 ? match.getTeam2().first.toStdString() : "Draw")) << "\n";
    ss << "Amount: " << amount;

    return ss.str();
}

void Processing::reset()
{
    status = Processing::Status::START;
    userMatches.clear();
    match = Match();
    result = -1;
    amount = 0;
    koef = 0;
}

double Processing::getKoef() const
{
    return koef;
}

void Processing::setKoef(double value)
{
    koef = value;
}


