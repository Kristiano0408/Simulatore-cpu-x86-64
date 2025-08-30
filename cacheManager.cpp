#include "cacheManager.hpp"
#include "bus.hpp"
#include "memory.hpp"

#include <iostream>


CacheLevel::CacheLevel(uint64_t size, uint64_t associativity, Bus& bus, CacheLevel* nextLevel)
    : cacheSize(size), associativity(associativity), numSets(size / (associativity * CACHE_LINE_SIZE)), bus(bus), nextLevel(nextLevel)
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
        if(nextLevel != nullptr)
        {
            // Write back the dirty line to the next cache level
            nextLevel->write( (set.lines[index].tag * numSets + set.setIndex) * CACHE_LINE_SIZE, set.lines[index].data);
            set.lines[index].dirty = false; // Mark the line as not dirty after writing back
        }
        else
        {
            // Write back the dirty line to main memory
            bus.getMemory().writeGeneric( (set.lines[index].tag * numSets + set.setIndex) * CACHE_LINE_SIZE, set.lines[index].data);
            set.lines[index].dirty = false; // Mark the line as not dirty after writing back
        }
        set.lines[index].dirty = false; // Mark the line as not dirty after writing back


    }

    return index; // Return the index of the line to be replaced
}

Result<std::array<uint8_t, CACHE_LINE_SIZE>> CacheLevel::read(uint64_t address)
{
    //craetion of the structure for the result
    Result<std::array<uint8_t, CACHE_LINE_SIZE>> result;

    std::cout << "Reading from cache at address: " << std::hex << address << std::endl;
    //bitwise index/tag calculation
    constexpr unsigned offsetBits = ilog2_constexpr(CACHE_LINE_SIZE); // Calculate the number of bits for the offset

    std::cout << "Offset bits: " << offsetBits << std::endl;

    unsigned indexBits = ilog2(numSets); // Calculate the number of bits for the index

    std::cout << "Index bits: " << indexBits << std::endl;


    uint64_t offset = address & ((1ULL << offsetBits) - 1); // Calculate the offset within the cache line

    std::cout << "Offset: " << offset << std::endl;
    //manage the offset for the read operation
    if (offset_cache(EventType::CACHE_READ_ERROR, ErrorType::READ_FAIL, result, offset, address)) 
    {
        std::cout << "Cache read error at address: " << std::hex << address << std::endl;
        return result;
    }

    std::cout << "Offset after check: " << offset << std::endl;

    uint64_t setIndex = (address >> offsetBits) & ((1ULL << indexBits) - 1); // Calculate the set index
    std::cout << "Set index: " << setIndex << std::endl;
    uint64_t tag = address >> (offsetBits + indexBits); // Calculate the tag
    std::cout << "Tag: " << tag << std::endl;

    CacheSet& set = sets[setIndex]; // Get the cache set
    auto* line = findLine(set, tag); // Check if the line is in the cache

    if(line != nullptr)
    {
        // Cache hit

        // Read the data from the cache line
        std::memcpy(&result.data, &line->data, sizeof(std::array<uint8_t, CACHE_LINE_SIZE>)); // Copy the data from the cache line to the result

        line->lastAccessTime = bus.getClock().getCycles(); // Update the last access time

        // Set success to true
        result.success = true; 

        // Set the event type to CACHE_HIT
        result.errorInfo.event = EventType::CACHE_HIT; // Set the event type to CACHE_HIT
        result.errorInfo.source = ComponentType::CACHE; // Set the source to CACHE
        result.errorInfo.message = "Cache hit at address: " + std::to_string(address); // Set the message for debugging
        result.errorInfo.error = ErrorType::NONE; // Set the error type to NONE

        std::cout << "Cache hit at address: " << std::hex << address << std::endl; // Print the cache hit message
        std::cout << result << std::endl; // Print the result

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

template<>
bool offset_cache(EventType event, ErrorType error, Result<void>& result, uint64_t offset, uint64_t address)
{
    if (offset > CACHE_LINE_SIZE)
    {
        result.success = false;
        result.errorInfo.event = event;
        result.errorInfo.source = ComponentType::CACHE;
        result.errorInfo.message = "Write exceeds cache line boundary at address: " + std::to_string(address);
        result.errorInfo.error = error;
        return true; // Indicate that there was an error
    }
    return false; // No error
}

template<>
bool offset_cache(EventType event, ErrorType error, Result<std::array<uint8_t, CACHE_LINE_SIZE>>& result, uint64_t offset, uint64_t address)
{
    if (offset > CACHE_LINE_SIZE)
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

// CacheManager class implementation
CacheManager::CacheManager(Bus& bus, uint64_t l1Size, uint64_t l2Size, uint64_t l3Size,uint64_t l1Associativity, uint64_t l2Associativity, uint64_t l3Associativity) 
                         :L1Cache(l1Size, l1Associativity, bus, &L2Cache), L2Cache(l2Size, l2Associativity, bus, &L3Cache), L3Cache(l3Size, l3Associativity, bus, nullptr), bus(bus)
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


Result<std::array<uint8_t, CACHE_LINE_SIZE>> CacheManager::readSingleLine(uint64_t address, uint64_t l1SetIndex, uint64_t l1Tag, uint64_t l2SetIndex, uint64_t l2Tag, uint64_t l3SetIndex, uint64_t l3Tag, uint64_t offset)
{
    //temporary result that holds the line read from cache
    Result<std::array<uint8_t, CACHE_LINE_SIZE>> temporary_result;

    //trying to read from L1 cache
    std::cout << "Trying to read line from L1 cache at address: " << std::hex << address << std::endl;
    temporary_result = L1Cache.read(address); // Read from L1 cache


     if(temporary_result.errorInfo.error == ErrorType::READ_FAIL)
    {
        return temporary_result;
    }

    if (temporary_result.success) // Check if the read was successful
    {
        //changing the event type from generic cache to l1 cache
        temporary_result.errorInfo.source = ComponentType::CACHE_L1; // Set the source to CACHE_L1

        return temporary_result; // Return the result
    }
    else
    {
        std::cout << "Cache miss in L1, trying L2 cache at address: " << std::hex << address << std::endl;
        // Cache miss in L1, try L2 cache
        temporary_result = L2Cache.read(address); // Read from L2 cache

        if(temporary_result.errorInfo.error == ErrorType::READ_FAIL)
        {
            return temporary_result;
        }

        // Check if the read was successful
        if(temporary_result.success)
        {
            //changing the event type from generic cache to l2 cache
            temporary_result.errorInfo.source = ComponentType::CACHE_L2; // Set the source to CACHE_L2

            //load the data into L1 cache

            // Find a free line in L1 cache
            uint64_t freePosition = L1Cache.findFreeLineIndex(L1Cache.getSets()[l1SetIndex]); 

            // Load the data into L1 cache
            L1Cache.load(l1SetIndex, l1Tag, temporary_result.data, freePosition); // Load the data into L1 cache

            return temporary_result; // Return the result

        }
        else
        {
            // Cache miss in L2, try L3 cache
            std::cout << "Cache miss in L2, trying L3 cache at address: " << std::hex << address << std::endl;
            
            temporary_result = L3Cache.read(address); // Read from L3 cache

            if(temporary_result.errorInfo.error == ErrorType::READ_FAIL)
            {
                return temporary_result;
            }

            // Check if the read was successful
            if(temporary_result.success)
            {
                //changing the event type from generic cache to l3 cache
                temporary_result.errorInfo.source = ComponentType::CACHE_L3; // Set the source to CACHE_L3

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

                return temporary_result; // Return the result

            }
            else
            {
                //calculating the start of the line
                uint64_t lineStart = address - offset;

                std::cout << "Cache miss in L3, reading from RAM at address: " << std::hex << lineStart << std::endl; // Print the cache miss message

                // Cache miss in L3, read from RAM
                temporary_result = bus.getMemory().template readGeneric<std::array<uint8_t, CACHE_LINE_SIZE>>(lineStart); // Read from RAM

                std::cout << "Read from RAM at address: " << std::hex << lineStart << std::endl; // Print the RAM read message

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

Result<std::array<uint8_t, CACHE_LINE_SIZE * 2>> CacheManager::readCrossLines(uint64_t address)
{
    std::cout << "Reading cross lines at address: " << std::hex << address << std::endl;
    // Read data spanning two cache lines
    Result<std::array<uint8_t, CACHE_LINE_SIZE * 2>> result;

    uint64_t l1SetIndex = (address / CACHE_LINE_SIZE) % L1Cache.getNumSets(); // Calculate the L1 set index
    uint64_t l1Tag = address / (CACHE_LINE_SIZE * L1Cache.getNumSets()); // Calculate the L1 tag

    uint64_t l2SetIndex = (address / CACHE_LINE_SIZE) % L2Cache.getNumSets(); // Calculate the L2 set index
    uint64_t l2Tag = address / (CACHE_LINE_SIZE * L2Cache.getNumSets()); // Calculate the L2 tag

    uint64_t l3SetIndex = (address / CACHE_LINE_SIZE) % L3Cache.getNumSets(); // Calculate the L3 set index
    uint64_t l3Tag = address / (CACHE_LINE_SIZE * L3Cache.getNumSets()); // Calculate the L3 tag

    uint64_t offset = address % CACHE_LINE_SIZE; // Calculate the offset within the cache line

    // Read the first cache line
    auto result1 = readSingleLine(address, l1SetIndex, l1Tag, l2SetIndex, l2Tag, l3SetIndex, l3Tag, offset);
    if (!result1.success)
    {
        result.success = false;
        result.errorInfo = result1.errorInfo;
        return result;
    }

    std::cout << "First line read successfully at address: " << std::hex << address << std::endl;

    uint64_t new_address = address + (CACHE_LINE_SIZE - offset);
    l1SetIndex = (new_address / CACHE_LINE_SIZE) % L1Cache.getNumSets(); // Calculate the L1 set index
    l1Tag = new_address / (CACHE_LINE_SIZE * L1Cache.getNumSets()); // Calculate the L1 tag

    l2SetIndex = (new_address / CACHE_LINE_SIZE) % L2Cache.getNumSets(); // Calculate the L2 set index
    l2Tag = new_address / (CACHE_LINE_SIZE * L2Cache.getNumSets());

    l3SetIndex = (new_address / CACHE_LINE_SIZE) % L3Cache.getNumSets(); // Calculate the L3 set index
    l3Tag = new_address / (CACHE_LINE_SIZE * L3Cache.getNumSets());

    offset = new_address % CACHE_LINE_SIZE; // Calculate the offset within the cache line





    // Read the second cache line
    auto result2 = readSingleLine(new_address, l1SetIndex, l1Tag, l2SetIndex, l2Tag, l3SetIndex, l3Tag, offset);
    if (!result2.success)
    {
        result.success = false;
        result.errorInfo = result2.errorInfo;
        return result;
    }

    std::cout << "Successfully read cross lines at address: " << std::hex << address << std::endl;
    std::cout << "First line: "<< result1 << std::endl;
    std::cout << "Second line: "<< result2 << std::endl;

    // Copy the data from both lines into the result
    std::memcpy(result.data.data(), result1.data.data(), CACHE_LINE_SIZE);
    std::memcpy(result.data.data() + CACHE_LINE_SIZE, result2.data.data(), CACHE_LINE_SIZE);

    std::cout << "Combined data: "<< result << std::endl;

    result.success = true;
    return result;
}