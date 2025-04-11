#ifndef CACHEMANAGER_HPP
#define CACHEMANAGER_HPP
#include <cstdint>
#include <vector>
#include <array>

const uint64_t CACHE_LINE_SIZE = 64; // Size of a cache line in bytes

struct CacheLine
{
    bool valid;
    uint64_t tag;
    std::array<uint8_t, CACHE_LINE_SIZE> data;  // Data stored in the cache line
    uint64_t lastAccessTime; // For LRU replacement policy


};


struct CacheSet
{
    std::vector<CacheLine> lines; // Lines in the cache set
    uint64_t setIndex;
};

class L1Cache
{
    private:
        std::vector<CacheSet> sets; // Cache sets
        uint64_t numSets;
        uint64_t associativity;
        uint64_t cacheSize;

    public:
        L1Cache(uint64_t size, uint64_t associativity, uint64_t blockSize);
        ~L1Cache();
        bool read(uint64_t address, std::vector<uint8_t>& data);
        bool write(uint64_t address, const std::vector<uint8_t>& data);
        void load(uint64_t address, const std::vector<uint8_t>& data);
        bool findLine(uint64_t address, CacheLine& line);
        void manageReplacementPolicy(CacheSet& set, CacheLine& line);
        void invalidate(uint64_t address);
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


#endif //CACHEMANAGER_HPP