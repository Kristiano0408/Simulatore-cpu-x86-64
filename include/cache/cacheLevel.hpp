#ifndef CACHELEVEL_HPP
#define CACHELEVEL_HPP

#include "cacheController.hpp"
#include "cacheStorage.hpp"
#include "device.hpp"
#include "replacementPolicy.hpp"
#include "requestScheduler.hpp"
#include "writePolicy.hpp"
#include <cstdint>
#include <memory>

class Bus;

/// Cache level structure(L1, L2, L3)
/// Contains multiple cache sets and manages the cache operations
class CacheLevel : public TickedDevice, public FaultDevice
{
private:
    uint64_t cacheSize;

    uint32_t numSets;

    Bus& bus;

    CacheLevel* nextLevel;
    CacheLevel* parentLevel1;
    CacheLevel* parentLevel2;

    std::array<CacheLevel*, 2> L1Ipath;
    std::array<CacheLevel*, 2> L1Dpath;

    uint8_t associativity;
    uint8_t latencyCycles;
    uint8_t latencyFill;
    CacheLevelType type = CacheLevelType::NONE;

    CacheEventHandler eventHandler;

    CacheStorage storage;
    CacheController controller;
    RequestScheduler scheduler;

    std::unique_ptr<ReplacementPolicy> replacementPolicy; // Unique pointer to the replacement policy used by the cache level
    std::unique_ptr<WritePolicy> writePolicy;             // Unique pointer to the write policy used by the cache level

    static void onHitWrapper(void* context, CacheEventPayload& payload);           // Wrapper function to trigger a cache hit event for logging or debugging purposes
    static void onMissWrapper(void* context, CacheEventPayload& payload);          // Wrapper function to trigger a cache miss event for logging or debugging purposes
    static void onHitCrossLinesWrapper(void* context, CacheEventPayload& payload); // Wrapper function to trigger a cache hit cross
                                                                                   // lines event for logging or debugging purposes
    static void onFillWrapper(void* context, CacheEventPayload& payload);
    static LookUpResult lookupWrapper(void* context, CacheLookupPayload& payload); // Wrapper function to trigger a cache lookup event
                                                                                   // for logging or debugging purposes

    // on address information and cache request
    void readSingleLine(const AddressInfo& addressInfo, CacheRequest& request);                                   // Function to read data from a single cache line based on address
                                                                                                                  // information and cache request
    void readCrossLines(const AddressInfo& addressInfo1, const AddressInfo& addressInfo2, CacheRequest& request); // Function to read data that spans across two cache lines based
                                                                                                                  // on address information and cache request
    // void write(const AddressInfo& addressInfo, CacheRequest& request); // Function to write data to the cache based
    // on address information and cache request
    void writeSingleLine(const AddressInfo& addressInfo, CacheRequest& request);                                   // Function to write data to a single cache line based on address
                                                                                                                   // information and cache request
    void writeCrossLines(const AddressInfo& addressInfo1, const AddressInfo& addressInfo2, CacheRequest& request); // Function to write data that spans across two cache lines based
                                                                                                                   // on address information and cache request
    void onEviction(const AddressInfo& addressInfo, const CacheLine& evictedLine);
    void propagateWriteToNextLevel(CacheRequest& request, bool propagateCallback); // Function to propagate a write operation to the next cache
                                                                                   // level or memory based on the write policy
    // void fillLine(const AddressInfo& addressInfo, CacheRequest& request); // Function to fill a cache line with data
    // from the next level or memory based on the cache request and address information

public:
    CacheLevel(uint64_t size, uint8_t associativityValue, uint8_t latency, uint8_t fillLatency, Bus& busRef, CacheLevel* nextLevelPtr = nullptr, CacheLevel* parentLevel1Ptr = nullptr,
               CacheLevel* parentLevel2Ptr = nullptr, CacheLevelType cacheType = CacheLevelType::NONE, std::array<CacheLevel*, 2> L1Icourse = {nullptr, nullptr},
               std::array<CacheLevel*, 2> L1Dcourse = {nullptr, nullptr}); // Constructor to initialize the cache level with size, associativity, latency, fill latency, bus
                                                                           // reference, next level reference, parent level references, cache level type and L1 courses
    ~CacheLevel() = default;

