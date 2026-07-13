#ifndef CACHESTORAGE_HPP
#define CACHESTORAGE_HPP


#include <cstdint>
#include <vector>
#include "helpers.hpp"
#include "device.hpp"

class CacheStorage : public FaultDevice
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

//implicit template, it must be defined in the header file, otherwise it will not be found by the linker
void CacheStorage::flush(auto&& memoryWriteFunction)
{
    int i = 0;
    for (CacheSet& set : sets) // Loop through all cache sets
    {

        for (CacheLine& line : set.lines) // Loop through all lines in the cache set
        {
            if (line.valid && line.dirty) // Check if the line is valid and dirty before flushing
            {
                uint8_t offsetBits = std::countr_zero(CACHE_LINE_SIZE); // Calculate the number of bits for the offset based on the cache line size
                uint8_t setIndexBits = std::countr_zero(sets.size()); // Calculate the number of bits for the set index based on the number of cache sets
                uint64_t address = (line.tag << (offsetBits + setIndexBits)) | (i<< offsetBits);
                memoryWriteFunction(address, line.data);
                // Example: memory.write(line.tag * CACHE_LINE_SIZE, line.data);
                line.dirty = false; // Mark the line as not dirty after flushing
            }
        }
        i++;
    }
}






#endif //CACHESTORAGE_HPP