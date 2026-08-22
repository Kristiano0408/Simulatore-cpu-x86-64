#include "cache/cacheStorage.hpp"


CacheStorage::CacheStorage(uint32_t numSets, uint8_t associativity)
{
    sets.resize(numSets); // Resize the vector of cache sets based on the number of sets

    for (uint32_t i = 0; i < numSets; ++i)
    {
        sets[i].setIndex = i; // Initialize the set index for each cache set
        sets[i].lines.resize(associativity); // Resize the vector of cache lines in each set based on the associativity

        for (uint8_t j = 0; j < associativity; ++j)
        {
            sets[i].lines[j].valid = false; // Initialize all lines as invalid
            sets[i].lines[j].dirty = false; // Initialize all lines as not dirty
            sets[i].lines[j].tag = 0;
            sets[i].lines[j].lastAccessTime = 0; // Initialize last access time to 0
        }
    }
}

CacheLine* CacheStorage::findLine(uint32_t setIndex, uint64_t tag)
{
    CacheSet& set = sets[setIndex]; // Get the cache set based on the set index

    for (CacheLine& line : set.lines) // Loop through the lines in the set
    {
        if (line.valid && line.tag == tag) // Check if the line is valid and the tag matches
        {
            return &line; // Return a pointer to the matching cache line
        }
    }

    return nullptr; // Return nullptr if no matching line is found
}

int8_t CacheStorage::findLineIndex(uint32_t setIndex, uint64_t tag)
{
    CacheSet& set = sets[setIndex];
    for (size_t i = 0; i < set.lines.size(); ++i)
        if (set.lines[i].valid && set.lines[i].tag == tag)
            return static_cast<int8_t>(i);

    return -1; // miss
}

void CacheStorage::invalidateLine(uint32_t setIndex, uint8_t lineIndex)
{
    CacheSet& set = sets[setIndex]; // Get the cache set based on the set index
    CacheLine& line = set.lines[lineIndex]; // Get the cache line based on the line index

    if (line.valid) // Check if the line is valid before invalidating
    {
        line.valid = false; // Invalidate the line
        line.dirty = false; // Mark the line as not dirty
    }
}

void CacheStorage::invalidateAllLines()
{
    for (uint32_t i = 0; i < sets.size(); ++i) // Loop through all cache sets
    {
        for (uint8_t j = 0; j < uint8_t(sets[i].lines.size()); ++j) // Loop through all lines in the cache set
        {
            invalidateLine(i, j); // Invalidate each line in the cache set
        }
    }
}


void CacheStorage::invalidateLineByAddress(AddressInfo addressInfo)
{
    int8_t lineIndex = findLineIndex(addressInfo.setIndex, addressInfo.tag); // Find the line index based on the set index and tag from the address information

    if (lineIndex < 0) [[unlikely]] // If no matching line is found, return without invalidating
        return;

    invalidateLine(addressInfo.setIndex, (uint8_t)lineIndex); // Invalidate the specific cache line based on the set index and line index
}


void CacheStorage::loadLine(uint32_t setIndex, const CacheLine& line, uint8_t lineIndex)
{
    CacheSet& set = sets[setIndex]; // Get the cache set based on the set index
    CacheLine& targetLine = set.lines[lineIndex]; // Get the target cache line based on the line index

    targetLine.data = line.data; // Load the data into the target cache line
    targetLine.tag = line.tag; // Set the tag for the target cache line
    targetLine.valid = true; // Mark the target cache line as valid
    targetLine.dirty = false; // Mark the target cache line as not dirty since it's being loaded with new data

}
