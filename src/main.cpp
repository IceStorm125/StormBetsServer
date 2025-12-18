#include <QCoreApplication>
#include <QDebug>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QPluginLoader>

#include <unordered_map>
#include <vector>
#include <algorithm>
#include <thread>
#include <memory>

#include <boost/algorithm/string.hpp>

#include "tgbot/tgbot.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "processing.h"
#include "keyboardcreator.h"
#include "messagestosend.h"
#include "messageeventsgenerator.h"
#include "EnvManager.hpp"
#include "regexmatcher.h"
#include "dtos/playerdto.h"
#include "dtos/betdto.h"
#include "dtos/admindto.h"
#include "dtos/matchdto.h"


void runBot(TgBot::Bot &bot) {
    TgBot::TgLongPoll longPoll(bot);

    int retryDelay = 1;
    const int retryDelayMax = 60;

    while (true) {
        try {
            longPoll.start();
            retryDelay = 1;
        } catch (TgBot::TgException &e) {
            spdlog::error("Telegram API error: {}", e.what());
        } catch (std::runtime_error &e) {
            spdlog::error("Runtime error: {}", e.what());
        } catch (std::exception &e) {
            spdlog::error("Unexpected error: {}", e.what());
        }

        std::this_thread::sleep_for(std::chrono::seconds(retryDelay));

        retryDelay = std::min(retryDelay * 2, retryDelayMax);
    }
}

void cleanupExpiredSessions(
    std::unordered_map<long, std::chrono::system_clock::time_point> &userSessions,
    std::unordered_map<long, std::unique_ptr<Processing> > &currentProcesses,
    std::mutex &sessionMutex,
    const std::chrono::seconds &timeout = std::chrono::minutes(10)
) {
    auto now = std::chrono::system_clock::now();
    std::vector<long> keysToRemove;
    keysToRemove.reserve(userSessions.size());

    std::lock_guard m(sessionMutex);
    std::erase_if(userSessions, [&](auto &entry) {
        auto &[id, lastTimeVisited] = entry;

        if (now - lastTimeVisited > timeout) {
            keysToRemove.push_back(id);
            return true;
        }
        return false;
    });

    for (long id: keysToRemove) {
        currentProcesses.erase(id);
        spdlog::info("User {} session expired", id);
    }
}

