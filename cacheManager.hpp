#ifndef CACHEMANAGER_HPP
#define CACHEMANAGER_HPP
#include <cstdint>
#include <vector>
#include <array>
#include "helpers.hpp"

class Memory; // Forward declaration of Memory class

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
        uint64_t numSets;
        uint64_t associativity;
        uint64_t cacheSize;

        //reference to cachemanager for the flush and invalidate operations
        CacheManager& cacheManager; // Pointer to the cache manager for flush and invalidate operations

    public:
        CacheLevel(uint64_t size, uint64_t associativity);
        ~CacheLevel();
        template <typename T>
        Result<T> read(uint64_t address);
        template <typename T>
        Result<void> write(uint64_t address, const T& data); // Write data to the cache
        void load(uint64_t setIndex, uint64_t tag, const std::array<uint8_t, CACHE_LINE_SIZE>& data, uint64_t freePosition);


        // Function to find a cache line in a set
        CacheLine* findLine(const CacheSet& set, uint64_t tag);

        uint64_t manageReplacementPolicy(const CacheSet& set);
        uint64_t findFreeLineIndex(const CacheSet& set);

        void invalidate(uint64_t address);
        void invalidateAll(); // Invalidate all lines in the cache
        void flush();
        void printCacheState() const; // For debugging purposes

        //getters and setters for the cache size and associativity
        uint64_t getCacheSize() const { return cacheSize; }
        uint64_t getAssociativity() const { return associativity; }
        uint64_t getNumSets() const { return numSets; }

        void setCacheSize(uint64_t size) { cacheSize = size; }
        void setAssociativity(uint64_t assoc) { associativity = assoc; }
        void setNumSets(uint64_t sets) { numSets = sets; };

  


};


// Cache manager class to manage multiple cache levels
class CacheManager
{   
    public:
        CacheManager(Memory* mem,uint64_t l1Size, uint64_t l2Size, uint64_t l3Size, uint64_t l1Assoc, uint64_t l2Assoc, uint64_t l3Assoc);
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
        Memory* ram; // Pointer to the main memory
};

#endif //CACHEMANAGER_HPP