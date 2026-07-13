#ifndef FAULTBUS_HPP
#define FAULTBUS_HPP

#include <cstdint>
#include <array>
#include <vector>
#include <bitset>
#include "device.hpp"
#include "helpers.hpp"


struct FaultCallback
{
    uint16_t deviceId; // Unique identifier for the device that generated the fault
    void(*callback)(void* context); // Callback function to handle the fault
};





class FaultBus
{
    
    public:
        FaultBus();
        ~FaultBus() = default;

        FaultBus(const FaultBus&) = delete;
        FaultBus& operator=(const FaultBus&) = delete;

        void subscribeToFaults(FaultDevice* device, const std::array<void(*)(void*), size_t(FaultType::SIZE)>& handlers = {}); // Subscribe a device to the fault bus with optional fault handlers

        void sendFault(FaultType faultType); // Send a fault event to the busQueue

        void processFaults(); // Process the queued fault events and invoke the corresponding handlers

        bool isDeviceSubscribed(uint16_t deviceId) const;

    private:
        std::array<FaultDevice*, MAX_DEVICES> subscribers; // array to hold subscribers and their contexts
        FixedSizeQueueCacheFriendly<FaultType, 100> faultQueue; // Queue to hold fault events with a fixed size of 100
        std::array<std::vector<FaultCallback>, size_t(FaultType::SIZE)> faultCallBacks; // Vector of vectors to hold callback for every fault type, indexed by FaultType enum

   
};

















#endif // FAULTBUS_HPP