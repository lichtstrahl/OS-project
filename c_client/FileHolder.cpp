//
// Created by igor on 04.03.19.
//

#include "FileHolder.h"
#include "const.h"
#include <string.h>
FileHolder::FileHolder(const char *filename) {
    this->filename = filename;
}

/**
 * Возвращается прочитанное действие. Если произошла ошикбка,
 * то id у действия будет равняться -1
 * @return
 */
Action FileHolder::readFile() {
    string line;
    ifstream file(filename);

    if (file.is_open()) {

        // Считываем id
        getline(file, line);
        int id = (int)strtol(line.c_str(), nullptr, 10);
        // Считываем текст
        getline(file, line);
        ActionType type = ActionType::NONE;
        if (!strcmp(line.c_str(), ATYPE_LEFT))
            type = ActionType::CLICK_LEFT;
        if (!strcmp(line.c_str(), ATYPE_RIGHT))
            type = ActionType::CLICK_RIGHT;
        if (!strcmp(line.c_str(), ATYPE_MIDDLE))
            type = ActionType::CLICK_MIDDLE;
        if (!strcmp(line.c_str(), ATYPE_UP))
            type = ActionType::SCROLL_UP;
        if (!strcmp(line.c_str(), ATYPE_DOWN))
            type = ActionType::SCROLL_DOWN;
        if (!strcmp(line.c_str(), ATYPE_SIDE_1))
            type = ActionType::SIDE_1;
        if (!strcmp(line.c_str(), ATYPE_SIDE_2))
            type = ActionType::SIDE_2;

        Action action = {id, type, line.c_str()};
        file.close();
        return action;
    } else {
        SEND_MSG(MSG_FILE_INFO_NOT_FOUND);
    }

    return Action{-1, ActionType::NONE, nullptr};
}