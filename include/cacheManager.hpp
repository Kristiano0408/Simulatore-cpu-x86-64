#ifndef CACHEMANAGER_HPP
#define CACHEMANAGER_HPP
#include <cstdint>
#include <cstring>
#include <vector>
#include <array>
#include <iostream>
#include "helpers.hpp"
#include "memory.hpp"
#include <variant>
#include <queue>
#include <memory>
#include  <list>
#include <random>
#include "eventHandler.hpp"
#include "device.hpp"


class Bus;

enum class CacheLevelType
{
    L1,
    L2,
    L3,
    NONE //placeholder for basic class

};

//basic structure for the cache line
//it contains the data, the tag, the valid bit and the dirty bit
struct alignas(CACHE_LINE_SIZE) CacheLine
{
    LineData data;  // Data stored in the cache line

    uint64_t tag;
    uint64_t lastAccessTime; // use it as a counter for replacement policy, syncronized with clock when access

    bool valid;
    bool dirty; // Indicates if the line has been modified
   

    CacheLine() : data{}, tag{}, lastAccessTime{}, valid(false), dirty(false) {}


};


/// Cache set structure
/// Contains multiple cache lines and the set index
struct CacheSet
{
    std::vector<CacheLine> lines; // Lines in the cache set
    uint64_t setIndex;
};


struct PendingRequest
{   
    CacheLine line= CacheLine{}; //copy of the line 
    CacheRequest request;
    int remainingLatency; // Remaining latency in ticks
    RequestState state = RequestState::IDLE;
    

    PendingRequest(CacheRequest&& req, int latency)
        : request(std::move(req)), remainingLatency(latency) {}

    PendingRequest(CacheRequest&& req, int latency, CacheLine res)
        : line(res), request(std::move(req)), remainingLatency(latency)  {}
    PendingRequest(CacheRequest&& req, int latency, RequestState st)
        : request(std::move(req)), remainingLatency(latency), state(st) {}
    PendingRequest(CacheRequest&& req, CacheLine res)
        : line(res), request(std::move(req)) {}
    PendingRequest() = default;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct AddressInfo
{
    uint64_t address;
    uint64_t setIndex;
    uint64_t tag;
    uint64_t offset;

    AddressInfo(uint64_t addr, uint64_t setIdx, uint64_t tg, uint64_t off)
        : address(addr), setIndex(setIdx), tag(tg), offset(off) {}
};

struct CacheEventPayload
{
    AddressInfo addressInfo1;
    AddressInfo addressInfo2;
    CacheRequest& request;
    CacheLine* line; // Pointer to the cache line involved in the event, if applicable

    CacheEventPayload(const AddressInfo& addrInfo1, const AddressInfo& addrInfo2, CacheRequest& req) : addressInfo1(addrInfo1), addressInfo2(addrInfo2), request(req) {}
    CacheEventPayload(const AddressInfo& addrInfo, CacheRequest& req) : addressInfo1(addrInfo), addressInfo2(0, 0, 0, 0), request(req), line(nullptr) {}
    CacheEventPayload(const AddressInfo& addrInfo, CacheRequest& req, CacheLine* cacheLine) : addressInfo1(addrInfo), addressInfo2(0, 0, 0, 0), request(req), line(cacheLine) {}
};

struct CacheLookupPayload
{
    AddressInfo addressInfo;
    TypeofData dataType;

    CacheLookupPayload(const AddressInfo& addrInfo, TypeofData dt) : addressInfo(addrInfo), dataType(dt) {}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ReplacementPolicy
{
    public:
        virtual uint8_t selectLineToReplace(CacheSet& set) = 0; // Pure virtual function to select a line to replace based on the replacement policy
        virtual void updateOnAccess(CacheSet& set, uint8_t lineIndex) = 0; // Pure virtual function to update the replacement policy state on cache access
        virtual void onLineLoaded(CacheSet& set, uint8_t lineIndex) = 0; // Pure virtual function to update the replacement policy state when a line is loaded into the cache
        virtual void initializeSet(uint8_t setIndex, uint8_t associativity) = 0; // Pure virtual function to initialize the replacement policy state for a cache set
        virtual ~ReplacementPolicy() = default; // Virtual destructor for proper cleanup of derived classes
};

class NULLPolicy : public ReplacementPolicy
{
    public:

