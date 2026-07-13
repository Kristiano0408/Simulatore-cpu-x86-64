#include "device.hpp"
#include "helpers.hpp"

void TickedDevice::tick() {
    
    ticks_progress++;
    DEBUG_LOG(debugLog("Device ticked, progress: " + std::to_string(ticks_progress) + "/" + std::to_string(getTicksNeeded())));
    if (ticks_progress >= getTicksNeeded()) {
        execute_operation();
        ticks_progress = 0; // Reset progress after operation is executed
    }
}

int TickedDevice::getTicksNeeded() const {
    return ticks_needed;
}

int TickedDevice::getTicksProgress() const {
    return ticks_progress;
}

void TickedDevice::setTicksNeeded(int ticks) {
    ticks_needed = ticks;
}

void TickedDevice::resetTicksProgress() {
    ticks_progress = 0;
}


std::atomic<uint16_t> FaultDevice::globalId{0}; // Initialize the static atomic globalId to 0

FaultDevice::FaultDevice() 
{
    deviceId = globalId++; // Assign a unique device ID and increment the global counter
}

uint16_t FaultDevice::getDeviceId() const 
{
    return deviceId;
}

void FaultDevice::setFaultBus(FaultBus* bus) 
{
    faultHandler.faultBus = bus; // Set the FaultBus pointer in the FaultHandler
}

// Note: The execute_operation method is pure virtual and must be implemented by derived classes.

