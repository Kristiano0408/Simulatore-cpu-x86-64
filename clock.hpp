#ifndef CLOCK_HPP
#define CLOCK_HPP


#include <cstdint>

class Clock {
private:
    uint64_t cycles = 0;
public:
    void tick();
    uint64_t getCycles() const;
};

#endif // CLOCK_HPP