#include <iostream>
#include <unistd.h>
#include "timer.h"
#include "FileHolder.h"
#include <fstream>
#include "const.h"
#include <vector>

using std::ifstream;
using std::vector;


#define MAX_COUNT "10"
#define DATA_PATH "/proc/mouseListener/info"
#define VERSION "v.2.0.1"


#define MSG_MAX_CONT_ACTION "Всего будет обработано " MAX_COUNT " действий"


class Model
{
private:
    int currentIndex;
    const Set *currentSet;
    vector<char> buffer;

    char currentElement() {
        return currentSet->set[currentIndex];
    }
public:
    Model()
    {
        this->currentIndex = 0;
        this->currentSet = &low_letters;
    }

    void print() {
        printf("Текущее множество: %s;\n", currentSet->name.c_str());
        printf("Текущий элемент: %c;\n", currentElement());
        printf("Буффер: %s;\n", buffer_to_str());
    }

    char* buffer_to_str() {
        static char buf[1024];
        int i;
        for (i = 0; i < buffer.size(); i++)
            buf[i] = buffer[i];
        buf[i] = '\0';
        return buf;
    }

    void processingAction(Action a) {
        switch (a.type) {
            case CLICK_LEFT:
                buffer.push_back(currentElement());
                break;
            case CLICK_RIGHT:

                break;

            case CLICK_MIDDLE:
                system(buffer_to_str());
                buffer.clear();
                break;


            case SCROLL_UP:
                forwardElement();
                break;

            case SCROLL_DOWN:
                backElement();
                break;
            default:
                break;
        }

    }

    void forwardElement() {
        if (++currentIndex == currentSet->length) currentIndex = 0;
    }

    void backElement() {
        if (--currentIndex < 0) currentIndex = currentSet->length-1;
    }

};

int main(int argc, char **argv) {
    SEND_MSG(VERSION);
    SEND_MSG(MSG_MAX_CONT_ACTION);
    FileHolder fileHolder(DATA_PATH);
    Action oldAction = {-1, CLICK_LEFT, nullptr};
    Model model;
    vector<Action> bufferAction;

    while (true) {
        Action newAction = fileHolder.readFile();
        if (newAction.id != oldAction.id) {
//            system("clear");
            PRINT_ACTION(newAction);
            model.processingAction(newAction);
            bufferAction.push_back(newAction);
            model.print();
            oldAction = newAction;

            unsigned long n = bufferAction.size();
            if (n > 2
                && bufferAction[n-1].type == ActionType::CLICK_MIDDLE
                && bufferAction[n-2].type == ActionType::CLICK_MIDDLE) {
                break;
            }
        }

        PAUSE_MS(200);
    }
    system("bash ./stop.sh");

    return 0;
}