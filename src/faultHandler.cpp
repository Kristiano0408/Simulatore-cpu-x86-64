#include "faultHandler.hpp"
#include "faultBus.hpp"


void FaultHandler::setCallback(FaultType faultType, FaultHandler::CallbackFunction callback)
{
    faultHandlers[static_cast<size_t>(faultType)] = callback; // Set the callback function for the specified fault type
}


const std::array<FaultHandler::CallbackFunction, size_t(FaultType::SIZE)>& FaultHandler::getCallbacks() const
{
    return faultHandlers; // Return the array of fault handlers
}


void FaultHandler::setFaultBus(FaultBus* bus)
{
    faultBus = bus; // Set the FaultBus pointer for this FaultHandler
}