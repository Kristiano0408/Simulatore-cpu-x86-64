#ifndef REQUESTSCHEDULER_HPP
#define REQUESTSCHEDULER_HPP


#include <cstdint>
#include <vector>
#include "device.hpp"

class RequestScheduler : public FaultDevice
{
    using CallbackType = void(*)(void*, PendingRequest&);
    private:
        uint8_t latency; // Latency of the cache level in ticks
        uint8_t fillLatency;
        std::vector<PendingRequest> pendingRequests; // Vector to hold pending requests being processed by the cache scheduler

        CallbackType cacheControllerCallback; // Callback function to be called when a cache request is ready to be processed by the cache controller
        void* CallbackContext; // Context pointer to be passed to the callback function for additional information or state management
        
        public:
        RequestScheduler(uint8_t levelLatency, uint8_t fillL, CallbackType callback, void* context) : latency(levelLatency), fillLatency(fillL), cacheControllerCallback(callback), CallbackContext(context) {} // Constructor to initialize the request scheduler with a callback function for processing cache requests
        void processRequests(); // Function to be called every clock tick to process pending requests
        void scheduleRequest(CacheRequest&& request); // Function to schedule a cache request
        void schedulePendingRequest(PendingRequest&& pendingRequest); // Function to schedule a pending request
        const std::vector<PendingRequest>& getPendingRequests() const { return pendingRequests; } // Function to get the pending requests being processed by the cache scheduler
};










#endif //REQUESTSCHEDULER_HPP