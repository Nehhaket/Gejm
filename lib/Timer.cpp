#include "../include/Timer.h"


//Timer class implementation

//Constructor
Timer::Timer() {
    _in_progress = false;
    _duration.zero();
}

//Public methods
void Timer::start() {
    if(!_in_progress)
    {
        _from = std::chrono::steady_clock::now();
        _in_progress = true;
    }
}
void Timer::stop() {
    if(_in_progress)
    {
        std::chrono::steady_clock::time_point temp = std::chrono::steady_clock::now();
        _duration += std::chrono::duration_cast<std::chrono::milliseconds>(temp - _from);
        _in_progress = false;
    }
}
double Timer::getTime() {
    return _duration.count() / 1000;
}
void Timer::resetTime() {
    _duration.zero();
    _in_progress = false;
}
//