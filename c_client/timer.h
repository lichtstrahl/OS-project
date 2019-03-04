//
// Created by igor on 04.03.19.
//

#ifndef C_CLIENT_TIMER_H
#define C_CLIENT_TIMER_H

#include <chrono>
#include <thread>

#define PAUSE_MS(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))

#endif //C_CLIENT_TIMER_H
