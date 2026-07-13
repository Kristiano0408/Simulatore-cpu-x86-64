#include "faultBus.hpp"


FaultBus::FaultBus() 
{
    subscribers.fill(nullptr); // Initialize all subscriber slots to nullptr
    for (auto& handlers : faultCallBacks) 
    {
        handlers.reserve(10); // Reserve space for the number of subscribers to avoid frequent reallocations
    }
}
void FaultBus::subscribeToFaults(FaultDevice* device, const std::array<void(*)(void*), size_t(FaultType::SIZE)>& handlers)
{
    subscribers[device->getDeviceId()] = device; // Store the device pointer in the subscribers array using its unique device ID
    device->setFaultBus(this); // Set the FaultBus pointer in the device

    for (uint8_t i = 0; i < uint8_t(FaultType::SIZE); ++i) 
    {
        if (handlers[i] != nullptr) 
        {
            FaultCallback callback{device->getDeviceId(), handlers[i]}; // Create a FaultCallback for the device and the corresponding handler
            faultCallBacks[i].push_back(callback); // Store the handlers for each fault type
        }
        
    }
}


void FaultBus::sendFault(FaultType faultType)
{
    if (faultType == FaultType::NONE) 
    {
        return; // No fault to send
    }

    faultQueue.push(faultType); // Push the fault type to the queue
}


void FaultBus::processFaults()
{
    while (faultQueue.size() > 0) 
    {
        FaultType faultType = faultQueue.pop(); // Pop the fault type from the queue
        uint8_t faultIndex = static_cast<uint8_t>(faultType); // Convert FaultType to its underlying integer value

        if (faultIndex >= static_cast<uint8_t>(FaultType::SIZE)) 
        {
            continue; // Invalid fault type, skip processing
        }

        for (const auto& handler : faultCallBacks[faultIndex]) 
        {
            FaultDevice* device = subscribers[handler.deviceId]; // Get the device pointer from the subscribers array using the device ID
            if(device != nullptr)
            {
                handler.callback(subscribers[handler.deviceId]); // Invoke the callback for the subscribed device WITH the device context
            }
        }
    }
}

