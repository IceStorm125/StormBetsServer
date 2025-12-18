#include "keyboardcreator.h"

KeyboardCreator::KeyboardCreator() {
}

void KeyboardCreator::createKeyboard(const std::vector<std::string> &buttonStrings, TgBot::ReplyKeyboardMarkup::Ptr &kb,
                                     int columns) {
    std::vector<TgBot::KeyboardButton::Ptr> row;
    for (size_t i = 0; i < buttonStrings.size(); ++i) {
        TgBot::KeyboardButton::Ptr button(new TgBot::KeyboardButton);
        button->text = buttonStrings.at(i);
        button->requestContact = false;
        button->requestLocation = false;
        row.push_back(button);
        if ((i + 1) % columns == 0 || i == buttonStrings.size() - 1) {
            kb->keyboard.push_back(row);
            row.clear();
        }
    }
    kb->oneTimeKeyboard = true;
    kb->resizeKeyboard = true;
}

void KeyboardCreator::createOneColumnKeyboard(const std::vector<std::string> &buttonStrings,
                                              TgBot::ReplyKeyboardMarkup::Ptr &kb) {
    for (size_t i = 0; i < buttonStrings.size(); ++i) {
        std::vector<TgBot::KeyboardButton::Ptr> row;
        TgBot::KeyboardButton::Ptr button(new TgBot::KeyboardButton);
        button->text = buttonStrings.at(i);
        button->requestContact = false;
        button->requestLocation = false;
        row.push_back(button);
        kb->keyboard.push_back(row);
    }
    kb->oneTimeKeyboard = true;
    kb->resizeKeyboard = true;
}
