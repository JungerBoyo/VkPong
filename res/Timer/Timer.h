#ifndef TIMER_H
#define TIMER_H

#include <chrono>

using f64 = double;

struct Timer
{
    Timer(f64* timeStorageVar);
    ~Timer();

    const std::chrono::time_point<std::chrono::high_resolution_clock> beginTimer;

    private: f64* _duration;
};

#endif