    void execute_operation() override; // Override of the pure virtual function from Device class

    const std::vector<PendingRequest>& getPendingRequests() const
    {
        return scheduler.getPendingRequests();
    } // Function to get the pending requests being processed by the cache scheduler
    LookUpResult lookupCache(const AddressInfo& addressInfo, TypeofData dataType); // Function to perform cache lookup based on address information and
                                                                                   // type of data being accessed, returns the result of the lookup
                                                                                   // (hit, miss, hit cross lines, miss cross lines)
    void onHit(const AddressInfo& addressInfo, CacheRequest& request);             // Function to handle cache hit events based on address information and cache request
    void onMiss(CacheRequest& request);                                            // Function to handle cache miss events based on address information and cache request
    void onHitCrossLines(const AddressInfo& addressInfo1, const AddressInfo& addressInfo2, CacheRequest& request); // Function to handle cache hit events that span across two cache
                                                                                                                   // lines based on address information and cache request
    void onFill(const AddressInfo& addressInfo, [[maybe_unused]] CacheRequest& request, CacheLine& line); // Function to handle filling a cache line with data from the next level
                                                                                                          // or memory based on the cache request and address information
    // void onMissCrossLines(const AddressInfo& addressInfo, CacheRequest& request); // Function to handle cache miss
    // events that span across two cache lines based on address information and cache request
    void scheduleRequest(CacheRequest&& request);                 // Function to schedule a cache request for processing by the cache scheduler
    void schedulePendingRequest(PendingRequest&& pendingRequest); // Function to schedule a pending request for
                                                                  // processing by the cache scheduler

    void fillToL1I(CacheLine* line, const AddressInfo& addressInfo); // Function to fill a cache line in parents with L1I data
    void fillToL1D(CacheLine* line, const AddressInfo& addressInfo); // Function to fill a cache line in parents with L1D data
    void fillToL1Idouble(CacheLine* line1, CacheLine* line2, const AddressInfo& addressInfo1,
                         const AddressInfo& addressInfo2); // Function to fill a cache line in parents with L1I data for cross lines
    void fillToL1Ddouble(CacheLine* line1, CacheLine* line2, const AddressInfo& addressInfo1,
                         const AddressInfo& addressInfo2); // Function to fill a cache line in parents with L1D data for cross lines
    // getters and setters for the cache size and associativity
    uint64_t getCacheSize() const { return cacheSize; }
    uint8_t getAssociativity() const { return associativity; }
    uint32_t getNumSets() const { return numSets; }

    void setLatency(uint8_t latency) { latencyCycles = latency; }
    uint8_t getLatency() const { return latencyCycles; }

    void setFillLatency(uint8_t fillLatency) { latencyFill = fillLatency; }
    uint8_t getFillLatency() const { return latencyFill; }

    void setCacheSize(uint32_t size) { cacheSize = size; }
    void setAssociativity(uint8_t assoc) { associativity = assoc; }
    void setNumSets(uint32_t sets) { numSets = sets; };

    void setParentLevel1(CacheLevel* parent) { parentLevel1 = parent; }
    CacheLevel* getParentLevel1() const { return parentLevel1; }

    void setParentLevel2(CacheLevel* parent) { parentLevel2 = parent; }
    CacheLevel* getParentLevel2() const { return parentLevel2; }

    void setNextLevel(CacheLevel* next) { nextLevel = next; }
    CacheLevel* getNextLevel() const { return nextLevel; }

    void setL1Ipath(std::array<CacheLevel*, 2>&& path) { L1Ipath = path; }
    const std::array<CacheLevel*, 2>& getL1Ipath() const { return L1Ipath; }

    void setL1Dpath(std::array<CacheLevel*, 2>&& path) { L1Dpath = path; }
    const std::array<CacheLevel*, 2>& getL1Dpath() const { return L1Dpath; }

    CacheStorage& getStorage() { return storage; }
};

#endif // CACHELEVEL_HPP