    void initializeSet([[maybe_unused]] uint8_t setIndex, [[maybe_unused]] uint8_t associativity) override {}; // Function to initialize the LRU state for a cache set
    uint8_t selectLineToReplace([[maybe_unused]] CacheSet& set) override {return 0;}; // Override of the function to select a line to replace based on LRU policy
    void updateOnAccess([[maybe_unused]] CacheSet& set, [[maybe_unused]] uint8_t lineIndex) override {}; // Override of the function to update the LRU state on cache access
    void onLineLoaded([[maybe_unused]] CacheSet& set,[[maybe_unused]] uint8_t lineIndex) override {}; // Override of the function to update the LRU state when a line is loaded into the cache
};

struct LRUState
{
   std::vector<uint8_t> lineIndices; // Vector to maintain the order of line indices based on recency of access (least recently accessed at the front)
   std::unordered_map<uint8_t, uint8_t> indexMap; // Map to quickly access the position of a line index in the vector for O(1) updates
   uint8_t validLines = 0; // Counter to keep track of the number of valid lines in the cache set, used for optimization to quickly find free lines without searching through the vector
};
//LRU replacement policy implementation
class LRUReplacementPolicy : public ReplacementPolicy
{
    private:
        std::unordered_map<uint8_t, LRUState> lruMap; // Map to hold the LRU state for each cache set (set index mapped to a vector of line indices ordered by recency of access)
    public:
        LRUReplacementPolicy(const uint8_t& numSets, const uint8_t& associativity); // Constructor to initialize the LRU state for all cache sets based on the number of sets and associativity
        void initializeSet(uint8_t setIndex, uint8_t associativity) override; // Function to initialize the LRU state for a cache set
        uint8_t selectLineToReplace(CacheSet& set) override; // Override of the function to select a line to replace based on LRU policy
        void updateOnAccess(CacheSet& set, uint8_t lineIndex) override; // Override of the function to update the LRU state on cache access
        void onLineLoaded(CacheSet& set, uint8_t lineIndex) override; // Override of the function to update the LRU state when a line is loaded into the cache
};


struct PLRUTree
{
    std::vector<bool> bits; // Vector to hold the bits of the PLRU tree, where each bit indicates the direction to take for replacement (0 for left, 1 for right)
    uint8_t validLines = 0; // Counter to keep track of the number of valid lines in the cache set, used for optimization to quickly find free lines without traversing the tree
    PLRUTree(uint8_t associativity) : bits(associativity - 1, false) {} // Constructor to initialize the PLRU tree based on the associativity of the cache set
    uint8_t selectLineToReplace(); // Function to select a line to replace based on the PLRU tree
    void updateLine(uint8_t lineIndex); // Function to update the PLRU tree on cache access to reflect the most recently used line

};
//PLRU replacement policy implementation    
class PLRUReplacementPolicy : public ReplacementPolicy
{
    private:
        std::unordered_map<uint8_t, PLRUTree> plruMap; // Map to hold the PLRU state for each cache set (set index mapped to a vector of bits representing the PLRU tree)
    public:
        PLRUReplacementPolicy(const uint8_t& numSets, const uint8_t& associativity); // Constructor to initialize the PLRU state for all cache sets based on the number of sets and associativity
        void initializeSet(uint8_t setIndex, uint8_t associativity) override; // Function to initialize the PLRU state for a cache set
        uint8_t selectLineToReplace(CacheSet& set) override; // Override of the function to select a line to replace based on PLRU policy
        void updateOnAccess(CacheSet& set, uint8_t lineIndex) override; // Override of the function to update the PLRU state on cache access
        void onLineLoaded(CacheSet& set, uint8_t lineIndex) override; // Override of the function to update the PLRU state when a line is loaded into the cache

};


//random replacement policy implementation
class RandomReplacementPolicy : public ReplacementPolicy
{
    private:
        std::random_device rd; // Random device to seed the random number generator
        std::mt19937 gen;
    public:
        RandomReplacementPolicy(){gen.seed(rd());}
        uint8_t selectLineToReplace(CacheSet& set) override; // Override of the function to select a line to replace based on random policy
        void updateOnAccess([[maybe_unused]] CacheSet& set, [[maybe_unused]] uint8_t lineIndex) override {} // No state to update on access for random replacement policy
        void onLineLoaded([[maybe_unused]] CacheSet& set, [[maybe_unused]] uint8_t lineIndex) override {} // No state to update when a line is loaded for random replacement policy
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WritePolicy
{
    public:
        virtual bool writeThroughOnHit() const = 0; // Pure virtual function to determine if the write policy is write-through
        virtual bool writeAllocateOnMiss() const = 0; // Pure virtual function to determine if the write policy is write-allocate
        virtual ~WritePolicy() = default; // Virtual destructor for proper cleanup of derived classes
};

class WriteBackNoAllocate: public WritePolicy
{
    public:
        bool writeThroughOnHit() const override { return false; } // Override to indicate that this policy is write-back on hit
        bool writeAllocateOnMiss() const override { return false; } // Override to indicate that this policy is write-allocate on miss
};

class WriteThroughNoAllocate : public WritePolicy
{
    public:
        bool writeThroughOnHit() const override { return true; } // Override to indicate that this policy is write-through on hit
        bool writeAllocateOnMiss() const override { return false; } // Override to indicate that this policy is write-allocate on miss
};

class WriteThroughAllocate : public WritePolicy
{
    public:
        bool writeThroughOnHit() const override { return true; } // Override to indicate that this policy is write-through on hit
        bool writeAllocateOnMiss() const override { return true; } // Override to indicate that this policy is write-allocate on miss
};

class WriteBackAllocate : public WritePolicy
{
    public:
        bool writeThroughOnHit() const override { return false; } // Override to indicate that this policy is write-back on hit
        bool writeAllocateOnMiss() const override { return true; } // Override to indicate that this policy is write-allocate on miss
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class CacheStorage
{
    private:
        std::vector<CacheSet> sets; // Cache sets

    public:
        CacheStorage(uint8_t numSets, uint8_t associativity); // Constructor to initialize the cache storage with the specified number of sets and associativity
        CacheSet& getSet(uint8_t setIndex) { return sets[setIndex]; } // Function to get a reference to a cache set based on the set index
        std::vector<CacheSet>& getSets() { return sets; }
        CacheLine* findLine(uint8_t setIndex, uint64_t tag); // Function to find a cache line based on set index and tag
        int8_t findLineIndex(uint8_t setIndex, uint64_t tag); // Function to find the index of a cache line based on set index and tag
        void invalidateLine(uint8_t setIndex, uint8_t lineIndex); // Function to invalidate a specific cache line
        void invalidateAllLines(); // Function to invalidate all cache lines in the cache storage
        void invalidateLineByAddress(AddressInfo addressInfo); // Function to invalidate a specific cache line based on address information
        void flush(auto&& memoryWriteFunction); // Function to flush the cache by writing back all dirty lines to memory using the provided memory write function
        void loadLine(uint8_t setIndex, const CacheLine& line, uint8_t lineIndex); // Function to load a cache line into the cache storage at the specified set index and line index
};

class RequestScheduler
{
    private:
        uint8_t latency; // Latency of the cache level in ticks
        uint8_t fillLatency;
        std::vector<PendingRequest> pendingRequests; // Vector to hold pending requests being processed by the cache scheduler
        std::function<void(PendingRequest&)> cacheControllerCallback; // Callback function to be called when a cache request is ready to be processed by the cache controller
    public:
        RequestScheduler(uint8_t latency, uint8_t latencyFill, std::function<void(PendingRequest&)> cacheControllerCallback) : latency(latency), fillLatency(latencyFill), cacheControllerCallback(std::move(cacheControllerCallback)) {} // Constructor to initialize the request scheduler with a callback function for processing cache requests
        void processRequests(); // Function to be called every clock tick to process pending requests
        void scheduleRequest(CacheRequest&& request); // Function to schedule a cache request
        void schedulePendingRequest(PendingRequest&& pendingRequest); // Function to schedule a pending request
};

class CacheController
{
    private:
        EventHandler<EventHandlerCacheEventType>& cacheEventHandler; // Event handler for managing cache events and callbacks
        uint32_t numSets; // Number of cache sets in the cache level
    public:
        CacheController(EventHandler<EventHandlerCacheEventType>& eventHandler, uint32_t numSets) : cacheEventHandler(eventHandler), numSets(numSets) {} // Constructor to initialize the cache controller with an event handler and number of cache sets
        
