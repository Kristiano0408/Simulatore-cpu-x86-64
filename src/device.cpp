#include "device.hpp"

void Device::tick() {
    ticks_progress++;
    if (ticks_progress >= ticks_needed) {
        execute_operation();
        ticks_progress = 0; // Reset progress after operation is executed
    }
}

int Device::getTicksNeeded() const {
    return ticks_needed;
}

int Device::getTicksProgress() const {
    return ticks_progress;
}

void Device::setTicksNeeded(int ticks) {
    ticks_needed = ticks;
}

void Device::resetTicksProgress() {
    ticks_progress = 0;
}

// Note: The execute_operation method is pure virtual and must be implemented by derived classes.

