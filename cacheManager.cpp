#include "cacheManager.hpp"
#include <iostream>



CacheLevel::CacheLevel(uint64_t size, uint64_t associativity)
    : cacheSize(size), associativity(associativity), numSets(size / (associativity * CACHE_LINE_SIZE))
{
    //resizing the vector of sets
    sets.resize(numSets);

    for (uint64_t i = 0; i < numSets; ++i)
    {
        sets[i].lines.resize(associativity);
        sets[i].setIndex = i;
        for (uint64_t j = 0; j < associativity; ++j)
        {
            sets[i].lines[j].valid = false; // Initialize all lines as invalid
            sets[i].lines[j].dirty = false; // Initialize all lines as not dirty
            sets[i].lines[j].tag = 0;
            sets[i].lines[j].lastAccessTime = 0; // Initialize last access time to 0
        }
    }
}

CacheLevel::~CacheLevel()
{
    // No dynamic memory to free, but can be used for cleanup if needed

}


Result<uint64_t> CacheLevel::read(uint64_t address)
{
    //craetion of the structure for the result
    Result<uint64_t> result;


    uint64_t setIndex = (address / CACHE_LINE_SIZE) % numSets; // Calculate the set index
    uint64_t tag = address / (CACHE_LINE_SIZE * numSets); // Calculate the tag


    CacheSet& set = sets[setIndex]; // Get the cache set

    auto* line = findLine(set, tag, setIndex); // Check if the line is in the cache

    if(line != nullptr)
    {
        // Cache hit

        // Calculate the offset within the cache line
        uint64_t offset = address % CACHE_LINE_SIZE; 

        // Read the data from the cache line
        result.data = line->data[offset]; 

        line->lastAccessTime++;

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





void CacheLevel::load(uint64_t setIndex, uint64_t tag, const std::array<uint8_t, CACHE_LINE_SIZE>& data, uint64_t freePosition)
{
    // Load data into the cache line at the specified set index and tag
    CacheSet& set = sets[setIndex]; // Get the cache set

    CacheLine& line = set.lines[freePosition]; // Get the cache line at the free position

    line.valid = true; // Mark the line as valid
    line.dirty = false; // Mark the line as not dirty
    line.tag = tag; // Set the tag
    line.data = data; // Load the data into the cache line
    line.lastAccessTime++; // Increment the last access time

    std::cout << "Loaded data into cache at set index: " << setIndex << ", tag: " << tag << std::endl; // Print the load message



}



CacheLine* CacheLevel::findLine(const CacheSet& set , uint64_t tag, uint64_t setIndex)
{
    // Loop through the lines in the set to find a matching tag
    for ( const CacheLine& line : set.lines)
    {
        if (line.valid && line.tag == tag) // Check if the line is valid and the tag matches
        {
            return const_cast<CacheLine*>(&line); // Return the matching line
        }
    }

    return nullptr; // Return nullptr if no matching line is found
}