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


