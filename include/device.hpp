#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <cstdint>
#include <atomic>
#include <array>
#include "helpers.hpp"
#include "faultHandler.hpp"


class TickedDevice {

protected:
    // Protected members can be accessed by derived classes
    int ticks_needed = 1; // Number of ticks needed for an operation(default is 1)
    int ticks_progress = 0; // Current progress in ticks (like the clock one but for this device and this operation)

public:
   
    virtual ~TickedDevice() = default;

    virtual void tick(); // Function to be called every clock tick

    virtual void execute_operation() = 0; // Pure virtual function to be implemented by derived classes

    virtual int getTicksNeeded() const; // Get the number of ticks needed for the current operation

    int getTicksProgress() const; // Get the current progress in ticks

    void setTicksNeeded(int ticks); // Set the number of ticks needed for the current operation

    void resetTicksProgress(); // Reset the ticks progress to zero
    


    
};


class FaultDevice
{
    public:
        FaultDevice();
        uint16_t getDeviceId() const;
        void setFaultBus(FaultBus* bus); // Set the FaultBus pointer (called by the FaultBus class)

    protected:
        FaultHandler faultHandler; // Each device has its own FaultHandler to manage its fault callbacks

    private:
        uint16_t deviceId; // Unique identifier for the device
        static std::atomic<uint16_t> globalId; // Global fault counter for the device(for creating unique fault IDs)
        
        
        
    
        
};

#endif // DEVICE_HPP