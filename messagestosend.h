#ifndef MESSAGES_H
#define MESSAGES_H

#include <string>

namespace Emojis {
const std::string ROCKET = "\xF0\x9F\x9A\x80";
const std::string MONEY = "\xF0\x9F\x92\xB0";
const std::string FOLDED_HANDS = "\xF0\x9F\x99\x8F";
const std::string CHECK_MARK = "\xE2\x9C\x85";
const std::string CROSS_MARK = "\xE2\x9D\x8C";
}

namespace Messages {
const std::string PLACE_BET = Emojis::ROCKET + " Place a bet";
const std::string CURRENT_BETS = Emojis::FOLDED_HANDS + " My current bets";
const std::string COINS = Emojis::MONEY + " My coins";
const std::string LOGIN = "You are not logged in, please, use /start command";
const std::string CHOOSE_OPTION = "Please, choose option";
const std::string CHOOSE_MATCH = "Please, pick match number";
const std::string NO_MATCHES = "No upcomming matches";
const std::string CHOOSE_WINNER = "Choose a winner";
const std::string INPUT_AMOUNT = "Input bet amount";
const std::string CONFIRM_BET = "Confirm a bet";
const std::string CHOOSE_NUMBER = "Please, choose a number from a given keyboard";
const std::string CONFIRM = Emojis::CHECK_MARK + " Confirm";
const std::string RESET = Emojis::CROSS_MARK + " Reset";
const std::string CONFIRMED = "Bet confirmed " + Emojis::CHECK_MARK;
const std::string RESETED = "Bet reseted " + Emojis::CROSS_MARK;
}

#endif // MESSAGES_H
