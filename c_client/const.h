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

#define SET_LOW_LETTERS "Маленькие буквы"
#define SET_BIG_LETTERS "Большие буквы"

struct Set
{
    std::string name;
    char *set;
    int length;
};
typedef struct Set Set;

const Set low_letters = {
        SET_LOW_LETTERS,
        new char[26] {
            'a','b','c','d','e','f','g','h','i',
            'j','k','l','m','n','o','p','q','r',
            's','t','u','v','w','x','y','z'
        },
        26
};

const Set big_letters = {
        SET_BIG_LETTERS,
        new char[26] {
            'A','B','C','D','E','F','G','H','I',
            'J','K','L','M','N','O','P','Q','R',
            'S','T','U','V','W','X','Y','Z'
        },
        26
};

#endif //C_CLIENT_CONST_H
