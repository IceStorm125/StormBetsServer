#include <QCoreApplication>
#include <QDebug>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QPluginLoader>

#include <map>
#include <vector>
#include <algorithm>
#include <thread>

#include <boost/algorithm/string.hpp>

#include "tgbot/tgbot.h"
#include "spdlog/spdlog.h"


#include "processing.h"
#include "event.h"
#include "keyboardcreator.h"
#include "messagestosend.h"
#include "messageeventsgenerator.hpp"
#include "EnvManager.hpp"
#include "regexmatcher.h"
#include "dtos/playerdto.h"
#include "dtos/betdto.h"
#include "dtos/admindto.h"
#include "dtos/matchdto.h"



int main(int argc, char *argv[])
{
    using namespace TgBot;

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("StormBetsServerLog.txt", false);

    std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};
    auto logger = std::make_shared<spdlog::logger>("main_logger", sinks.begin(), sinks.end());

    spdlog::register_logger(logger); 
    spdlog::set_default_logger(logger); 

    spdlog::info("Loadding .env file...");
    
    auto& env = EnvManager::getInstance();
    if(!env.isLoaded()) {
        spdlog::error("Failed to load .env file");
        return 1;
    }
    spdlog::info("Loaded .env file");
    std::optional<std::string> TOKEN = env.getValue("BOT_TOKEN");

    if(!TOKEN.has_value()) 
    {
        spdlog::error("BOT_TOKEN not found in .env file");
        return EXIT_FAILURE;
    }

    Bot bot(TOKEN.value());

    std::map<long, std::unique_ptr<Processing>> currentProceses;

    ReplyKeyboardRemove::Ptr ptrForRemoveKeyboard(new ReplyKeyboardRemove);

    ReplyKeyboardMarkup::Ptr menuKeyboard(new ReplyKeyboardMarkup);
    KeyboardCreator::createKeyboard({Messages::PLACE_BET, Messages::CURRENT_BETS,  Messages::PLAYED_BETS, Messages::COINS, Messages::DELETE_BET, Messages::STATS}, menuKeyboard, 2);

    std::vector<BotCommand::Ptr> commands;
    BotCommand::Ptr cmdArray1(new BotCommand);
    cmdArray1->command = "start";
    cmdArray1->description = "Start using bot";
    commands.push_back(cmdArray1);
    bot.getApi().setMyCommands(commands);


    bot.getEvents().onCommand("start", [&bot, &currentProceses, &menuKeyboard](Message::Ptr message) {
        spdlog::info("User {} wrote command: {}", message->chat->id, message->text);

        const long chatID{message->chat->id};

        bot.getApi().sendMessage(chatID, Messages::CHOOSE_OPTION, 0, false, menuKeyboard);

        if(auto it = currentProceses.find(chatID); it != currentProceses.end())
            currentProceses.erase(it);

        PlayerDTO dto(chatID);
        dto.add(message->chat->firstName, message->chat->lastName);
        currentProceses[chatID] = std::make_unique<Processing>(chatID);
    });

    bot.getEvents().onAnyMessage([&bot, &currentProceses, &ptrForRemoveKeyboard, &menuKeyboard](Message::Ptr message) {
        spdlog::info("User {} wrote message: {}", message->chat->id, message->text);
        const long chatID{message->chat->id};

        if(QString::fromStdString(message->text).startsWith("/"))
            return;

        if(auto it = currentProceses.find(chatID); it == currentProceses.end())
        {
            bot.getApi().sendMessage(chatID, Messages::LOGIN, false, 0, ptrForRemoveKeyboard);
            return;
        }
        else
        {
            Processing &current = *(it->second);
            switch (current.getStatus())
            {
            case Processing::Status::START:{
                if(message->text == Messages::PLACE_BET)
                {
                    bot.getApi().sendMessage(chatID, Messages::LOADING, 0, false, ptrForRemoveKeyboard);
                    MatchDTO mdto;
                    std::vector<Match> matches = mdto.getAllMatches();
                    if(matches.empty())
                    {
                        bot.getApi().sendMessage(chatID, Messages::NO_MATCHES, 0, false, menuKeyboard);
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

                    current.setUserMatches(matches);
                    current.setStatus(Processing::Status::CHOOSING_MATCH);
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
                if(message->text == Messages::PLAYED_BETS)
                {
                    BetDTO dto(chatID);
                    const std::string  playerPlayedBetsStr = dto.playerPlayedBets(10);
                    bot.getApi().sendMessage(chatID, playerPlayedBetsStr == "" ? Messages::NO_BETS : playerPlayedBetsStr);
                }
                if(message->text == Messages::DELETE_BET)
                {
                    BetDTO dto(chatID);

                    std::map<int, int> betNumbersToID;
                    std::string playerCurrentBetsStr = dto.playerCurrentBetsToDelete(betNumbersToID);

                    if(playerCurrentBetsStr.empty())
                    {
                        bot.getApi().sendMessage(chatID, Messages::NO_BETS);
                    }
                    else
                    {
                        bot.getApi().sendMessage(chatID, Messages::CHOOSE_BET_TO_DELETE, 0, false, ptrForRemoveKeyboard);
                        current.setMatchNumberToID(betNumbersToID);

                        std::vector<std::string> numbersToSend;
                        numbersToSend.reserve(betNumbersToID.size());
                        std::for_each(betNumbersToID.cbegin(), betNumbersToID.cend(), [&numbersToSend](auto const &m){
                            numbersToSend.push_back(std::to_string(m.first));
                        });

                        current.setStatus(Processing::Status::DELETTING_BET);

                        ReplyKeyboardMarkup::Ptr kb(new ReplyKeyboardMarkup);
                        KeyboardCreator::createOneColumnKeyboard(numbersToSend, kb);
                        bot.getApi().sendMessage(chatID, playerCurrentBetsStr, false, 0, kb);
                        break;
                    }
                }
                if(message->text == Messages::STATS)
                {
                    PlayerDTO dto(chatID);
                    std::string statsStr = dto.getStats();
                    bot.getApi().sendMessage(chatID, statsStr, 0, false, menuKeyboard);
                    break;
                }

                bot.getApi().sendMessage(chatID, Messages::CHOOSE_OPTION, 0, false, menuKeyboard);

                break;
            }
            case Processing::Status::CHOOSING_MATCH:{
                if(RegexMatcher::isStringPositiveNumber(message->text) && std::stoi(message->text) <= static_cast<int>(current.getUserMatches().size()))
                {
                    const std::vector<Match> matches = current.getUserMatches();

                    const Match match = matches.at(std::stoi(message->text) - 1);
                    current.setMatch(match);
                    current.setStatus(Processing::Status::CHOOSING_WINNER);
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
                if(const Match match = current.getMatch(); message->text == match.getTeam1().first.toStdString())
                {
                    current.setResult(Processing::Result::W1);
                    current.setKoef(match.getTeam1().second);
                }
                else if(message->text == match.getTeam2().first.toStdString())
                {
                    current.setResult(Processing::Result::W2);
                    current.setKoef(match.getTeam2().second);
                }
                else if(message->text == "Draw")
                {
                    current.setResult(Processing::Result::X);
                    current.setKoef(match.getKoefDraw());
                }
                else
                {
                    bot.getApi().sendMessage(chatID, Messages::CHOOSE_WINNER);
                    break;
                }
                bot.getApi().sendMessage(chatID, Messages::INPUT_AMOUNT, 0, false, ptrForRemoveKeyboard);
                current.setStatus(Processing::Status::CHOOSING_AMOUNT);

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
                const int coins = dto.getCoins();
                const int amount = std::stoi(message->text);

                if(coins < amount)
                {
                    bot.getApi().sendMessage(chatID, "You have only " + std::to_string(coins) + Emojis::MONEY, 0, false, ptrForRemoveKeyboard);
                    break;
                }

                current.setAmount(amount);
                current.setStatus(Processing::Status::ACCEPTING);

                bot.getApi().sendMessage(chatID, Messages::CONFIRM_BET);
                ReplyKeyboardMarkup::Ptr kb(new ReplyKeyboardMarkup);
                KeyboardCreator::createOneColumnKeyboard({Messages::CONFIRM, Messages::RESET}, kb);
                bot.getApi().sendMessage(chatID, current.toPrint(), 0, false, kb);
                break;
            }
            case Processing::Status::ACCEPTING:{
                if(message->text == Messages::CONFIRM)
                {
                    BetDTO dto(chatID);
                    if(dto.confirm(current))
                    {
                        PlayerDTO pdto(chatID);
                        const int coins = pdto.getCoins();
                        pdto.updateCoins(coins - current.getAmount());
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
                current.reset();
                bot.getApi().sendMessage(chatID, Messages::CHOOSE_OPTION, 0, false, menuKeyboard);

                break;

            }
            case Processing::Status::DELETTING_BET: {             
                if(!RegexMatcher::isStringPositiveNumber(message->text))
                {
                    bot.getApi().sendMessage(chatID, "It's not a number");
                    break;
                }

                if(current.getMatchNumberToID().find(std::stoi(message->text)) == current.getMatchNumberToID().cend())
                {
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
                returnAmountstr.erase (returnAmountstr.find_last_not_of('.') + 1, std::string::npos);
                bot.getApi().sendMessage(chatID, "Bet deleted. You got " + returnAmountstr + " coins back.");

                current.reset();
                bot.getApi().sendMessage(chatID, Messages::CHOOSE_OPTION, 0, false, menuKeyboard);

                break;
                }
            }
        }
    });


    signal(SIGINT, [](int s) {
        printf("SIGINT got\n");
        exit(0);
    });

    auto t = std::thread([&bot](){
        while (true)
        {
            AdminDTO dto;
            dto.registerResults(bot);
            std::this_thread::sleep_for(std::chrono::minutes(1));
        }
    });
    t.detach();

    try
    {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();

        TgLongPoll longPoll(bot);

        while (true)
        {
            // qInfo() << "Long poll started";
            longPoll.start();
        }


    } catch (std::exception& e)
    {
        spdlog::error(e.what());
    }


    return 0;
}
