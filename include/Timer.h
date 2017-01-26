#ifndef GEJM_TIMER_H
#define GEJM_TIMER_H


#include <chrono>



//Timer class declaration
class Timer
{
private:
  //private fields
    std::chrono::duration<double, std::milli> _duration;
    bool _in_progress;
    std::chrono::steady_clock::time_point _from;

public:
  //public methods
    Timer();
    double getTime(); //returns evaluated duration in seconds
    void start();
    void stop();
    void resetTime();
};


#endif //GEJM_TIMER_H