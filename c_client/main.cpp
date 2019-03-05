#include <iostream>
#include <unistd.h>
#include "timer.h"
#include "FileHolder.h"
#include <fstream>
#include "const.h"
#include "color.h"
#include <vector>
#include <string>
#include <string.h>

using std::ifstream;
using std::vector;
using std::string;

#define MAX_COUNT "10"
#define DATA_PATH "/proc/mouseListener/info"
#define VERSION "v.2.2.0 Добавлены специальные символы [Space] [Backspace]"


#define MSG_MAX_CONT_ACTION "Всего будет обработано " MAX_COUNT " действий"

class Model
{
private:
    int currentIndex;
    int currentSet;
    vector<string> buffer;

    string getCurrentElement() {
        return availableSet[currentSet].set[currentIndex];
    }
    Set getCurrentSet() {
        return availableSet[currentSet];
    }

public:
    Model()
    {
        this->currentIndex = 0;
        this->currentSet = 0;
    }

    void print() {
        for (int i = 0; i < COUNT_AVAILABLE_SET; i++) {
            char c1 = (i==currentSet) ? '[' : ' ';
            char c2 = (i == currentSet) ? ']' : ' ';
            const char* color = (i == currentSet) ? COLOR_CYAN : COLOR_DEFAULT;
            printf("%c%s%s%s%c", c1, color, availableSet[i].name.c_str(), COLOR_DEFAULT, c2);
        }
        printf("\n");


        Set set = getCurrentSet();
        for (int i = 0; i < set.length; i++) {
            char c1 = (i== currentIndex) ? '[' : ' ';
            char c2 = (i == currentIndex) ? ']' : ' ';
            const char *color = (i == currentIndex) ? COLOR_GREEN : COLOR_DEFAULT;
            printf("%c%s%s%s%c", c1, color, set.set[i].c_str(), COLOR_DEFAULT, c2);
        }
        printf("\n");


        printf("Буффер: %s;\n", buffer.size() != 0 ? buffer_to_str() : MSG_NEXT_CLICK_EXIT);
    }

    char* buffer_to_str() {
        static char buf[1024];
        int i;
        for (i = 0; i < buffer.size(); i++)
            buf[i] = buffer[i].c_str()[0];
        buf[i] = '\0';
        return buf;
    }

    bool processingAction(Action a) {
        switch (a.type) {
            case CLICK_LEFT:
                processingClickLeft();
                break;
            case CLICK_RIGHT:{
                unsigned long n = buffer.size();
                system(buffer_to_str());
                return n == 0;
            }

            case SIDE_1:
                forwardSet();
                break;

            case SIDE_2:
                backSet();
                break;

            case CLICK_MIDDLE:
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

        return false;

    }

    void processingClickLeft() {

        switch (getCurrentSet().type) {
            case SetType::COMMON:
                buffer.push_back(getCurrentElement());
                break;

            case SetType::SPECIFIC: {
                string action = getCurrentElement();
                if (!strcmp(action.c_str(), SPECIAL_SPACE) && buffer.size() > 0) {   // Обработка пробела
                    buffer.push_back(" ");
                }

                if (!strcmp(action.c_str(), SPECIAL_BACKSPACE) && buffer.size() > 0) {
                    buffer.pop_back();
                }


                break;
            }

        }
    }

    void forwardSet() {
        if (++currentSet == COUNT_AVAILABLE_SET) {
            currentSet = 0;
        }
        currentIndex = 0;
    }

    void backSet() {
        if (--currentSet < 0) {
            currentSet = COUNT_AVAILABLE_SET-1;
        }
        currentIndex = 0;
    }

    void forwardElement() {
        if (++currentIndex == availableSet[currentSet].length) currentIndex = 0;
    }

    void backElement() {
        if (--currentIndex < 0) currentIndex = availableSet[currentSet].length-1;
    }

};

#define CLEAR system("clear")
#define RMMOD_USBHID    "sudo rmmod usbhid"
#define INSMOD_LISTENER "sudo insmod mouseListener/mouseListener.ko"
#define INSMOD_USBMOUSE "sudo insmod iv_usb_mouse_driver/iv_usb_mouse_driver.ko"
#define RMMOD_USBMOUSE  "sudo rmmod iv_usb_mouse_driver"
#define RMMOD_LISTENER  "sudo rmmod mouseListener"
#define PROBE_USBHID    "sudo modprobe usbhid"

int main(int argc, char **argv) {
    system(RMMOD_USBHID);
    system(INSMOD_LISTENER);
    system(INSMOD_USBMOUSE);
    CLEAR;
    FileHolder fileHolder(DATA_PATH);
    Action oldAction = {-1, CLICK_LEFT, nullptr};
    Model model;
    vector<Action> bufferAction;


    SEND_MSG(VERSION);
    model.print();

    while (true) {
        Action newAction = fileHolder.readFile();
        if (newAction.type != ActionType::NONE && newAction.id != oldAction.id) {
            CLEAR;
            SEND_MSG(VERSION);
            bool exit = model.processingAction(newAction);
            bufferAction.push_back(newAction);
            model.print();
            oldAction = newAction;

            if (exit) {
                break;
            }
        }

        PAUSE_MS(250);
    }


    system(RMMOD_USBMOUSE);
    system(RMMOD_LISTENER);
    system(PROBE_USBHID);
    return 0;
}
