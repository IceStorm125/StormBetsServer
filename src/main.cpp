#include <QCoreApplication>
#include <QDebug>
#include <QTest>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QPluginLoader>

#include <vector>
#include <algorithm>

#include <boost/algorithm/string.hpp>

#include "tgbot/tgbot.h"

#include "processing.h"
#include "jsondataextractor.h"
#include "event.h"
#include "keyboardcreator.h"
#include "messagestosend.h"
#include "messageeventsgenerator.hpp"
#include "regexmatcher.h"
#include "dtos/playerdto.h"
#include "dtos/betdto.h"
#include "dtos/admindto.h"
#include "tests/test_regexmatcher.h"

enum Tournaments
{
    ENGLISH_LEAGUE = 3465,
    CHAMPIONS_LEAGUE = 3465,
    LIMA_MAJOR = 7200
};

int main(int argc, char *argv[])
{
    using namespace TgBot;

    const std::string TOKEN = "5197186113:AAFtm1pkRcWgssxEgW2XphepTkIGWDCHNx0";
    const long ADMIN_ID = 427038898;
    bool adminIsWorking = false;

    Bot bot(TOKEN);


    JsonDataExtractor extractor;
    std::vector<Processing> currentProceses;

    ReplyKeyboardRemove::Ptr ptrForRemoveKeyboard(new ReplyKeyboardRemove);

    std::vector<BotCommand::Ptr> commands;
    BotCommand::Ptr cmdArray1(new BotCommand);
    BotCommand::Ptr cmdArray2(new BotCommand);
    cmdArray1->command = "start";
    cmdArray1->description = "Start using bot";
    cmdArray2->command = "admin";
    cmdArray2->description = "Only for admin";
    commands.push_back(cmdArray1);
    commands.push_back(cmdArray2);
    bot.getApi().setMyCommands(commands);

    bot.getEvents().onCommand("start", [&bot, &currentProceses](Message::Ptr message) {
        const long chatID{message->chat->id};

        ReplyKeyboardMarkup::Ptr kb(new ReplyKeyboardMarkup);
        KeyboardCreator::createOneColumnKeyboard({Messages::PLACE_BET, Messages::CURRENT_BETS, Messages::COINS}, kb);
        bot.getApi().sendMessage(chatID, Messages::CHOOSE_OPTION, 0, false, kb);

        if(auto it = std::remove_if(currentProceses.begin(), currentProceses.end(),[chatID](Processing &p){ return chatID == p.getUserID(); }); it != currentProceses.end())
            currentProceses.erase(it, currentProceses.end());

        PlayerDTO dto(chatID);
        dto.add(message->chat->firstName, message->chat->lastName);
        currentProceses.push_back(Processing(chatID));
    });

    bot.getEvents().onCommand("admin", [&bot, &adminIsWorking, &ptrForRemoveKeyboard](Message::Ptr message) {
        const long chatID{message->chat->id};

        if(chatID != ADMIN_ID)
            return;

        adminIsWorking = true;

        AdminDTO dto;
        std::string matchesInfo = dto.getAllMatchesWithoutResult();
        bot.getApi().sendMessage(chatID, "Using: <matchID> <W1|W2|X>", false, 0, ptrForRemoveKeyboard);
        bot.getApi().sendMessage(chatID, matchesInfo.size() ? matchesInfo : "No matches", false, 0, ptrForRemoveKeyboard);
    });

    bot.getEvents().onAnyMessage([&bot, &extractor, &currentProceses, &ptrForRemoveKeyboard, &adminIsWorking](Message::Ptr message) {
        const long chatID{message->chat->id};

        if(QString::fromStdString(message->text).startsWith("/"))
            return;

        if(adminIsWorking && chatID == ADMIN_ID)
            return;

        if(auto it = std::find_if(currentProceses.begin(), currentProceses.end(),[chatID](Processing &p){ return chatID == p.getUserID(); }); it == currentProceses.end())
        {
            bot.getApi().sendMessage(chatID, Messages::LOGIN, false, 0, ptrForRemoveKeyboard);
            return;
        }
        else
        {
            switch (it->getStatus())
            {
            case Processing::Status::START:{
                if(message->text == Messages::PLACE_BET)
                {
                    it->setStatus(Processing::Status::CHOOSING_MATCH);
                    bot.getApi().sendMessage(chatID, Messages::LOADING, 0, false, ptrForRemoveKeyboard);
                    std::vector<Match> matches = extractor.getUpcomingMatchesByTournamentID(Tournaments::CHAMPIONS_LEAGUE);
                    if(matches.empty())
                    {
                        bot.getApi().sendMessage(chatID, Messages::NO_MATCHES, 0, false, ptrForRemoveKeyboard);
                        break;
                    }

                    bot.getApi().sendMessage(chatID, Messages::CHOOSE_MATCH, 0, false, ptrForRemoveKeyboard);

                    std::vector<std::string> numbersToSend;
                    numbersToSend.reserve(matches.size());
                    MessageEventsGenerator<Match> gen;
                    std::string outMsg;
                    gen.generateMessage(matches, outMsg, numbersToSend);

                    ReplyKeyboardMarkup::Ptr kb(new ReplyKeyboardMarkup);
                    KeyboardCreator::createKeyboard(numbersToSend, kb);
                    bot.getApi().sendMessage(chatID, outMsg, 0, false, kb);

                    it->setUserMatches(matches);
                    it->setStatus(Processing::Status::CHOOSING_MATCH);
                    break;
                }
                if (message->text == Messages::CURRENT_BETS)
                {
                    BetDTO dto(chatID);
                    const std::string  playerCurrentBetsStr = dto.playerCurrentBets();
                    bot.getApi().sendMessage(chatID, playerCurrentBetsStr == "" ? Messages::NO_BETS : playerCurrentBetsStr);
                }
                if (message->text == Messages::COINS)
                {
                    PlayerDTO dto(chatID);
                    const int coins = dto.getCoins();
                    bot.getApi().sendMessage(chatID, std::to_string(coins));
                }

                ReplyKeyboardMarkup::Ptr kb(new ReplyKeyboardMarkup);
                KeyboardCreator::createOneColumnKeyboard({Messages::PLACE_BET, Messages::CURRENT_BETS, Messages::COINS}, kb);
                bot.getApi().sendMessage(chatID, Messages::CHOOSE_OPTION, 0, false, kb);

                break;
            }
            case Processing::Status::CHOOSING_MATCH:{
                if(RegexMatcher::isStringPositiveNumber(message->text) && std::stoi(message->text) <= static_cast<int>(it->getUserMatches().size()))
                {
                    const std::vector<Match> matches = it->getUserMatches();

                    const Match match = matches.at(std::stoi(message->text) - 1);
                    it->setMatch(match);
                    it->setStatus(Processing::Status::CHOOSING_WINNER);

                    ReplyKeyboardMarkup::Ptr kb(new ReplyKeyboardMarkup);

                    if(match.getKoefDraw() != 0)
                        KeyboardCreator::createOneColumnKeyboard({match.getTeam1().first.toStdString(), "Draw", match.getTeam2().first.toStdString()}, kb);
                    else
                        KeyboardCreator::createOneColumnKeyboard({match.getTeam1().first.toStdString(), match.getTeam2().first.toStdString()}, kb);

                    bot.getApi().sendMessage(chatID, Messages::CHOOSE_WINNER, 0, false, kb);
                }
                else
                {
                    bot.getApi().sendMessage(chatID, Messages::CHOOSE_NUMBER);
                }

                break;
            }
            case Processing::Status::CHOOSING_WINNER:{
                if(const Match match = extractor.getMatchByID(it->getMatch().getID()).value(); message->text == match.getTeam1().first.toStdString())
                {
                    it->setResult(Processing::Result::W1);
                    it->setKoef(match.getTeam1().second);
                }
                else if(message->text == match.getTeam2().first.toStdString())
                {
                    it->setResult(Processing::Result::W2);
                    it->setKoef(match.getTeam2().second);
                }
                else if(message->text == "Draw")
                {
                    it->setResult(Processing::Result::X);
                    it->setKoef(match.getKoefDraw());
                }
                else
                {
                    bot.getApi().sendMessage(chatID, Messages::CHOOSE_WINNER);
                    break;
                }
                bot.getApi().sendMessage(chatID, Messages::INPUT_AMOUNT, 0, false, ptrForRemoveKeyboard);
                it->setStatus(Processing::Status::CHOOSING_AMOUNT);

                break;
            }
            case Processing::Status::CHOOSING_AMOUNT:{
                if(message->text.size() > 10)
                {
                    bot.getApi().sendMessage(chatID, Messages::LARGE_NUMBER, 0, false, ptrForRemoveKeyboard);
                    break;
                }
                if(!RegexMatcher::isStringPositiveNumber(message->text))
                {
                    bot.getApi().sendMessage(chatID, Messages::INPUT_AMOUNT, 0, false, ptrForRemoveKeyboard);
                    break;
                }

                PlayerDTO dto(chatID);
                int coins = dto.getCoins();
                int amount = std::stoi(message->text);

                if(coins < amount)
                {
                    bot.getApi().sendMessage(chatID, Messages::NO_COINS);
                    break;
                }

                it->setAmount(amount);
                it->setStatus(Processing::Status::ACCEPTING);

                bot.getApi().sendMessage(chatID, Messages::CONFIRM_BET);
                ReplyKeyboardMarkup::Ptr kb(new ReplyKeyboardMarkup);
                KeyboardCreator::createOneColumnKeyboard({Messages::CONFIRM, Messages::RESET}, kb);
                bot.getApi().sendMessage(chatID, it->getPrintedBet(), 0, false, kb);
                break;
            }
            case Processing::Status::ACCEPTING:{
                if(message->text == Messages::CONFIRM)
                {
                    BetDTO dto(chatID);
                    if(dto.confirm(*it))
                    {
                        PlayerDTO pdto(chatID);
                        int coins = pdto.getCoins();
                        pdto.updateCoins(coins - it->getAmount());
                        bot.getApi().sendMessage(chatID, Messages::CONFIRMED, 0, false, ptrForRemoveKeyboard);
                    }
                }
                else if(message->text == Messages::RESET)
                {
                    bot.getApi().sendMessage(chatID, Messages::RESETED, 0, false, ptrForRemoveKeyboard);
                }
                else
                {
                    break;
                }
                it->reset();
                ReplyKeyboardMarkup::Ptr kb(new ReplyKeyboardMarkup);
                KeyboardCreator::createOneColumnKeyboard({Messages::PLACE_BET, Messages::CURRENT_BETS, Messages::COINS}, kb);
                bot.getApi().sendMessage(chatID, Messages::CHOOSE_OPTION, 0, false, kb);

                break;
            }
            }
        }
    });

    // Admin pannel
    bot.getEvents().onAnyMessage([&bot, &adminIsWorking](Message::Ptr message) {
        const long chatID{message->chat->id};

        if(chatID == ADMIN_ID && adminIsWorking)
        {
            std::string strMessage = message->text;

            if(strMessage == "End")
            {
                adminIsWorking = false;
                return;
            }

            boost::trim(strMessage);
            std::vector<std::string> values;
            boost::split(values, strMessage, boost::is_any_of(" "), boost::token_compress_on);

            if(values.size() != 2)
            {
                bot.getApi().sendMessage(chatID, "Bad input values");
                return;
            }
            if(!RegexMatcher::isStringPositiveNumber(values.at(0)))
            {
                bot.getApi().sendMessage(chatID, "Bad first value");
                return;
            }
            if(values.at(1) != "W1" && values.at(1) != "W2" && values.at(1) != "X")
            {
                bot.getApi().sendMessage(chatID, "Bad second value");
                return;
            }

            int matchID = std::stoi(values.at(0));
            std::string result = values.at(1);
            AdminDTO dto;
            dto.updateResult(matchID, result);
        }
    });

    signal(SIGINT, [](int s) {
        printf("SIGINT got\n");
        exit(0);
    });

    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();

        TgLongPoll longPoll(bot);

        while (true) {
            qInfo() << "Long poll started";
            longPoll.start();
        }
    } catch (std::exception& e) {
        qInfo() << "error: " << e.what();
    }

    return 0;
}