        AddressInfo decodeAddress(uint64_t address); // Function to calculate the set index, tag and offset from a memory address

        LookUpResult lookupCache(const AddressInfo& addressInfo, TypeofData dataType); // Function to perform cache lookup based on address information and type of data being accessed
        void handleRequest(PendingRequest& request); // Function to handle incoming cache requests and coordinate the cache operations
    
};

/// Cache level structure(L1, L2, L3)
/// Contains multiple cache sets and manages the cache operations
class CacheLevel: public Device
{
    private:
        uint32_t cacheSize;
        uint8_t associativity;
        uint32_t numSets;

        Bus& bus;

        CacheLevel* nextLevel;
        CacheLevel* parentLevel;

        uint8_t latencyCycles;
        uint8_t latencyFill;
        CacheLevelType type = CacheLevelType::NONE;

        CacheStorage storage;
        CacheController controller;
        RequestScheduler scheduler;

        std::unique_ptr<ReplacementPolicy> replacementPolicy; // Unique pointer to the replacement policy used by the cache level
        std::unique_ptr<WritePolicy> writePolicy; // Unique pointer to the write policy used by the cache level

        EventHandler<EventHandlerCacheEventType> eventHandler;

        //void read(const AddressInfo& addressInfo, CacheRequest& request); // Function to read data from the cache based on address information and cache request
        void readSingleLine(const AddressInfo& addressInfo, CacheRequest& request); // Function to read data from a single cache line based on address information and cache request
        void readCrossLines(const AddressInfo& addressInfo1, const AddressInfo& addressInfo2, CacheRequest& request); // Function to read data that spans across two cache lines based on address information and cache request
        //void write(const AddressInfo& addressInfo, CacheRequest& request); // Function to write data to the cache based on address information and cache request
        void writeSingleLine(const AddressInfo& addressInfo, CacheRequest& request); // Function to write data to a single cache line based on address information and cache request
        void writeCrossLines(const AddressInfo& addressInfo1, const AddressInfo& addressInfo2, CacheRequest& request); // Function to write data that spans across two cache lines based on address information and cache request
        void onEviction(const AddressInfo& addressInfo, const CacheLine& evictedLine);
        void propagateWriteToNextLevel(CacheRequest& request, bool propagateCallback); // Function to propagate a write operation to the next cache level or memory based on the write policy
        //void fillLine(const AddressInfo& addressInfo, CacheRequest& request); // Function to fill a cache line with data from the next level or memory based on the cache request and address information
    
    
        public:
        CacheLevel(uint32_t size, uint8_t associativity, uint8_t latency, uint8_t fillLatency, Bus& bus, CacheLevel* nextLevel = nullptr, CacheLevel* parentLevel = nullptr, CacheLevelType type = CacheLevelType::NONE);
        ~CacheLevel() = default;

