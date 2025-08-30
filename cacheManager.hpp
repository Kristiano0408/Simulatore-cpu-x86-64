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

constexpr unsigned CACHE_LINE_SIZE = 64; // Size of a cache line in bytes


//basic structure for the cache line
//it contains the data, the tag, the valid bit and the dirty bit
struct CacheLine
{
    bool valid;
    bool dirty; // Indicates if the line has been modified
    uint64_t tag;
    std::array<uint8_t, CACHE_LINE_SIZE> data;  // Data stored in the cache line
    uint64_t lastAccessTime; // Since we have not a cloack yet, we use it as a counter for replacement policy


};

/// Cache set structure
/// Contains multiple cache lines and the set index
struct CacheSet
{
    std::vector<CacheLine> lines; // Lines in the cache set
    uint64_t setIndex;
};


/// Cache level structure
/// Contains multiple cache sets and manages the cache operations
class CacheLevel
{
    private:
        std::vector<CacheSet> sets; // Cache sets
        uint64_t cacheSize;
        uint64_t associativity;
        uint64_t numSets;
        Bus &bus; // Reference to the bus for memory access
        CacheLevel* nextLevel = nullptr; // Pointer to the next cache level (L2 or L3)
        

    public:
        CacheLevel(uint64_t size, uint64_t associativity, Bus& bus, CacheLevel* nextLevel);
        ~CacheLevel();
        Result<std::array<uint8_t, CACHE_LINE_SIZE>> read(uint64_t address);
        template <typename T>
        Result<void> write(uint64_t address, const T& data); // Write data to the cache
        void load(uint64_t setIndex, uint64_t tag, const std::array<uint8_t, CACHE_LINE_SIZE>& data, uint64_t freePosition);




        // Function to find a cache line in a set
        CacheLine* findLine(const CacheSet& set, uint64_t tag);

        uint64_t manageReplacementPolicy(CacheSet& set);
        
        uint64_t findFreeLineIndex(CacheSet& set);

        void invalidate(uint64_t address);
        void invalidateAll(); // Invalidate all lines in the cache
        void flush();
        void printCacheState() const; // For debugging purposes

        //getters and setters for the cache size and associativity
        uint64_t getCacheSize() const { return cacheSize; }
        uint64_t getAssociativity() const { return associativity; }
        uint64_t getNumSets() const { return numSets; }
        std::vector<CacheSet>& getSets() { return sets; }


        void setCacheSize(uint64_t size) { cacheSize = size; }
        void setAssociativity(uint64_t assoc) { associativity = assoc; }
        void setNumSets(uint64_t sets) { numSets = sets; };

  


};


// Cache manager class to manage multiple cache levels
class CacheManager
{   
    public:
        CacheManager(Bus& bus,uint64_t l1Size, uint64_t l2Size, uint64_t l3Size, uint64_t l1Assoc, uint64_t l2Assoc, uint64_t l3Assoc);
        ~CacheManager();
        template <typename T>
        Result<T> read(uint64_t address);
        template <typename T>
        Result<void> write(uint64_t address, const T& data); // Write data to the cache
        Result<std::array<uint8_t, CACHE_LINE_SIZE * 2>> readCrossLines(uint64_t address);
        Result<std::array<uint8_t, CACHE_LINE_SIZE>> readSingleLine(uint64_t address, uint64_t l1SetIndex, uint64_t l1Tag, uint64_t l2SetIndex, uint64_t l2Tag, uint64_t l3SetIndex, uint64_t l3Tag, uint64_t offset);
        void flushAllCaches();
        void invalidateAllCaches();
        void printCacheState() const; // For debugging purposes

    private:
        CacheLevel L1Cache;
        CacheLevel L2Cache;
        CacheLevel L3Cache;
        Bus& bus; // Reference to the bus
};





// Function to manage cache offset errors
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


//tempalte functions implementation (cachelevel:read is the only one that dont need it,cus it always read an entire line)



template <typename T>
Result<void> CacheLevel::write(uint64_t address, const T& data)
{
    // Create a result structure for the write operation
    Result<void> result;

    constexpr unsigned offsetBits = ilog2_constexpr(CACHE_LINE_SIZE); // Calculate the number of bits for the offset
    unsigned indexBits = ilog2(numSets); // Calculate the number of bits for the index

    uint64_t offset = address & ((1ULL << offsetBits) - 1); // Calculate the offset within the cache line

    //manage the offset for the write operation
    if (offset_cache(EventType::CACHE_WRITE_ERROR, ErrorType::WRITE_FAIL, result, offset, address))
    {
        return result;
    }

    uint64_t setIndex = (address >> offsetBits) & ((1ULL << indexBits) - 1); // Calculate the set index
    uint64_t tag = address >> (offsetBits + indexBits); // Calculate the tag


    
    CacheSet& set = sets[setIndex]; // Get the cache set

    auto* line = findLine(set, tag); // Check if the line is in the cache

    if(line != nullptr)
    {
        
        // Cache hit
        line->dirty = true; // Mark the line as dirty
        line->lastAccessTime = bus.getClock().getCycles(); // Update the last access time

        // Write the data to the cache line
        std::memcpy(&line->data[offset], &data, sizeof(T)); 

        result.success = true; // Set success to true

        // Set the event type to CACHE_HIT
        result.errorInfo.event = EventType::CACHE_HIT; // Set the event type to CACHE_HIT
        result.errorInfo.source = ComponentType::CACHE; // Set the source to CACHE
        result.errorInfo.message = "Cache hit at address: " + std::to_string(address); // Set the message for debugging
        result.errorInfo.error = ErrorType::NONE; // Set the error type to NONE

        std::cout << "Cache hit at address: " << std::hex << address << std::endl; // Print the cache hit message
        
        return result; // Return the result

    }
    else
    {
        // Cache miss
        result.success = false; // Set success to false

        // Set the event type to CACHE_MISS
        result.errorInfo.event = EventType::CACHE_MISS; // Set the event type to CACHE_MISS
        result.errorInfo.source = ComponentType::CACHE; // Set the source to CACHE
        result.errorInfo.message = "Cache miss at address: " + std::to_string(address); // Set the message for debugging
        result.errorInfo.error = ErrorType::NONE;

        std::cout << "Cache miss at address: " << std::hex << address << std::endl; // Print the cache miss message

        return result; // Return the result
    }
    
}

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

    std::cout << "Reading from cachemanager at address: " << std::hex << address << std::endl;

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

    Result<std::array<uint8_t,CACHE_LINE_SIZE> > read_result;

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

                //reading the line from RAM
                read_result = bus.getMemory().template readGeneric<std::array<uint8_t, CACHE_LINE_SIZE>>(lineStart); // Read from RAM

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
                    result.errorInfo.message = "RAM access failed at address: " + std::to_string(address); // Set the message for debugging
                    result.errorInfo.error = ErrorType::WRITE_FAIL; // Set the error type to WRITE_FAIL

                    return result; // Return the result

                }
            }
        }

    }
}
   

#endif //CACHEMANAGER_HPP