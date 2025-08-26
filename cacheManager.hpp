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
        

    public:
        CacheLevel(uint64_t size, uint64_t associativity, Bus& bus);
        ~CacheLevel();
        template <typename T>
        Result<T> read(uint64_t address);
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




//tempalte functions implementation

template <typename T>
Result<T> CacheLevel::read(uint64_t address)
{
    //craetion of the structure for the result
    Result<T> result;

    //bitwise index/tag calculation
    constexpr unsigned offsetBits = ilog2_constexpr(CACHE_LINE_SIZE); // Calculate the number of bits for the offset
    unsigned indexBits = ilog2(numSets); // Calculate the number of bits for the index

    uint64_t offset = address & ((1ULL << offsetBits) - 1); // Calculate the offset within the cache line

    //manage the offset for the read operation
    if (offset_cache(EventType::CACHE_READ_ERROR, ErrorType::READ_FAIL, result, offset, address)) 
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

        // Read the data from the cache line
        std::memcpy(&result.data, &line->data[offset], sizeof(T)); // Copy the data from the cache line to the result

        line->lastAccessTime = bus.getClock().getCycles(); // Update the last access time

        // Set success to true
        result.success = true; 

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
        result.errorInfo.error = ErrorType::NONE; // Set the error type to NONE

        std::cout << "Cache miss at address: " << std::hex << address << std::endl; // Print the cache miss message

        return result; // Return the result
    }
    
    
}

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

    //uint64_t offset = address % CACHE_LINE_SIZE; // Calculate the offset within the cache line


    // Try to read from L1 cache
    Result<T> temporary_result = L1Cache.read<T>(address); // Read from L1 cache

    if(temporary_result.errorInfo.error == ErrorType::READ_FAIL)
    {
        return temporary_result; // Return the result if there was an error in the read operation
    }

    if (temporary_result.success) // Check if the read was successful
    {
        //changing the event type from generic cache to l1 cache
        temporary_result.errorInfo.source = ComponentType::CACHE_L1; // Set the source to CACHE_L1
        
        return temporary_result; // Return the result
    }
    else
    {
        // Cache miss in L1, try L2 cache
        temporary_result = L2Cache.read<T>(address); // Read from L2 cache

        if(temporary_result.errorInfo.error == ErrorType::READ_FAIL)
        {
        return temporary_result; // Return the result if there was an error in the read operation
        }

        // Check if the read was successful
        if(temporary_result.success)
        {
            //load the data into L1 cache

            // Find a free line in L1 cache
            uint64_t freePosition = L1Cache.findFreeLineIndex(L1Cache.getSets()[l1SetIndex]); 

            // Load the data into L1 cache
            L1Cache.load(l1SetIndex, l1Tag, temporary_result.data, freePosition); // Load the data into L1 cache

            //changing the event type from generic cache to l2 cache
            temporary_result.errorInfo.source = ComponentType::CACHE_L2; // Set the source to CACHE_L2

            return temporary_result; // Return the result

        }
        else
        {
            // Cache miss in L2, try L3 cache
            temporary_result = L3Cache.read<T>(address); // Read from L3 cache

            if(temporary_result.errorInfo.error == ErrorType::READ_FAIL)
            {
                return temporary_result; // Return the result if there was an error in the read operation
            }

            // Check if the read was successful
            if(temporary_result.success)
            {
                //load the data into L2 cache

                // Find a free line in L2 cache
                uint64_t freePosition = L2Cache.findFreeLineIndex(L2Cache.getSets()[l2SetIndex]); 

                // Load the data into L2 cache
                L2Cache.load(l2SetIndex, l2Tag, temporary_result.data, freePosition); // Load the data into L2 cache

                //load the data into L1 cache

                // Find a free line in L1 cache
                freePosition = L1Cache.findFreeLineIndex(L1Cache.getSets()[l1SetIndex]); 

                // Load the data into L1 cache
                L1Cache.load(l1SetIndex, l1Tag, temporary_result.data, freePosition); // Load the data into L1 cache

                //changing the event type from generic cache to l3 cache
                temporary_result.errorInfo.source = ComponentType::CACHE_L3; // Set the source to CACHE_L3

                return temporary_result; // Return the result

            }
            else
            {
                // Cache miss in L3, read from RAM
                temporary_result = bus.getMemory().template readGeneric<T>(address); // Read from RAM

                // Check if the read was successful
                if(temporary_result.success)
                {
                    //load the data into L3 cache

                    // Find a free line in L3 cache
                    uint64_t freePosition = L3Cache.findFreeLineIndex(L3Cache.getSets()[l3SetIndex]); 

                    // Load the data into L3 cache
                    L3Cache.load(l3SetIndex, l3Tag, temporary_result.data, freePosition); // Load the data into L3 cache

                    //load the data into L2 cache

                    // Find a free line in L2 cache
                    freePosition = L2Cache.findFreeLineIndex(L2Cache.getSets()[l2SetIndex]); 

                    // Load the data into L2 cache
                    L2Cache.load(l2SetIndex, l2Tag, temporary_result.data, freePosition); // Load the data into L2 cache

                    //load the data into L1 cache

                    // Find a free line in L1 cache
                    freePosition = L1Cache.findFreeLineIndex(L1Cache.getSets()[l1SetIndex]); 

                    // Load the data into L1 cache
                    L1Cache.load(l1SetIndex, l1Tag, temporary_result.data, freePosition); // Load the data into L1 cache

                    return temporary_result; // Return the result

                }
                else
                {
                    // Cache miss in RAM, return error
                    temporary_result.success = false; // Set success to false

                    // Set the event type to RAM_ACCESS
                    temporary_result.errorInfo.event = EventType::RAM_ACCESS; // Set the event type to RAM_ACCESS
                    temporary_result.errorInfo.source = ComponentType::RAM; // Set the source to RAM
                    temporary_result.errorInfo.message = "RAM access failed at address: " + std::to_string(address); // Set the message for debugging
                    temporary_result.errorInfo.error = ErrorType::READ_FAIL; // Set the error type to READ_FAIL

                    return temporary_result; // Return the result

                }

                
            }

        }
    }  
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


    // Try to write to L1 cache
    Result<T> temporary_result = L1Cache.write(address, data); // Write to L1 cache

    if(temporary_result.errorInfo.error == ErrorType::WRITE_FAIL)
    {
        return From_T_toVoid(temporary_result); // Return the result if there was an error in the write operation
    }

    if (temporary_result.success) // Check if the write was successful
    {
        //changing the event type from generic cache to l1 cache
        temporary_result.errorInfo.source = ComponentType::CACHE_L1; // Set the source to CACHE_L1
        
        return From_T_toVoid(temporary_result); // Return the result
    }
    else
    {
        // Cache miss in L1, try L2 cache
        temporary_result = L2Cache.write(address, data); // Write to L2 cache

        if(temporary_result.errorInfo.error == ErrorType::WRITE_FAIL)
        {
            return From_T_toVoid(temporary_result); // Return the result if there was an error in the write operation
        }

        // Check if the write was successful
        if(temporary_result.success)
        {
            //load the data into L1 cache

            // Find a free line in L1 cache
            uint64_t freePosition = L1Cache.findFreeLineIndex(L1Cache.getSets()[l1SetIndex]); 

            // Load the data into L1 cache
            L1Cache.load(l1SetIndex, l1Tag, temporary_result.data, freePosition); // Load the data into L1 cache

            //changing the event type from generic cache to l2 cache
            temporary_result.errorInfo.source = ComponentType::CACHE_L2; // Set the source to CACHE_L2

            return From_T_toVoid(temporary_result); // Return the result
        }
        else
        {
            // Cache miss in L2, try L3 cache
            temporary_result = L3Cache.write(address, data); // Write to L3 cache

            if(temporary_result.errorInfo.error == ErrorType::WRITE_FAIL)
            {
                return From_T_toVoid(temporary_result); // Return the result if there was an error in the write operation
            }

            // Check if the write was successful
            if(temporary_result.success)
            {
                //load the data into L2 cache

                // Find a free line in L2 cache
                uint64_t freePosition = L2Cache.findFreeLineIndex(L2Cache.getSets()[l2SetIndex]); 

                // Load the data into L2 cache
                L2Cache.load(l2SetIndex, l2Tag, temporary_result.data, freePosition); // Load the data into L2 cache

                //load the data into L1 cache

                // Find a free line in L1 cache
                freePosition = L1Cache.findFreeLineIndex(L1Cache.getSets()[l1SetIndex]); 

                // Load the data into L1 cache
                L1Cache.load(l1SetIndex, l1Tag, temporary_result.data, freePosition); // Load the data into L1 cache

                //changing the event type from generic cache to l3 cache
                temporary_result.errorInfo.source = ComponentType::CACHE_L3; // Set the source to CACHE_L3

                return From_T_toVoid(temporary_result); // Return the result

            }
            else
            {
                // Cache miss in L3, write to RAM
                temporary_result = bus.getMemory().writeGeneric(address, data); // Write to RAM

                // Check if the write was successful
                if(temporary_result.success)
                {
                    //load the data into L3 cache

                    // Find a free line in L3 cache
                    uint64_t freePosition = L3Cache.findFreeLineIndex(L3Cache.getSets()[l3SetIndex]); 

                    // Load the data into L3 cache
                    L3Cache.load(l3SetIndex, l3Tag, temporary_result.data, freePosition); // Load the data into L3 cache

                    //load the data into L2 cache

                    // Find a free line in L2 cache
                    freePosition = L2Cache.findFreeLineIndex(L2Cache.getSets()[l2SetIndex]); 

                    // Load the data into L2
                    L2Cache.load(l2SetIndex, l2Tag, temporary_result.data, freePosition); // Load the data into L2 cache

                    //load the data into L1 cache

                    // Find a free line in L1 cache
                    freePosition = L1Cache.findFreeLineIndex(L1Cache.getSets()[l1SetIndex]);

                    // Load the data into L1 cache
                    L1Cache.load(l1SetIndex, l1Tag, temporary_result.data, freePosition); // Load the data into L1 cache

                    return From_T_toVoid(temporary_result); // Return the result
                }
                else
                {
                    // Cache miss in RAM, return error
                    temporary_result.success = false; // Set success to false

                    // Set the event type to RAM_ACCESS
                    temporary_result.errorInfo.event = EventType::RAM_ACCESS; // Set the event type to RAM_ACCESS
                    temporary_result.errorInfo.source = ComponentType::RAM; // Set the source to RAM
                    temporary_result.errorInfo.message = "RAM access failed at address: " + std::to_string(address); // Set the message for debugging
                    temporary_result.errorInfo.error = ErrorType::WRITE_FAIL; // Set the error type to WRITE_FAIL

                    return From_T_toVoid(temporary_result); // Return the result

                }
            }
        }
    }
}
   
#endif //CACHEMANAGER_HPP