        void execute_operation() override; // Override of the pure virtual function from Device class

        LookUpResult lookupCache(const AddressInfo& addressInfo, TypeofData dataType); // Function to perform cache lookup based on address information and type of data being accessed, returns the result of the lookup (hit, miss, hit cross lines, miss cross lines)    
        void onHit(const AddressInfo& addressInfo, CacheRequest& request); // Function to handle cache hit events based on address information and cache request
        void onMiss(CacheRequest& request); // Function to handle cache miss events based on address information and cache request
        void onHitCrossLines(const AddressInfo& addressInfo1, const AddressInfo& addressInfo2, CacheRequest& request); // Function to handle cache hit events that span across two cache lines based on address information and cache request
        void onFill(const AddressInfo& addressInfo, [[maybe_unused]] CacheRequest& request, CacheLine& line); // Function to handle filling a cache line with data from the next level or memory based on the cache request and address information
        //void onMissCrossLines(const AddressInfo& addressInfo, CacheRequest& request); // Function to handle cache miss events that span across two cache lines based on address information and cache request
        void scheduleRequest(CacheRequest&& request); // Function to schedule a cache request for processing by the cache scheduler
        void schedulePendingRequest(PendingRequest&& pendingRequest); // Function to schedule a pending request for processing by the cache scheduler

        //getters and setters for the cache size and associativity
        uint32_t getCacheSize() const { return cacheSize; }
        uint8_t getAssociativity() const { return associativity; }
        uint32_t getNumSets() const { return numSets; }
        
        void setLatency(uint8_t latency) { latencyCycles = latency; }
        uint8_t getLatency() const { return latencyCycles; }

