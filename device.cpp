#include "device.hpp"

void Device::tick() {
    ticks_progress++;
    if (ticks_progress >= ticks_needed) {
        execute_operation();
        ticks_progress = 0; // Reset progress after operation is executed
    }
}

