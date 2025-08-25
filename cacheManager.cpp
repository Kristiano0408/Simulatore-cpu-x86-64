#include "cacheManager.hpp"
#include "bus.hpp"
#include "memory.hpp"

#include <iostream>


CacheLevel::CacheLevel(uint64_t size, uint64_t associativity, Bus& bus)
    : cacheSize(size), associativity(associativity), numSets(size / (associativity * CACHE_LINE_SIZE)), bus(bus)
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


void CacheLevel::load(uint64_t setIndex, uint64_t tag, const std::array<uint8_t, CACHE_LINE_SIZE>& data, uint64_t freePosition)
{
    // Load data into the cache line at the specified set index and tag
    CacheSet& set = sets[setIndex]; // Get the cache set

    CacheLine& line = set.lines[freePosition]; // Get the cache line at the free position

    line.valid = true; // Mark the line as valid
    line.dirty = false; // Mark the line as not dirty
    line.tag = tag; // Set the tag
    line.data = data; // Load the data into the cache line
    line.lastAccessTime = bus.getClock().getCycles(); // Update the last access time

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
        //check if the line is dirty
        if (line.dirty)
        {
            bus.getMemory().writeGeneric( (line.tag * numSets + setIndex) * CACHE_LINE_SIZE, line.data);
        }

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

void CacheLevel::invalidateAll()
{
    // Loop through all sets and lines to invalidate the cache
    for (CacheSet& set : sets) // Loop through the cache sets
    {
        for (CacheLine& line : set.lines) // Loop through the lines in the set
        {
            if (line.dirty) // Check if the line is dirty
            {
                // Write back the dirty line
                bus.getMemory().writeGeneric( (line.tag * numSets + set.setIndex) * CACHE_LINE_SIZE, line.data);
            }

            line.valid = false; // Invalidate the line
            line.dirty = false; // Mark the line as not dirty
        }
    }

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
                bus.getMemory().writeGeneric( (line.tag * numSets + set.setIndex) * CACHE_LINE_SIZE, line.data);
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


uint64_t CacheLevel::findFreeLineIndex(CacheSet& set)
{
    for (uint64_t i = 0; i < set.lines.size(); ++i)
    {
        if (!set.lines[i].valid)
            return i;
    }

    // Se nessuna linea è libera, usa la politica di rimpiazzamento
    return manageReplacementPolicy(set);
}

uint64_t CacheLevel::manageReplacementPolicy(CacheSet& set)
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

    //control for dirty lines
    if (set.lines[index].dirty)
    {
        // Write back the dirty line
        bus.getMemory().writeGeneric( (set.lines[index].tag * numSets + set.setIndex) * CACHE_LINE_SIZE, set.lines[index].data);

        set.lines[index].dirty = false; // Mark the line as not dirty after writing back


    }

    return index; // Return the index of the line to be replaced
}


// CacheManager class implementation
CacheManager::CacheManager(Bus& bus, uint64_t l1Size, uint64_t l1Associativity, uint64_t l2Size, uint64_t l2Associativity, uint64_t l3Size, uint64_t l3Associativity) 
                         :L1Cache(l1Size, l1Associativity, bus), L2Cache(l2Size, l2Associativity, bus), L3Cache(l3Size, l3Associativity, bus), bus(bus)
{
    //nothing to do here

}

CacheManager::~CacheManager()
{
    // No dynamic memory to free, but can be used for cleanup if needed
}



void CacheManager::flushAllCaches()
{
    L1Cache.flush(); // Flush L1 cache
    L2Cache.flush(); // Flush L2 cache
    L3Cache.flush(); // Flush L3 cache
}

void CacheManager::invalidateAllCaches()
{
    L1Cache.invalidateAll(); // Invalidate all lines in L1 cache
    L2Cache.invalidateAll(); // Invalidate all lines in L2 cache
    L3Cache.invalidateAll(); // Invalidate all lines in L3 cache
}

void CacheManager::printCacheState() const
{
    std::cout << "Cache Manager State:" << std::endl; // Print the cache manager state
    L1Cache.printCacheState(); // Print L1 cache state
    L2Cache.printCacheState(); // Print L2 cache state
    L3Cache.printCacheState(); // Print L3 cache state
}

