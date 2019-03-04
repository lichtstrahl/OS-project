//
// Created by igor on 04.03.19.
//

#ifndef C_CLIENT_CONST_H
#define C_CLIENT_CONST_H


/**
 * Сообщения
 */
#include <stdio.h>
#define SEND_MSG(msg) printf(msg "\n")
#define MSG_FILE_INFO_NOT_FOUND "Не удалось найти файл модуля. Вероятно он не был загружен или был отключен."
#define MSG_NEXT_CLICK_EXIT     "(Нажать на ПКМ, чтобы выйти)"

#define SET_LOW_LETTERS "Маленькие буквы"
#define SET_BIG_LETTERS "Большие буквы"
#define SET_SYMBOLS     "Символы"
#define SET_SPECIAL     "Специальные символы"

enum SetType {
    COMMON,
    SPECIFIC
};



struct Set
{
    std::string name;
    std::string *set;
    int length;
    SetType type;
};
typedef struct Set Set;

const Set low_letters = {
        SET_LOW_LETTERS,
        new string[26] {
            "a","b","c","d","e","f","g","h","i",
            "j","k","l","m","n","o","p","q","r",
            "s","t","u","v","w","x","y","z"
        },
        26,
        SetType::COMMON
};

const Set big_letters = {
        SET_BIG_LETTERS,
        new string[26] {
            "A","B","C","D","E","F","G","H","I",
            "J","K","L","M","N","O","P","Q","R",
            "S","T","U","V","W","X","Y","Z"
        },
        26,
        SetType::COMMON
};

const Set symbols = {
        SET_SYMBOLS,
        new string[10] {
            "-","=","+","*","\"",">","<",",",".","/"
        },
        10,
        SetType::COMMON
};


#define SPECIAL_SPACE           "[Space]"
#define SPECIAL_BACKSPACE       "[Backspace]"
const Set special = {
        SET_SPECIAL,
        new string[2] {
            SPECIAL_SPACE, SPECIAL_BACKSPACE
        },
        2,
        SetType::SPECIFIC
};

#define COUNT_AVAILABLE_SET 4
const Set availableSet[COUNT_AVAILABLE_SET] = {
        low_letters, big_letters,symbols, special
};

#endif //C_CLIENT_CONST_H
