//
// Created by igor on 04.03.19.
//

#ifndef C_CLIENT_FILEHOLDER_H
#define C_CLIENT_FILEHOLDER_H

#include <iostream>
#include <fstream>
#include <string>
#include "Action.h"

using std::ifstream;
using std::string;

class FileHolder {
private:
    const char *filename;
public:
    FileHolder(const char *filename);
    Action readFile();
};


#endif //C_CLIENT_FILEHOLDER_H