        void setFillLatency(uint8_t fillLatency) { latencyFill = fillLatency;}
        uint8_t getFillLatency() const { return latencyFill;}

        void setCacheSize(uint32_t size) { cacheSize = size; }
        void setAssociativity(uint8_t assoc) { associativity = assoc; }
        void setNumSets(uint32_t sets) { numSets = sets; };

        void setParentLevel(CacheLevel* parent) { parentLevel = parent; }
        CacheLevel* getParentLevel() const { return parentLevel; }

        void setNextLevel(CacheLevel* next) {nextLevel = next;}
        CacheLevel* getNextLevel() const { return nextLevel;}

        
        CacheStorage& getStorage() { return storage; }
};

class MemoryScheduler
{
    private:
        Bus& bus; // Reference to the bus for memory access
        uint64_t memoryLatency; // Latency of memory access in cycles
        std::vector<PendingRequest> memoryRequestQueue; // Queue to hold pending memory requests
    public:
        MemoryScheduler(Bus& bus, uint64_t latency) : bus(bus), memoryLatency(latency) {}
        //void tick(); // Function to be called every clock tick to process memory requests
        void scheduleMemoryRequest(CacheRequest&& request); // Function to schedule a memory request
        void schedulePendingRequest(PendingRequest&& pendingRequest); // Function to schedule a pending memory request
        void processMemoryRequests(); // Function to process scheduled memory requests
        void processMemoryRequest(PendingRequest&& pendingRequest); // Function to process a single memory request based on its type and data
        void setMemoryLatency(uint64_t latency) { memoryLatency = latency; }
        uint64_t getMemoryLatency() const { return memoryLatency; }
};
/// Cache manager class to manage multiple cache levels
class CacheManager : public Device
{   
    public:
        CacheManager(Bus& bus,uint32_t l1Size, uint32_t l2Size, uint32_t l3Size, uint8_t l1Assoc, uint8_t l2Assoc, uint8_t l3Assoc, uint8_t l1Latency = 1, uint8_t l2Latency = 2, uint8_t l3Latency = 3, uint8_t l1FillLatency=1, uint8_t l2FillLatency = 1, uint8_t l3FillLatency = 1);
        ~CacheManager();

        CacheManager(const CacheManager&) = delete;
        CacheManager& operator=(const CacheManager&) = delete;

        void execute_operation() override; // Override of the pure virtual function from Device class

        void enqueRequest(CacheRequest&& request) { requestQueue.push_back(std::move(request));}

        void flushAllCaches();
        void invalidateAllCaches();
        void printCacheState() const; // For debugging purposes

        CacheLevel& getL1Cache() { return L1Cache; }
        CacheLevel& getL2Cache() { return L2Cache; }
        CacheLevel& getL3Cache() { return L3Cache; }
        MemoryScheduler& getMemoryScheduler() { return memoryScheduler; }

        uint8_t memoryLatency = 1;

    protected:
        CacheLevel L3Cache;
        CacheLevel L2Cache;
        CacheLevel L1Cache;
        MemoryScheduler memoryScheduler; // Memory scheduler for managing memory requests

    private:
        Bus& bus; // Reference to the bus
        // Queue to hold cache requests
        std::vector<CacheRequest> requestQueue;


};

/// Function to manage cache offset errors
template<typename T>
bool offset_cache(EventType event, ErrorType error, Result<T>& result, uint64_t offset, uint64_t address)
{
    if (offset + sizeof(T) > CACHE_LINE_SIZE)
    {
        result.success = false;
        result.errorInfo.event = event;
        result.errorInfo.source = ComponentType::CACHE;
        result.errorInfo.message = "Read exceeds cache line boundary at address: " + std::to_string(address);
        result.errorInfo.error = error;
        return true; // Indicate that there was an error
        
    }
    return false; // No error
}

//specialization for void and lines 
template<>
bool offset_cache(EventType event, ErrorType error, Result<void>& result, uint64_t offset, uint64_t address);

template<>
bool offset_cache(EventType event, ErrorType error, Result<std::array<uint8_t, CACHE_LINE_SIZE>>& result, uint64_t offset, uint64_t address);

template<>
bool offset_cache(EventType event, ErrorType error, Result<CacheLine>& result, uint64_t offset, uint64_t address);


#endif //CACHEMANAGER_HPP