int main(int argc, char *argv[]) {
    using namespace TgBot;

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("StormBetsServerLog.txt", false);

    std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
    auto logger = std::make_shared<spdlog::logger>("main_logger", sinks.begin(), sinks.end());

    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);
    spdlog::flush_on(spdlog::level::info);

    spdlog::info("Loadding .env file...");

    auto &env = EnvManager::getInstance();
    if (!env.isLoaded()) {
        spdlog::error("Failed to load .env file");
        return 1;
    }
    spdlog::info("Loaded .env file");
    std::optional<std::string> TOKEN = env.getValue("BOT_TOKEN");

    if (!TOKEN.has_value()) {
        spdlog::error("BOT_TOKEN not found in .env file");
        return EXIT_FAILURE;
    }

    Bot bot(TOKEN.value());

    std::unordered_map<long, std::unique_ptr<Processing> > currentProceses;
    std::unordered_map<long, std::chrono::time_point<std::chrono::system_clock> > userSessions;
    std::mutex sessionMutex;

    ReplyKeyboardRemove::Ptr ptrForRemoveKeyboard(new ReplyKeyboardRemove);

    ReplyKeyboardMarkup::Ptr menuKeyboard(new ReplyKeyboardMarkup);
    KeyboardCreator::createKeyboard({
                                        std::string(Messages::PLACE_BET), std::string(Messages::CURRENT_BETS),
                                        std::string(Messages::PLAYED_BETS),
                                        std::string(Messages::COINS), std::string(Messages::DELETE_BET),
                                        std::string(Messages::STATS)
                                    }, menuKeyboard, 2);

    std::vector<BotCommand::Ptr> commands;
    BotCommand::Ptr cmdArray1(new BotCommand);
    cmdArray1->command = "start";
    cmdArray1->description = "Start using bot";
    commands.push_back(cmdArray1);
    bot.getApi().setMyCommands(commands);


    bot.getEvents().onCommand(
        "start", [&bot, &currentProceses, &menuKeyboard, &userSessions, &sessionMutex](Message::Ptr message) {
            spdlog::info("User {} wrote command: {}", message->chat->id, message->text);

            const long chatID{message->chat->id};

            bot.getApi().sendMessage(chatID, std::string(Messages::CHOOSE_OPTION), nullptr, nullptr, menuKeyboard);

            {
                std::lock_guard m(sessionMutex);

                if (auto it = currentProceses.find(chatID); it != currentProceses.end())
                    currentProceses.erase(it);

                PlayerDTO dto(chatID);
                dto.add(message->chat->firstName, message->chat->lastName);
                currentProceses[chatID] = std::make_unique<Processing>(chatID);
                userSessions[chatID] = std::chrono::system_clock::now();
            }
        });

    bot.getEvents().onAnyMessage(
        [&bot, &currentProceses, &userSessions, &sessionMutex, &ptrForRemoveKeyboard, &menuKeyboard](
    Message::Ptr message) {
            spdlog::info("User {} wrote message: {}", message->chat->id, message->text);
            const long chatID{message->chat->id};

            {
                std::lock_guard m(sessionMutex);
                userSessions[chatID] = std::chrono::system_clock::now();
            }

            if (QString::fromStdString(message->text).startsWith("/"))
                return;

            if (auto it = currentProceses.find(chatID); it == currentProceses.end()) {
                bot.getApi().sendMessage(chatID, std::string(Messages::LOGIN), nullptr, nullptr, ptrForRemoveKeyboard);
                return;
            } else {
                Processing &current = *(it->second);
                switch (current.getStatus()) {
                    case Processing::Status::START: {
                        if (message->text == std::string(Messages::PLACE_BET)) {
                            bot.getApi().sendMessage(chatID, std::string(Messages::LOADING), nullptr, nullptr,
                                                     ptrForRemoveKeyboard);
                            MatchDTO mdto;
                            std::vector<Match> matches = mdto.getAllMatches();
                            if (matches.empty()) {
                                bot.getApi().sendMessage(chatID, std::string(Messages::NO_MATCHES), nullptr, nullptr,
                                                         menuKeyboard);
                                break;
                            }

                            bot.getApi().sendMessage(chatID, std::string(Messages::CHOOSE_MATCH), nullptr, nullptr,
                                                     ptrForRemoveKeyboard);

                            std::vector<std::string> numbersToSend;
                            numbersToSend.reserve(matches.size());
                            std::string outMsg = MessageEventsGenerator::generateAllMatches(matches, numbersToSend);

                            ReplyKeyboardMarkup::Ptr kb(new ReplyKeyboardMarkup);
                            KeyboardCreator::createKeyboard(numbersToSend, kb);
                            bot.getApi().sendMessage(chatID, outMsg, nullptr, nullptr, kb);

                            current.setUserMatches(matches);
                            current.setStatus(Processing::Status::CHOOSING_MATCH);
                            break;
                        }
                        if (message->text == std::string(Messages::CURRENT_BETS)) {
                            BetDTO dto(chatID);
                            const std::string playerCurrentBetsStr = dto.playerCurrentBets();
                            bot.getApi().sendMessage(
                                chatID, playerCurrentBetsStr == ""
                                            ? std::string(Messages::NO_BETS)
                                            : playerCurrentBetsStr);
                        }
                        if (message->text == std::string(Messages::COINS)) {
                            PlayerDTO dto(chatID);
                            const int coins = dto.getCoins();
                            bot.getApi().sendMessage(chatID, std::to_string(coins));
                        }
                        if (message->text == std::string(Messages::PLAYED_BETS)) {
                            BetDTO dto(chatID);
                            const std::string playerPlayedBetsStr = dto.playerPlayedBets(10);
                            bot.getApi().sendMessage(
                                chatID,
                                playerPlayedBetsStr == "" ? std::string(Messages::NO_BETS) : playerPlayedBetsStr);
                        }
                        if (message->text == std::string(Messages::DELETE_BET)) {
                            BetDTO dto(chatID);

                            std::map<int, int> betNumbersToID;
                            std::string playerCurrentBetsStr = dto.playerCurrentBetsToDelete(betNumbersToID);

                            if (playerCurrentBetsStr.empty()) {
                                bot.getApi().sendMessage(chatID, std::string(Messages::NO_BETS));
                            } else {
                                bot.getApi().sendMessage(chatID, std::string(Messages::CHOOSE_BET_TO_DELETE), nullptr,
                                                         nullptr,
                                                         ptrForRemoveKeyboard);
                                current.setMatchNumberToID(betNumbersToID);

                                std::vector<std::string> numbersToSend;
                                numbersToSend.reserve(betNumbersToID.size());
                                std::for_each(betNumbersToID.cbegin(), betNumbersToID.cend(),
                                              [&numbersToSend](auto const &m) {
                                                  numbersToSend.push_back(std::to_string(m.first));
                                              });

                                current.setStatus(Processing::Status::DELETTING_BET);

                                ReplyKeyboardMarkup::Ptr kb(new ReplyKeyboardMarkup);
                                KeyboardCreator::createOneColumnKeyboard(numbersToSend, kb);
                                bot.getApi().sendMessage(chatID, playerCurrentBetsStr, nullptr, nullptr, kb);
                                break;
                            }
                        }
                        if (message->text == std::string(Messages::STATS)) {
                            PlayerDTO dto(chatID);
                            std::string statsStr = dto.getStats();
                            bot.getApi().sendMessage(chatID, statsStr, nullptr, nullptr, menuKeyboard);
                            break;
                        }

                        bot.getApi().sendMessage(chatID, std::string(Messages::CHOOSE_OPTION), nullptr, nullptr,
                                                 menuKeyboard);

                        break;
                    }
                    case Processing::Status::CHOOSING_MATCH: {
                        if (RegexMatcher::isStringPositiveNumber(message->text) && std::stoi(message->text) <=
                            static_cast<
                                int>(current.getUserMatches().size())) {
                            const std::vector<Match> matches = current.getUserMatches();

                            const Match match = matches.at(std::stoi(message->text) - 1);
                            current.setMatch(match);
                            current.setStatus(Processing::Status::CHOOSING_WINNER);
                            ReplyKeyboardMarkup::Ptr kb(new ReplyKeyboardMarkup);

                            if (match.getKoefDraw() != 0)
                                KeyboardCreator::createOneColumnKeyboard({
                                                                             match.getTeam1().first,
                                                                             "Draw",
                                                                             match.getTeam2().first
                                                                         }, kb);
                            else
                                KeyboardCreator::createOneColumnKeyboard({
                                                                             match.getTeam1().first,
                                                                             match.getTeam2().first
                                                                         }, kb);

                            bot.getApi().sendMessage(chatID, std::string(Messages::CHOOSE_WINNER), nullptr, nullptr,
                                                     kb);
                        } else {
                            bot.getApi().sendMessage(chatID, std::string(Messages::CHOOSE_NUMBER));
                        }

                        break;
                    }
                    case Processing::Status::CHOOSING_WINNER: {
                        if (const Match match = current.getMatch();
                            message->text == match.getTeam1().first) {
                            current.setResult(Processing::Result::W1);
                            current.setKoef(match.getTeam1().second);
                        } else if (message->text == match.getTeam2().first) {
                            current.setResult(Processing::Result::W2);
                            current.setKoef(match.getTeam2().second);
                        } else if (message->text == "Draw") {
                            current.setResult(Processing::Result::X);
                            current.setKoef(match.getKoefDraw());
                        } else {
                            bot.getApi().sendMessage(chatID, std::string(Messages::CHOOSE_WINNER));
                            break;
                        }
                        bot.getApi().sendMessage(chatID, std::string(Messages::INPUT_AMOUNT), nullptr, nullptr,
                                                 ptrForRemoveKeyboard);
                        current.setStatus(Processing::Status::CHOOSING_AMOUNT);

                        break;
                    }
                    case Processing::Status::CHOOSING_AMOUNT: {
                        if (message->text.size() > 10) {
                            bot.getApi().sendMessage(chatID, std::string(Messages::LARGE_NUMBER), nullptr, nullptr,
                                                     ptrForRemoveKeyboard);
                            break;
                        }
                        if (!RegexMatcher::isStringPositiveNumber(message->text)) {
                            bot.getApi().sendMessage(chatID, std::string(Messages::INPUT_AMOUNT), nullptr, nullptr,
                                                     ptrForRemoveKeyboard);
                            break;
                        }

                        PlayerDTO dto(chatID);
                        const int coins = dto.getCoins();
                        const int amount = std::stoi(message->text);

                        if (coins < amount) {
                            bot.getApi().sendMessage(chatID, fmt::format("You have only {}{}", coins, Emojis::MONEY),
                                                     nullptr, nullptr, ptrForRemoveKeyboard);
                            break;
                        }

                        current.setAmount(amount);
                        current.setStatus(Processing::Status::ACCEPTING);

                        bot.getApi().sendMessage(chatID, std::string(Messages::CONFIRM_BET));
                        ReplyKeyboardMarkup::Ptr kb(new ReplyKeyboardMarkup);
                        KeyboardCreator::createOneColumnKeyboard({
                                                                     std::string(Messages::CONFIRM),
                                                                     std::string(Messages::RESET)
                                                                 }, kb);
                        bot.getApi().sendMessage(chatID, current.toPrint(), nullptr, nullptr, kb);
                        break;
                    }
                    case Processing::Status::ACCEPTING: {
                        if (message->text == std::string(Messages::CONFIRM)) {
                            BetDTO dto(chatID);
                            if (dto.confirm(current)) {
                                PlayerDTO pdto(chatID);
                                const int coins = pdto.getCoins();
                                pdto.updateCoins(coins - current.getAmount());
                                bot.getApi().sendMessage(chatID, std::string(Messages::CONFIRMED), nullptr, nullptr,
                                                         ptrForRemoveKeyboard);
                            }
                        } else if (message->text == std::string(Messages::RESET)) {
                            bot.getApi().sendMessage(chatID, std::string(Messages::RESETED), nullptr, nullptr,
                                                     ptrForRemoveKeyboard);
                        } else {
                            break;
                        }
                        current.reset();
                        bot.getApi().sendMessage(chatID, std::string(Messages::CHOOSE_OPTION), nullptr, nullptr,
                                                 menuKeyboard);

                        break;
                    }
                    case Processing::Status::DELETTING_BET: {
                        if (!RegexMatcher::isStringPositiveNumber(message->text)) {
                            bot.getApi().sendMessage(chatID, "It's not a number");
                            break;
                        }

                        if (current.getMatchNumberToID().find(std::stoi(message->text)) == current.getMatchNumberToID().
                            cend()) {
                            ReplyKeyboardMarkup::Ptr kb(new ReplyKeyboardMarkup);
                            bot.getApi().sendMessage(chatID, "There is no bet with this number");
                            break;
                        }

                        BetDTO bdto(chatID);
                        const int betIdToDelete = current.getMatchNumberToID().at(std::stoi(message->text));
                        const int amount = bdto.getBetAmountByID(betIdToDelete);

                        bdto.deleteBetByID(betIdToDelete);

                        PlayerDTO pdto(chatID);
                        constexpr double penaltyKoef = 0.1;
                        const int returnAmount = amount * (1.0 - penaltyKoef);
                        pdto.updateCoins(pdto.getCoins() + returnAmount);

                        std::string returnAmountstr = std::to_string(returnAmount);
                        returnAmountstr.erase(returnAmountstr.find_last_not_of('.') + 1, std::string::npos);
                        bot.getApi().sendMessage(chatID, "Bet deleted. You got " + returnAmountstr + " coins back.");

                        current.reset();
                        bot.getApi().sendMessage(chatID, std::string(Messages::CHOOSE_OPTION), nullptr, nullptr,
                                                 menuKeyboard);

                        break;
                    }
                }
            }
        });


    signal(SIGINT, [](int s) {
        printf("SIGINT got\n");
        exit(0);
    });

    auto registerResultsThread = std::thread([&bot]() {
        while (true) {
            AdminDTO dto;
            dto.registerResults(bot);
            std::this_thread::sleep_for(std::chrono::minutes(1));
        }
    });

    auto userSessionTread = std::thread([&userSessions, &currentProceses, &sessionMutex] {
        while (true) {
            cleanupExpiredSessions(userSessions, currentProceses, sessionMutex);
            std::this_thread::sleep_for(std::chrono::minutes(1));
        }
    });

    try {
        bot.getApi().deleteWebhook();
        runBot(bot);
    } catch (std::exception &e) {
        spdlog::critical("Fatal error: {}", e.what());
    }

    registerResultsThread.join();
    userSessionTread.join();

    return 0;
}
