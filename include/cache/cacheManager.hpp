#ifndef CACHEMANAGER_HPP
#define CACHEMANAGER_HPP
#include "../device.hpp"
#include "cacheLevel.hpp"
#include "memory.hpp"
#include "memoryScheduler.hpp"
#include <cstdint>
#include <cstring>
#include <vector>

class Bus;

/// Cache manager class to manage multiple cache levels
class CacheManager : public TickedDevice, public FaultDevice
{
public:
    CacheManager(Bus& busReference, uint64_t l1Size, uint64_t l2Size, uint64_t l3Size, uint8_t l1Assoc, uint8_t l2Assoc, uint8_t l3Assoc, uint8_t l1Latency = 1, uint8_t l2Latency = 2,
                 uint8_t l3Latency = 3, uint8_t l1FillLatency = 1, uint8_t l2FillLatency = 1, uint8_t l3FillLatency = 1);
    ~CacheManager();

    CacheManager(const CacheManager&) = delete;
    CacheManager& operator=(const CacheManager&) = delete;

    void execute_operation() override; // Override of the pure virtual function from Device class

    void enqueRequest(CacheRequest&& request, CacheLevelType cacheType);
    void flushAllCaches();
    void invalidateAllCaches();
    void printCacheState() const; // For debugging purposes

    CacheLevel& getL1ICache() { return L1ICache; }
    CacheLevel& getL1DCache() { return L1DCache; }
    CacheLevel& getL2Cache() { return L2Cache; }
    CacheLevel& getL3Cache() { return L3Cache; }
    const std::vector<PendingRequest>& getL1IRequestQueue() const { return L1ICache.getPendingRequests(); }
    const std::vector<PendingRequest>& getL1DRequestQueue() const { return L1DCache.getPendingRequests(); }
    const std::vector<PendingRequest>& getL2RequestQueue() const { return L2Cache.getPendingRequests(); }
    const std::vector<PendingRequest>& getL3RequestQueue() const { return L3Cache.getPendingRequests(); }
    const std::vector<PendingRequest>& getMemoryRequestQueue() const { return memoryScheduler.getPendingRequests(); }

    MemoryScheduler& getMemoryScheduler() { return memoryScheduler; }

    uint8_t memoryLatency = 1;

protected:
    CacheLevel L3Cache;
    CacheLevel L2Cache;
    CacheLevel L1ICache;
    CacheLevel L1DCache;
    MemoryScheduler memoryScheduler; // Memory scheduler for managing memory requests

private:
    Bus& bus; // Reference to the bus
    // Queue to hold cache requests
    std::vector<CacheRequest> requestQueueL1I;
    std::vector<CacheRequest> requestQueueL1D;
};

#endif // CACHEMANAGER_HPP