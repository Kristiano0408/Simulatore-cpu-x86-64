#ifndef CACHECONTROLLER_HPP
#define CACHECONTROLLER_HPP


#include <cstdint>
#include <cstring>
#include "eventHandler.hpp"
#include "device.hpp"

class CacheController : public FaultDevice
{
    private:
        CacheEventHandler& cacheEventHandler; // Event handler for managing cache events and callbacks
        uint32_t numSets; // Number of cache sets in the cache level

    public:
        CacheController(CacheEventHandler& eventHandler, uint32_t nSets) : cacheEventHandler(eventHandler), numSets(nSets) { DEBUG_LOG(debugLog("Cache Controller created")); } // Constructor to initialize the cache controller with an event handler and number of cache sets
        
        AddressInfo decodeAddress(uint64_t address); // Function to calculate the set index, tag and offset from a memory address

        LookUpResult lookupCache(const AddressInfo& addressInfo, TypeofData dataType); // Function to perform cache lookup based on address information and type of data being accessed
        void handleRequest(PendingRequest& request); // Function to handle incoming cache requests and coordinate the cache operations
        static void CallBackWrapperScheduler(void* context, PendingRequest& request); // Wrapper function to trigger a cache lookup event for logging or debugging purposes
    
};



#endif //CACHECONTROLLER_HPP