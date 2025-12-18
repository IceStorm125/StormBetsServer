#include "processing.h"

#include <sstream>

#include <fmt/core.h>


Processing::Processing(long userID_) {
    userID = userID_;
    status = Status::START;
}

int Processing::getUserID() const {
    return userID;
}

void Processing::setUserID(int value) {
    userID = value;
}

int Processing::getStatus() const {
    return status;
}

void Processing::setStatus(int value) {
    status = value;
}


int Processing::getResult() const {
    return result;
}

void Processing::setResult(int value) {
    result = value;
}

int Processing::getAmount() const {
    return amount;
}

void Processing::setAmount(int value) {
    amount = value;
}

std::vector<Match> Processing::getUserMatches() const {
    return userMatches;
}

void Processing::setUserMatches(const std::vector<Match> &value) {
    userMatches = value;
}

Match Processing::getMatch() const {
    return match;
}

void Processing::setMatch(const Match &value) {
    match = value;
}

std::string Processing::toPrint() {
    std::string out = fmt::format(
        "{}"
        "Winner: {}\n"
        "Amount: {}",
        match.toPrint(),
        (result == Processing::Result::W1
             ? match.getTeam1().first
             : (result == Processing::Result::W2 ? match.getTeam2().first : "Draw")), amount);

    return out;
}

void Processing::reset() {
    status = Processing::Status::START;
    userMatches.clear();
    matchNumberToID.clear();
    match = Match();
    result = -1;
    amount = 0;
    koef = 0;
}

std::map<int, int> Processing::getMatchNumberToID() const {
    return matchNumberToID;
}

void Processing::setMatchNumberToID(const std::map<int, int> &value) {
    matchNumberToID = value;
}

double Processing::getKoef() const {
    return koef;
}

void Processing::setKoef(double value) {
    koef = value;
}


