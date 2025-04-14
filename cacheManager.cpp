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

    auto* line = findLine(set, tag); // Check if the line is in the cache

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

Result<void> CacheLevel::write(uint64_t address, const std::array<uint8_t, CACHE_LINE_SIZE>& data)
{
    // Create a result structure for the write operation
    Result<void> result;

    uint64_t setIndex = (address / CACHE_LINE_SIZE) % numSets; // Calculate the set index
    uint64_t tag = address / (CACHE_LINE_SIZE * numSets); // Calculate the tag

    CacheSet& set = sets[setIndex]; // Get the cache set

    auto* line = findLine(set, tag); // Check if the line is in the cache

    if(line != nullptr)
    {
        // Cache hit
        line->data = data; // Write data to the cache line
        line->dirty = true; // Mark the line as dirty
        line->lastAccessTime++; // Increment the last access time

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

CacheLine* CacheLevel::findLine(const CacheSet& set , uint64_t tag)
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

void CacheLevel::invalidate(uint64_t address)
{
    uint64_t setIndex = (address / CACHE_LINE_SIZE) % numSets; // Calculate the set index
    uint64_t tag = address / (CACHE_LINE_SIZE * numSets); // Calculate the tag

    CacheSet& set = sets[setIndex]; // Get the cache set

    for (CacheLine& line : set.lines) // Loop through the lines in the set
    {
        if (line.valid && line.tag == tag) // Check if the line is valid and the tag matches
        {
            line.valid = false; // Invalidate the line
            line.dirty = false; // Mark the line as not dirty
            std::cout << "Invalidated cache line at set index: " << setIndex << ", tag: " << tag << std::endl; // Print the invalidate message
            return; // Exit the function
        }
    }

    std::cout << "No matching cache line found to invalidate at address: " << std::hex << address << std::endl; // Print message if no matching line is found

}


void CacheLevel::flush()
{
    // Loop through all sets and lines to flush the cache
    for (CacheSet& set : sets) // Loop through the cache sets
    {
        for (CacheLine& line : set.lines) // Loop through the lines in the set
        {
            if (line.dirty) // Check if the line is dirty
            {
                // Write back the data to memory (not implemented here)
                line.dirty = false; // Mark the line as not dirty after flushing
            }
            line.valid = false; // Invalidate the line
        }
    }

    std::cout << "Cache flushed" << std::endl; // Print the flush message

}

void CacheLevel::printCacheState() const
{
    // Print the state of the cache for debugging purposes
    std::cout << "Cache State:" << std::endl;
    for (const CacheSet& set : sets) // Loop through the cache sets
    {
        std::cout << "Set Index: " << set.setIndex << std::endl; // Print the set index
        for (const CacheLine& line : set.lines) // Loop through the lines in the set
        {
            std::cout << "  Line Tag: " << line.tag << ", Valid: " << line.valid << ", Dirty: " << line.dirty << std::endl; // Print the line state
        }
    }
}


uint64_t CacheLevel::findFreeLineIndex(const CacheSet& set)
{
    for (uint64_t i = 0; i < set.lines.size(); ++i)
    {
        if (!set.lines[i].valid)
            return i;
    }

    // Se nessuna linea è libera, usa la politica di rimpiazzamento
    return manageReplacementPolicy(set);
}

uint64_t CacheLevel::manageReplacementPolicy(const CacheSet& set)
{
    uint64_t index = 0; // Initialize the index to 0

    uint64_t oldestTime = set.lines[0].lastAccessTime; // Initialize the oldest time to the first line's last access time

    for (uint64_t i = 1; i < set.lines.size(); ++i) // Loop through the lines in the set
    {
        if (set.lines[i].lastAccessTime < oldestTime) // Check if the current line's last access time is older
        {
            oldestTime = set.lines[i].lastAccessTime; // Update the oldest time
            index = i; // Update the index to the current line
        }
    }

    return index; // Return the index of the line to be replaced
}

// CacheManager class implementation
CacheManager::CacheManager(uint64_t l1Size, uint64_t l1Associativity, uint64_t l2Size, uint64_t l2Associativity, uint64_t l3Size, uint64_t l3Associativity) 
                         :L1Cache(l1Size, l1Associativity), L2Cache(l2Size, l2Associativity), L3Cache(l3Size, l3Associativity)
{
    //nothing to do here

}

CacheManager::~CacheManager()
{
    // No dynamic memory to free, but can be used for cleanup if needed
}

