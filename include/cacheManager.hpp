#ifndef CACHEMANAGER_HPP
#define CACHEMANAGER_HPP
#include <cstdint>
#include <cstring>
#include <vector>
#include <array>
#include <iostream>
#include "helpers.hpp"
#include "bus.hpp"
#include "memory.hpp"
#include <variant>
#include <queue>
#include <memory>
#include  <list>




//basic structure for the cache line
//it contains the data, the tag, the valid bit and the dirty bit
struct CacheLine
{
    bool valid;
    bool dirty; // Indicates if the line has been modified

    uint64_t tag;
    std::array<uint8_t, CACHE_LINE_SIZE> data;  // Data stored in the cache line
    
    uint64_t lastAccessTime; // use it as a counter for replacement policy, syncronized with clock when access

    CacheLine() : valid(false), dirty(false), tag(0), data{}, lastAccessTime(0) {}


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
    std::unique_ptr<CacheRequest> request;
    RequestState state = RequestState::IDLE;
    int remainingLatency; // Remaining latency in ticks
    CacheLine line= CacheLine{}; //copy of the line 

    PendingRequest(std::unique_ptr<CacheRequest> req, int latency)
        : request(std::move(req)), remainingLatency(latency) {}

    PendingRequest(std::unique_ptr<CacheRequest> req, int latency, CacheLine res)
        : request(std::move(req)), remainingLatency(latency), line(res) {}
    PendingRequest() = default;
};


struct AddressInfo
{
    uint64_t address;
    uint64_t setIndex;
    uint64_t tag;
    uint64_t offset;

    AddressInfo(uint64_t addr, uint64_t setIdx, uint64_t tg, uint64_t off)
        : address(addr), setIndex(setIdx), tag(tg), offset(off) {}
};



class ReplacementPolicy
{
    public:
        virtual uint64_t selectLineToReplace(CacheSet& set) = 0; // Pure virtual function to select a line to replace based on the replacement policy
        virtual void updateOnAccess(CacheSet& set, uint64_t lineIndex) = 0; // Pure virtual function to update the replacement policy state on cache access
        virtual void onLineLoaded(CacheSet& set, uint64_t lineIndex) = 0; // Pure virtual function to update the replacement policy state when a line is loaded into the cache
        virtual ~ReplacementPolicy() = default; // Virtual destructor for proper cleanup of derived classes
};

struct LRUState
{
   std::list<uint64_t> lineIndices; // List to maintain the order of line indices based on recency of access (least recently accessed at the front)
   std::unordered_map<uint64_t, std::list<uint64_t>::iterator> indexMap; // Map to quickly access the position of a line index in the list for O(1) updates

};
//LRU replacement policy implementation
class LRUReplacementPolicy : public ReplacementPolicy
{
    private:
        std::unordered_map<uint64_t, LRUState> lruMap; // Map to hold the LRU state for each cache set (set index mapped to a vector of line indices ordered by recency of access)
    public:
        LRUReplacementPolicy(const uint64_t& numSets, const uint64_t& associativity); // Constructor to initialize the LRU state for all cache sets based on the number of sets and associativity
        void initializeSet(uint64_t setIndex, uint64_t associativity); // Function to initialize the LRU state for a cache set
        uint64_t selectLineToReplace(CacheSet& set) override; // Override of the function to select a line to replace based on LRU policy
        void updateOnAccess(CacheSet& set, uint64_t lineIndex) override; // Override of the function to update the LRU state on cache access
        void onLineLoaded(CacheSet& set, uint64_t lineIndex) override; // Override of the function to update the LRU state when a line is loaded into the cache
};


struct PLRUTree
{
    std::vector<bool> bits; // Vector to hold the bits of the PLRU tree, where each bit indicates the direction to take for replacement (0 for left, 1 for right)

    PLRUTree(uint64_t associativity) : bits(associativity - 1, false) {} // Constructor to initialize the PLRU tree based on the associativity of the cache set
};
//PLRU replacement policy implementation    
class PLRUReplacementPolicy : public ReplacementPolicy
{
    private:
        std::unordered_map<uint64_t, PLRUTree> plruMap; // Map to hold the PLRU state for each cache set (set index mapped to a vector of bits representing the PLRU tree)
    public:
        PLRUReplacementPolicy(const uint64_t& numSets, const uint64_t& associativity); // Constructor to initialize the PLRU state for all cache sets based on the number of sets and associativity
        uint64_t selectLineToReplace(CacheSet& set) override; // Override of the function to select a line to replace based on PLRU policy
        void updateOnAccess(CacheSet& set, uint64_t lineIndex) override; // Override of the function to update the PLRU state on cache access
        void onLineLoaded(CacheSet& set, uint64_t lineIndex) override; // Override of the function to update the PLRU state when a line is loaded into the cache

};

