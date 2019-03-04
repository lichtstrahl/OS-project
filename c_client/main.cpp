#include <iostream>
#include <unistd.h>
#include "timer.h"
#include "FileHolder.h"
#include <fstream>
#include "const.h"

using std::ifstream;


#define MAX_COUNT "10"
#define DATA_PATH "/proc/mouseListener/info"
#define VERSION "v.2.0.1"


#define MSG_MAX_CONT_ACTION "Всего будет обработано " MAX_COUNT " действий"


class Model
{
private:
    int currentElement;
    const Set *currentSet;
public:
    Model()
    {
        this->currentElement = 0;
        this->currentSet = &low_letters;
    }

    void print() {
        printf("Текущее множество: %s;\n", currentSet->name.c_str());
        printf("Текущий индекс: %d;\n", currentElement);
        printf("Текущий элемент: %d;\n", currentSet->set[currentElement]);
    }

    bool processingAction(Action a) {
        switch (a.type) {
            case CLICK_LEFT:
                break;
            case CLICK_RIGHT:
                break;

            case CLICK_MIDDLE:
                return true;


            case SCROLL_UP:
                backElement();
                break;

            case SCROLL_DOWN:
                forwardElement();
                break;
        }
        return false;
    }

    void forwardElement() {
        if (++currentElement == currentSet->length) currentElement = 0;
    }

    void backElement() {
        if (--currentElement < 0) currentElement = currentSet->length-1;
    }

};

int main(int argc, char **argv) {
    SEND_MSG(VERSION);
    SEND_MSG(MSG_MAX_CONT_ACTION);
    FileHolder fileHolder(DATA_PATH);
    Action oldAction = {-1, CLICK_LEFT, nullptr};
    Model model;

    while (true) {
        Action newAction = fileHolder.readFile();
        if (newAction.id != oldAction.id) {
            PRINT_ACTION(newAction);
            bool exit = model.processingAction(newAction);
            model.print();
            oldAction = newAction;

            if (exit) {
                break;
            }
        }

        PAUSE_MS(200);
    }
    system("bash ./stop.sh");

    return 0;
}