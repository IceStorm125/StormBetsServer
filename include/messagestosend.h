#ifndef MESSAGES_H
#define MESSAGES_H

#include <string_view>

namespace Emojis {
    inline constexpr std::string_view ROCKET = "\xF0\x9F\x9A\x80";
    inline constexpr std::string_view MONEY = "\xF0\x9F\x92\xB0";
    inline constexpr std::string_view FOLDED_HANDS = "\xF0\x9F\x99\x8F";
    inline constexpr std::string_view CHECK_MARK = "\xE2\x9C\x85";
    inline constexpr std::string_view CROSS_MARK = "\xE2\x9D\x8C";
    inline constexpr std::string_view BACK_MARK = "\xF0\x9F\x94\x99";
    inline constexpr std::string_view STAT = "\xF0\x9F\x93\x88";
    inline constexpr std::string_view TROPHY = "\xF0\x9F\x8F\x86";
    inline constexpr std::string_view MONEY_WITH_WINGS = "\xF0\x9F\x92\xB8";
    inline constexpr std::string_view DOLLAR_MONEY = "\xF0\x9F\x92\xB5";
} // namespace Emojis


namespace Messages {
    // Формируем строки в runtime, чтобы избежать consteval ошибок
    inline const std::string PLACE_BET =
            std::string(Emojis::ROCKET) + " Place a bet";

    inline const std::string CURRENT_BETS =
            std::string(Emojis::FOLDED_HANDS) + " My current bets";

    inline const std::string COINS =
            std::string(Emojis::MONEY) + " My coins";

    inline const std::string PLAYED_BETS =
            std::string(Emojis::CHECK_MARK) + " Played bets";

    inline const std::string DELETE_BET =
            std::string(Emojis::BACK_MARK) + " Return bet";

    inline const std::string STATS =
            std::string(Emojis::STAT) + " My statistics";

    inline constexpr std::string_view LOGIN =
            "You are not logged in, please, use /start command";

    inline constexpr std::string_view LOADING = "Loading matches...";
    inline constexpr std::string_view CHOOSE_OPTION = "Please, choose option";
    inline constexpr std::string_view CHOOSE_MATCH = "Please, pick match number";
    inline constexpr std::string_view CHOOSE_BET_TO_DELETE =
            "Please, pick bet number. You will receive a penalty of 10% of the bet amount";
    inline constexpr std::string_view NO_MATCHES = "No upcomming matches";
    inline constexpr std::string_view LARGE_NUMBER = "Too large number";
    inline constexpr std::string_view CHOOSE_WINNER = "Choose a winner";
    inline constexpr std::string_view INPUT_AMOUNT = "Input bet amount";
    inline constexpr std::string_view NO_BETS = "You don't have bets";
    inline constexpr std::string_view NO_COINS = "You don't have that coins";
    inline constexpr std::string_view CONFIRM_BET = "Confirm a bet";
    inline constexpr std::string_view CHOOSE_NUMBER =
            "Please, choose a number from a given keyboard";

    inline const std::string CONFIRM =
            std::string(Emojis::CHECK_MARK) + " Confirm";

    inline const std::string RESET =
            std::string(Emojis::CROSS_MARK) + " Reset";

    inline const std::string CONFIRMED =
            "Bet confirmed " + std::string(Emojis::CHECK_MARK);

    inline const std::string RESETED =
            "Bet reseted " + std::string(Emojis::CROSS_MARK);
} // namespace Messages

#endif // MESSAGES_H