//random replacement policy implementation
class RandomReplacementPolicy : public ReplacementPolicy
{
    public:
        uint64_t selectLineToReplace(CacheSet& set) override; // Override of the function to select a line to replace based on random policy
        void updateOnAccess(CacheSet& set, uint64_t lineIndex) override {} // No state to update for random replacement policy
        void onLineLoaded(CacheSet& set, uint64_t lineIndex) override {} // No state to update when a line is loaded for random replacement policy
};

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




class CacheStorage
{
    private:
        std::vector<CacheSet> sets; // Cache sets

    public:
        CacheStorage(uint64_t numSets, uint64_t associativity); // Constructor to initialize the cache storage with the specified number of sets and associativity
        CacheSet& getSet(uint64_t setIndex);
        std::vector<CacheSet>& getSets() { return sets; }
        CacheLine* findLine(uint64_t setIndex, uint64_t tag); // Function to find a cache line based on set index and tag
        uint64_t findFreeLineIndex(uint64_t setIndex); // Function to find a free line index in a set
        void invalidateLine(uint64_t setIndex, uint64_t lineIndex); // Function to invalidate a specific cache line
        void invalidateAllLines(); // Function to invalidate all cache lines in the cache storage
        void invalidateLineByAddress(AddressInfo addressInfo); // Function to invalidate a specific cache line based on address information
        void flush();

};

class RequestScheduler
{
    private:
        std::vector<PendingRequest> pendingRequests; // Vector to hold pending requests being processed by the cache scheduler
        EventHandler& controllerEventHandler; // Reference to the event handler for managing cache events and callbacks
    public:
        RequestScheduler(EventHandler& handler) : controllerEventHandler(handler) {}
        void tick(); // Function to be called every clock tick to process pending requests
        void scheduleRequest(std::unique_ptr<CacheRequest>&& request); // Function to schedule a cache request
        void processRequests(); // Function to process scheduled cache requests
};

class CacheController
{
    private:
        EventHandler& cacheEventHandler; // Event handler for managing cache events and callbacks
        EventHandler  controllerEventHandler; // Event handler for managing controller-specific events and callbacks
    public:
    CacheController(EventHandler& eventHandler) : cacheEventHandler(eventHandler) {}
    void lookupCache(const AddressInfo& addressInfo); // Function to perform cache lookup based on address information
    void handleRequest(CacheRequest& request); // Function to handle incoming cache requests and coordinate the cache operations
    
};

/// Cache level structure(L1, L2, L3)
/// Contains multiple cache sets and manages the cache operations
class CacheLevel: public Device
{
    private:
        //std::vector<CacheSet> sets; // Cache sets in the cache level
        CacheStorage storage; // Cache storage for managing the cache lines and sets
        CacheController controller; // Cache controller for managing cache operations
        RequestScheduler scheduler; // Request scheduler for managing cache requests and their timing
        EventHandler eventHandler; // Event handler for managing cache events and callbacks
        std::unique_ptr<ReplacementPolicy> replacementPolicy; // Unique pointer to the replacement policy used by the cache level
        std::unique_ptr<WritePolicy> writePolicy; // Unique pointer to the write policy used by the cache level
        uint64_t latency_cycles; // Latency of the cache level in cycles
        uint64_t cacheSize;
        uint64_t associativity;
        uint64_t numSets;
        Bus &bus; // Reference to the bus for memory access
        CacheLevel* nextLevel = nullptr; // Pointer to the next cache level (L2 or L3)
        CacheLevel* parentLevel = nullptr; // Pointer to the upper cache level (L1 or L2)

