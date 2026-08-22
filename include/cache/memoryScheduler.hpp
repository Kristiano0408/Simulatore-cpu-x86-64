#ifndef MEMORYSCHEDULER_HPP
#define MEMORYSCHEDULER_HPP


#include <cstdint>
#include <vector>
#include "device.hpp"

class Bus;

class MemoryScheduler : public FaultDevice
{
    private:
        Bus& bus; // Reference to the bus for memory access
        uint16_t memoryLatency; // Latency of memory access in cycles
        std::vector<PendingRequest> memoryRequestQueue; // Queue to hold pending memory requests
    public:
        MemoryScheduler(Bus& b, uint16_t latency) : bus(b), memoryLatency(latency) { DEBUG_LOG(debugLog("Memory Scheduler created")); } // Constructor to initialize the memory scheduler with a reference to the bus and memory latency
        //void tick(); // Function to be called every clock tick to process memory requests
        const std::vector<PendingRequest>& getPendingRequests() const { return memoryRequestQueue; } // Function to get the pending memory requests being processed by the memory scheduler
        void scheduleMemoryRequest(CacheRequest&& request); // Function to schedule a memory request
        void schedulePendingRequest(PendingRequest&& pendingRequest); // Function to schedule a pending memory request
        void processMemoryRequests(); // Function to process scheduled memory requests
        void processMemoryRequest(PendingRequest&& pendingRequest); // Function to process a single memory request based on its type and data
        void setMemoryLatency(uint16_t latency) { memoryLatency = latency; }
        uint16_t getMemoryLatency() const { return memoryLatency; }
};



#endif //MEMORYSCHEDULER_HPP
