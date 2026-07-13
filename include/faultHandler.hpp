#ifndef FAULTHANDLER_HPP
#define FAULTHANDLER_HPP


#include <array>
#include <cstdint>
#include "helpers.hpp"

class FaultBus; // Forward declaration of FaultBus class

class FaultHandler
{
    

    friend class FaultDevice; // Declare FaultDevice as a friend class to allow access to private members

    public:
        using CallbackFunction = void(*)(void* context); // Define a type alias for the callback function pointer
        
        FaultHandler() = default;
        ~FaultHandler() = default;

        FaultHandler(const FaultHandler&) = delete;
        FaultHandler& operator=(const FaultHandler&) = delete;
        void setCallback(FaultType faultType, CallbackFunction callback);
        const std::array<CallbackFunction, size_t(FaultType::SIZE)>& getCallbacks() const;
        void setFaultBus(FaultBus* bus);

        
    private:
        FaultBus* faultBus = nullptr; // Pointer to the FaultBus for fault management
        std::array<CallbackFunction, size_t(FaultType::SIZE)> faultHandlers{}; // Array of fault handlers for each fault type
};









#endif // FAULTHANDLER_HPP