        void read(const AddressInfo& addressInfo, CacheRequest& request); // Function to read data from the cache based on address information and cache request
        void readSingle(const AddressInfo& addressInfo, CacheRequest& request); // Function to read data from a single cache line based on address information and cache request
        void readCrossLines(const AddressInfo& addressInfo, CacheRequest& request); // Function to read data that spans across two cache lines based on address information and cache request
        void write(const AddressInfo& addressInfo, CacheRequest& request); // Function to write data to the cache based on address information and cache request
        void writeSingleLine(const AddressInfo& addressInfo, CacheRequest& request); // Function to write data to a single cache line based on address information and cache request
        void writeCrossLines(const AddressInfo& addressInfo, CacheRequest& request); // Function to write data that spans across two cache lines based on address information and cache request

    
    public:
        CacheLevel(uint64_t size, uint64_t associativity, uint64_t latency, Bus& bus, CacheLevel* nextLevel, CacheLevel* parentLevel = nullptr);
        ~CacheLevel();

        void execute_operation() override; // Override of the pure virtual function from Device class

        AddressInfo DecodeAddress(uint64_t address); // Function to calculate the set index, tag and offset from a memory address


        //Result<CacheLine> readSingle(uint64_t address); // Read data from the cache also with metadata

        //Result<std::array<uint8_t, 2*CACHE_LINE_SIZE>> read(uint64_t address, uint64_t size, bool*twoLines = nullptr); // Read data from the cache based on a cache request

        //Result<std::array<uint8_t, 2*CACHE_LINE_SIZE>> readCrossLines(uint64_t address, uint64_t size); // Read data that spans across two cache lines

        //Result<void> write(uint64_t address, const std::array<uint8_t, CACHE_LINE_SIZE>& data, uint64_t size, bool* twoLines = nullptr); // Write data to the cache

        //Result<void> writeSingleLine(uint64_t address, const std::array<uint8_t, CACHE_LINE_SIZE>& data, uint64_t size); // Write data to a single cache line

        //Result<void> writeCrossLines(uint64_t address, const std::array<uint8_t, CACHE_LINE_SIZE>& data, uint64_t size); // Write data that spans across two cache lines

        //void load(uint64_t setIndex, uint64_t tag, const CacheLine& data, uint64_t freePosition); // Load an entire cache line into the cache with metadata

        //void processRequest();

        bool lookupCache(const AddressInfo& addressInfo);
        void onHit(const AddressInfo& addressInfo);
        void onMiss(const AddressInfo& addressInfo);
        void onEviction(const AddressInfo& addressInfo, const CacheLine& evictedLine);

        void accessRead(const AddressInfo& addressInfo, CacheRequest& request);
        void accessWrite(const AddressInfo& addressInfo, CacheRequest& request);

        // Function to find a cache line in a set
        //uint64_t manageReplacementPolicy(CacheSet& set);

        //getters and setters for the cache size and associativity
        uint64_t getCacheSize() const { return cacheSize; }
        uint64_t getAssociativity() const { return associativity; }
        uint64_t getNumSets() const { return numSets; }
        
        void setLatency(uint64_t latency) { latency_cycles = latency; }
        uint64_t getLatency() const { return latency_cycles; }

        void setCacheSize(uint64_t size) { cacheSize = size; }
        void setAssociativity(uint64_t assoc) { associativity = assoc; }
        void setNumSets(uint64_t sets) { numSets = sets; };

        void setParentLevel(CacheLevel* parent) { parentLevel = parent; }
        CacheLevel* getParentLevel() const { return parentLevel; }

  


};

class MemoryScheduler
{
    private:
        uint64_t memoryLatency; // Latency of memory access in cycles
        std::queue<std::unique_ptr<PendingRequest>> memoryRequestQueue; // Queue to hold pending memory requests
    public:
        MemoryScheduler(uint64_t latency) : memoryLatency(latency) {}
        void tick(); // Function to be called every clock tick to process memory requests
        void scheduleMemoryRequest(std::unique_ptr<CacheRequest>&& request); // Function to schedule a memory request
        void processMemoryRequests(); // Function to process scheduled memory requests
        void setMemoryLatency(uint64_t latency) { memoryLatency = latency; }
        uint64_t getMemoryLatency() const { return memoryLatency; }
};
/// Cache manager class to manage multiple cache levels
class CacheManager : public Device
{   
    public:
        CacheManager(Bus& bus,uint64_t l1Size, uint64_t l2Size, uint64_t l3Size, uint64_t l1Assoc, uint64_t l2Assoc, uint64_t l3Assoc, uint64_t l1Latency = 1, uint64_t l2Latency = 2, uint64_t l3Latency = 3);
        ~CacheManager();

