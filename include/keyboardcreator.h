#ifndef KEYBOARDCREATOR_H
#define KEYBOARDCREATOR_H

#include <tgbot/tgbot.h>
#include <vector>

class KeyboardCreator
{
public:
    KeyboardCreator();

    static void createKeyboard(const std::vector<std::string> &buttonStrings, TgBot::ReplyKeyboardMarkup::Ptr &kb, int columns = 4);
    static void createOneColumnKeyboard(const std::vector<std::string> &buttonStrings, TgBot::ReplyKeyboardMarkup::Ptr &kb);
};

#endif // KEYBOARDCREATOR_H
