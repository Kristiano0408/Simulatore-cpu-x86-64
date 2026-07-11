#ifndef CLOCK_HPP
#define CLOCK_HPP


#include <cstdint>
#include "eventLog.hpp"
class Clock {
private:
    uint64_t cycles = 0;
public:
    Clock() {EventLog::getInstance().bindTicks(&cycles);} // Bind the clock ticks to the EventLog instance
    void tick();
    uint64_t getCycles() const;
    inline void reset() { cycles = 0; }
};

#endif // CLOCK_HPP