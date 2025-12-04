#include "device.hpp"
#include "helpers.hpp"

void Device::tick() {
    
    ticks_progress++;
    debugLog("Device ticked, progress: " + std::to_string(ticks_progress) + "/" + std::to_string(getTicksNeeded()));
    if (ticks_progress >= getTicksNeeded()) {
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

