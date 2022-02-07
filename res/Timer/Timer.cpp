#include "Timer.h"

Timer::Timer(f64* timeStorageVar) 
     : _duration(timeStorageVar), beginTimer(std::chrono::high_resolution_clock::now()) { }

Timer::~Timer() 
{
    const auto endTimer{std::chrono::high_resolution_clock::now()};

    const auto beginMs{std::chrono::time_point_cast<std::chrono::microseconds>(beginTimer).time_since_epoch().count()};
    const auto endMs{std::chrono::time_point_cast<std::chrono::microseconds>(endTimer).time_since_epoch().count()};

    *_duration = static_cast<f64>(endMs - beginMs)/1000.;

    _duration = nullptr;
}
