#include "clock.hpp"

void Clock::tick() {
    cycles++;
}

uint64_t Clock::getCycles() const {
    return cycles;
}