        CacheManager(const CacheManager&) = delete;
        CacheManager& operator=(const CacheManager&) = delete;

        void execute_operation() override; // Override of the pure virtual function from Device class

        //void processRequest(); // Function to process cache requests

        //void enqueueMemoryRequest(std::unique_ptr<CacheRequest>&& request) { requestQueueMemory.push(std::move(request)); }

        void flushAllCaches();
        void invalidateAllCaches();
        void printCacheState() const; // For debugging purposes

        //void setRequest(std::unique_ptr<CacheRequest>&& request) { requestQueue.push(std::move(request)); } // Set the request queue

        CacheLevel& getL1Cache() { return L1Cache; }
        CacheLevel& getL2Cache() { return L2Cache; }
        CacheLevel& getL3Cache() { return L3Cache; }
        MemoryScheduler& getMemoryScheduler() { return memoryScheduler; }

        void setMemoryLatency(uint64_t latency) { memory_latency = latency; memoryScheduler.setMemoryLatency(latency); }
        uint64_t getMemoryLatency() const { return memory_latency; }

    protected:
        CacheLevel L1Cache;
        CacheLevel L2Cache;
        CacheLevel L3Cache;
        MemoryScheduler memoryScheduler; // Memory scheduler for managing memory requests

    private:
        Bus& bus; // Reference to the bus
        // Queue to hold cache requests
        std::queue<std::unique_ptr<CacheRequest>> requestQueue;
        int memory_latency = 0;


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









/*
template <typename T>
Result<T> CacheManager::read(uint64_t address)
{
    
    uint64_t l1SetIndex = (address / CACHE_LINE_SIZE) % L1Cache.getNumSets(); // Calculate the L1 set index
    uint64_t l1Tag = address / (CACHE_LINE_SIZE * L1Cache.getNumSets()); // Calculate the L1 tag

    uint64_t l2SetIndex = (address / CACHE_LINE_SIZE) % L2Cache.getNumSets(); // Calculate the L2 set index
    uint64_t l2Tag = address / (CACHE_LINE_SIZE * L2Cache.getNumSets()); // Calculate the L2 tag

    uint64_t l3SetIndex = (address / CACHE_LINE_SIZE) % L3Cache.getNumSets(); // Calculate the L3 set index
    uint64_t l3Tag = address / (CACHE_LINE_SIZE * L3Cache.getNumSets()); // Calculate the L3 tag



    uint64_t offset = address % CACHE_LINE_SIZE; // Calculate the offset within the cache line

    debugLog("Reading from cachemanager at address: " + to_string_hex(address));

    //temporary result that holds the line read from cache
    Result<std::array<uint8_t, CACHE_LINE_SIZE * 2>> temporary_result;

    bool twoLines = false;

    // Try to read from L1 cache
    if(offset + sizeof(T) > CACHE_LINE_SIZE)
    {
        twoLines = true;
        temporary_result = readCrossLines(address); // Read 2 lines
    }
    else
    {
        //function for copying arrays of different size inside result
        copyPartial(temporary_result, readSingleLine(address, l1SetIndex, l1Tag, l2SetIndex, l2Tag, l3SetIndex, l3Tag, offset)); // Initialize to avoid uninitialized warning
       
    }

    Result<T> final_result{};

    T data{};


    if(twoLines)
    {
        //extracting the first part of the instrcutuion/data from the first line
        std::memcpy(&data, temporary_result.data.data() + offset, CACHE_LINE_SIZE - offset);

        //extracting the second part of the instruction/data from the second line
        std::memcpy(reinterpret_cast<uint8_t*>(&data) + (CACHE_LINE_SIZE - offset), temporary_result.data.data()+ CACHE_LINE_SIZE, sizeof(T) - (CACHE_LINE_SIZE - offset));
    }
    else
    {
        //extracting the instruction/data from the line
        std::memcpy(&data, temporary_result.data.data() + offset, sizeof(T));
    }
    

    final_result.success = temporary_result.success;
    final_result.errorInfo = temporary_result.errorInfo;
    final_result.data = data;
    return final_result;
   
}

template <typename T>
Result<void> CacheManager::write(uint64_t address, const T& data)
{
    uint64_t l1SetIndex = (address / CACHE_LINE_SIZE) % L1Cache.getNumSets(); // Calculate the L1 set index
    uint64_t l1Tag = address / (CACHE_LINE_SIZE * L1Cache.getNumSets()); // Calculate the L1 tag

    uint64_t l2SetIndex = (address / CACHE_LINE_SIZE) % L2Cache.getNumSets(); // Calculate the L2 set index
    uint64_t l2Tag = address / (CACHE_LINE_SIZE * L2Cache.getNumSets()); // Calculate the L2 tag

    uint64_t l3SetIndex = (address / CACHE_LINE_SIZE) % L3Cache.getNumSets(); // Calculate the L3 set index
    uint64_t l3Tag = address / (CACHE_LINE_SIZE * L3Cache.getNumSets()); // Calculate the L3 tag

    uint64_t offset = address % CACHE_LINE_SIZE; // Calculate the offset within the cache line

    

    // Check if the write spans two cache lines
    if (offset + sizeof(T) > CACHE_LINE_SIZE)
    {
        // Write spans two cache lines, need to read both lines first
        return writeCrossLines(address, data);
        
    }
    else
    {
        //write is contained in a single line
        return writeSingleLine(address, data, l1SetIndex, l1Tag, l2SetIndex, l2Tag, l3SetIndex, l3Tag, offset);


    }
}


template< typename T>
Result<void> CacheManager::writeSingleLine(uint64_t address, const T& data, uint64_t l1SetIndex, uint64_t l1Tag, uint64_t l2SetIndex, uint64_t l2Tag, uint64_t l3SetIndex, uint64_t l3Tag, uint64_t offset)
{

    //temporary result that holds the line read from cache(it is used to load the line from lower levels or RAM)
    Result<CacheLine> read_result;
    

    // Try to write to L1 cache
    Result<void>result = L1Cache.write(address, data); // Write to L1 cache

    if(result.errorInfo.error == ErrorType::WRITE_FAIL)
    {
        return result;
    }

    if (result.success) // Check if the write was successful
    {
        return result;
    }
    else
    {
        //searching the line in L2
        read_result = L2Cache.read(address);

        if(read_result.success)
        {
            // Find a free line in L1 cache
            uint64_t freePosition = L1Cache.findFreeLineIndex(L1Cache.getSets()[l1SetIndex]); 

            // Load the data into L1 cache
            L1Cache.load(l1SetIndex, l1Tag, read_result.data, freePosition); // Load the data into L1 cache

            result = L1Cache.write(address, data); // Write to L2 cache

            return result;
        }
        else
        {
            //searching the line in L3
            read_result = L3Cache.read(address);

            if(read_result.success)
            {
                // Find a free line in L2 cache
                uint64_t freePosition = L2Cache.findFreeLineIndex(L2Cache.getSets()[l2SetIndex]); 

                // Load the data into L2 cache
                L2Cache.load(l2SetIndex, l2Tag, read_result.data, freePosition); // Load the data into L2 cache

                // Find a free line in L1 cache
                freePosition = L1Cache.findFreeLineIndex(L1Cache.getSets()[l1SetIndex]); 

                // Load the data into L1 cache
                L1Cache.load(l1SetIndex, l1Tag, read_result.data, freePosition); // Load the data into L1 cache

                result = L1Cache.write(address, data); // Write to L1 cache

                return result;
            }
            else
            {
                //calculating the start of the line
                uint64_t lineStart = address - offset;

                Result<std::array<uint8_t, CACHE_LINE_SIZE>> ram_result;
                //reading the line from RAM
                ram_result = bus.getMemory().template readGeneric<std::array<uint8_t, CACHE_LINE_SIZE>>(lineStart); // Read from RAM

                read_result.success = ram_result.success;
                read_result.errorInfo = ram_result.errorInfo;
                read_result.data.data = ram_result.data;
                read_result.data.valid = true;
                read_result.data.dirty = false;
                read_result.data.tag = l3Tag;

                if(read_result.success)
                {
                    // Find a free line in L3 cache
                    uint64_t freePosition = L3Cache.findFreeLineIndex(L3Cache.getSets()[l3SetIndex]); 

                    // Load the data into L3 cache
                    L3Cache.load(l3SetIndex, l3Tag, read_result.data, freePosition); // Load the data into L3 cache

                    // Find a free line in L2 cache
                    freePosition = L2Cache.findFreeLineIndex(L2Cache.getSets()[l2SetIndex]); 

                    // Load the data into L2 cache
                    L2Cache.load(l2SetIndex, l2Tag, read_result.data, freePosition); // Load the data into L2 cache

                    // Find a free line in L1 cache
                    freePosition = L1Cache.findFreeLineIndex(L1Cache.getSets()[l1SetIndex]); 

                    // Load the data into L1 cache
                    L1Cache.load(l1SetIndex, l1Tag, read_result.data, freePosition); // Load the data into L1 cache

                    result = L1Cache.write(address, data); // Write to L1 cache

                    return result;
                }
                else
                {
                    // RAM access failed, return error
                    result.success = false; // Set success to false

                    // Set the event type to RAM_ACCESS
                    result.errorInfo.event = EventType::RAM_ACCESS; // Set the event type to RAM_ACCESS
                    result.errorInfo.source = ComponentType::RAM; // Set the source to RAM
                    result.errorInfo.message = "RAM access failed at address: " + to_string_hex(address); // Set the message for debugging
                    result.errorInfo.error = ErrorType::WRITE_FAIL; // Set the error type to WRITE_FAIL

                    return result; // Return the result

                }
            }
        }
    };
}

template< typename T>
Result<void> CacheManager::writeCrossLines(uint64_t address, const T& data)
{
    // Write data spanning two cache lines
    Result<void> result;

    uint64_t l1SetIndex = (address / CACHE_LINE_SIZE) % L1Cache.getNumSets(); // Calculate the L1 set index
    uint64_t l1Tag = address / (CACHE_LINE_SIZE * L1Cache.getNumSets()); // Calculate the L1 tag

    uint64_t l2SetIndex = (address / CACHE_LINE_SIZE) % L2Cache.getNumSets(); // Calculate the L2 set index
    uint64_t l2Tag = address / (CACHE_LINE_SIZE * L2Cache.getNumSets()); // Calculate the L2 tag

    uint64_t l3SetIndex = (address / CACHE_LINE_SIZE) % L3Cache.getNumSets(); // Calculate the L3 set index
    uint64_t l3Tag = address / (CACHE_LINE_SIZE * L3Cache.getNumSets()); // Calculate the L3 tag

    uint64_t offset = address % CACHE_LINE_SIZE; // Calculate the offset within the cache line

    // Write the first part of the data to the first cache line
    auto result1 = writeSingleLine(address, data, l1SetIndex, l1Tag, l2SetIndex, l2Tag, l3SetIndex, l3Tag, offset);
    if (!result1.success)
    {
        result.success = false;
        result.errorInfo = result1.errorInfo;
        return result;
    }

    uint64_t new_address = address + (CACHE_LINE_SIZE - offset);
    l1SetIndex = (new_address / CACHE_LINE_SIZE) % L1Cache.getNumSets(); // Calculate the L1 set index
    l1Tag = new_address / (CACHE_LINE_SIZE * L1Cache.getNumSets()); // Calculate the L1 tag

    l2SetIndex = (new_address / CACHE_LINE_SIZE) % L2Cache.getNumSets(); // Calculate the L2 set index
    l2Tag = new_address / (CACHE_LINE_SIZE * L2Cache.getNumSets());

    l3SetIndex = (new_address / CACHE_LINE_SIZE) % L3Cache.getNumSets(); // Calculate the L3 set index
    l3Tag = new_address / (CACHE_LINE_SIZE * L3Cache.getNumSets());

    offset = new_address % CACHE_LINE_SIZE; // Calculate the offset within the cache line

    // Write the second part of the data to the second cache line
    auto result2 = writeSingleLine(new_address, reinterpret_cast<const uint8_t*>(&data) + (CACHE_LINE_SIZE - offset), l1SetIndex, l1Tag, l2SetIndex, l2Tag, l3SetIndex, l3Tag, offset);
    if (!result2.success)
    {
        result.success = false;
        result.errorInfo = result2.errorInfo;
        return result;
    }   
    result.success = true;
    result.errorInfo.event = EventType::CACHE_HIT; // Set the event type to CACHE_HIT
    result.errorInfo.source = ComponentType::CACHE; // Set the source to CACHE
    result.errorInfo.message = "Write completed successfully at address: " + to_string_hex(address); // Set the message for debugging
    result.errorInfo.error = ErrorType::NONE; // Set the error type to NONE
    return result;
}
*/

#endif //CACHEMANAGER_HPP