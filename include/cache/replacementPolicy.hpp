#ifndef REPLACEMENTPOLICY_HPP
#define REPLACEMENTPOLICY_HPP


#include <cstdint>
#include <random>
#include "cacheStorage.hpp"


class ReplacementPolicy
{
    public:
        virtual uint8_t selectLineToReplace(CacheSet& set) = 0; // Pure virtual function to select a line to replace based on the replacement policy
        virtual void updateOnAccess(CacheSet& set, uint8_t lineIndex) = 0; // Pure virtual function to update the replacement policy state on cache access
        virtual void onLineLoaded(CacheSet& set, uint8_t lineIndex) = 0; // Pure virtual function to update the replacement policy state when a line is loaded into the cache
        virtual void initializeSet(uint8_t associativity) = 0; // Pure virtual function to initialize the replacement policy state for a cache set
        virtual ~ReplacementPolicy() = default; // Virtual destructor for proper cleanup of derived classes
};

class NULLPolicy : public ReplacementPolicy
{
    public:

    void initializeSet([[maybe_unused]] uint8_t associativity) override {}; // Function to initialize the LRU state for a cache set
    uint8_t selectLineToReplace([[maybe_unused]] CacheSet& set) override {return 0;}; // Override of the function to select a line to replace based on LRU policy
    void updateOnAccess([[maybe_unused]] CacheSet& set, [[maybe_unused]] uint8_t lineIndex) override {}; // Override of the function to update the LRU state on cache access
    void onLineLoaded([[maybe_unused]] CacheSet& set,[[maybe_unused]] uint8_t lineIndex) override {}; // Override of the function to update the LRU state when a line is loaded into the cache
};

struct LRUState
{
    ReplacementPolicyQueueCacheFriendly<uint8_t> queue; // Queue to maintain the order of line indices based on recency of access (least recently accessed at the front)
};

//LRU replacement policy implementation
class LRUReplacementPolicy : public ReplacementPolicy
{

    private:
        std::vector<LRUState> lruStates; // Queue to maintain the order of line indices based on recency of access (least recently accessed at the front)
        //every lruState is associated with a set, and the queue maintains the order of sets based on recency of access (least recently accessed set at the front)
    public:
        LRUReplacementPolicy(const uint32_t& numSets, const uint8_t& associativity); // Constructor to initialize the LRU state for all cache sets based on the number of sets and associativity
        void initializeSet(uint8_t associativity) override; // Function to initialize the LRU state for a cache set
        uint8_t selectLineToReplace(CacheSet& set) override; // Override of the function to select a line to replace based on LRU policy
        void updateOnAccess(CacheSet& set, uint8_t lineIndex) override; // Override of the function to update the LRU state on cache access
        void onLineLoaded(CacheSet& set, uint8_t lineIndex) override; // Override of the function to update the LRU state when a line is loaded into the cache
};


struct PLRUTree
{
    std::vector<bool> bits; // Vector to hold the bits of the PLRU tree, where each bit indicates the direction to take for replacement (0 for left, 1 for right)
    PLRUTree(uint8_t associativity) : bits(associativity - 1, false) {} // Constructor to initialize the PLRU tree based on the associativity of the cache set
    uint8_t selectLineToReplace(); // Function to select a line to replace based on the PLRU tree
    void updateLine(uint8_t lineIndex); // Function to update the PLRU tree on cache access to reflect the most recently used line

};
//PLRU replacement policy implementation    
class PLRUReplacementPolicy : public ReplacementPolicy
{
    private:
        std::vector<PLRUTree> plruTrees; // Vector to hold the PLRU trees for each cache set, where each tree maintains the replacement state for its corresponding set
    public:
        PLRUReplacementPolicy(const uint32_t& numSets, const uint8_t& associativity); // Constructor to initialize the PLRU state for all cache sets based on the number of sets and associativity
        void initializeSet(uint8_t associativity) override; // Function to initialize the PLRU state for a cache set
        uint8_t selectLineToReplace(CacheSet& set) override; // Override of the function to select a line to replace based on PLRU policy
        void updateOnAccess(CacheSet& set, uint8_t lineIndex) override; // Override of the function to update the PLRU state on cache access
        void onLineLoaded(CacheSet& set, uint8_t lineIndex) override; // Override of the function to update the PLRU state when a line is loaded into the cache

};


//random replacement policy implementation
class RandomReplacementPolicy : public ReplacementPolicy
{
    private:
        std::random_device rd; // Random device to seed the random number generator
        std::mt19937 gen;
    public:
        RandomReplacementPolicy(){gen.seed(rd());}
        uint8_t selectLineToReplace(CacheSet& set) override; // Override of the function to select a line to replace based on random policy
        void updateOnAccess([[maybe_unused]] CacheSet& set, [[maybe_unused]] uint8_t lineIndex) override {} // No state to update on access for random replacement policy
        void onLineLoaded([[maybe_unused]] CacheSet& set, [[maybe_unused]] uint8_t lineIndex) override {} // No state to update when a line is loaded for random replacement policy
};











#endif //REPLACEMENTPOLICY_HPP