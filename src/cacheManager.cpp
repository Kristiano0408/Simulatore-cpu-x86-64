#include "cacheManager.hpp"
#include "bus.hpp"
#include "memory.hpp"
#include <iostream>
#include "cpu.hpp"
#include <random>




LRUReplacementPolicy::LRUReplacementPolicy(const uint8_t& numSets, const uint8_t& associativity)
{
    for (uint8_t i = 0; i < numSets; ++i)
    {
        initializeSet(i, associativity); // Initialize the LRU state for each cache set based on the number of sets and associativity
    }
}

void LRUReplacementPolicy::initializeSet(uint8_t setIndex, uint8_t associativity)
{
    LRUState state;
    for (uint64_t i = 0; i < associativity; ++i)
    {
        state.lineIndices.push_back(i); // Initialize the list with line indices in order of recency (initially all are equally recent)
        state.indexMap[i] =uint8_t(state.lineIndices.size() - 1); // Map each line index to its position in the list for O(1) access
    }
    lruMap[setIndex] = state; // Store the LRU state for the cache set
}

uint8_t LRUReplacementPolicy::selectLineToReplace(CacheSet& set)
{
    LRUState& state = lruMap[set.setIndex]; // Get the LRU state for the cache set

    if (state.validLines < set.lines.size()) // If there are still invalid lines in the set
       return state.validLines++; // Return the next free line index and increment the count of valid lines

    

    return state.lineIndices.front(); // Get the least recently used line index (the front of the vector)

}

void LRUReplacementPolicy::updateOnAccess(CacheSet& set, uint8_t lineIndex)  
{
    LRUState& state = lruMap[set.setIndex]; // Get the LRU state for the cache set

    uint8_t lineIdxInVector = state.indexMap[lineIndex]; // Get the position of the accessed line index in the vector

    std::swap(state.lineIndices[lineIdxInVector], state.lineIndices.back()); // Swap the accessed line index with the last element in the vector (most recently used)
    state.lineIndices.pop_back(); // Remove the last element (the accessed line index) from its current position in the vector

    state.lineIndices.push_back(lineIndex); // Move the accessed line index to the back of the list (most recently used)

    state.indexMap[lineIndex] = uint8_t(state.lineIndices.size() - 1); // Update the index map with the new position of the accessed line index
}

void LRUReplacementPolicy::onLineLoaded(CacheSet& set, uint8_t lineIndex) 
{
    LRUState& state = lruMap[set.setIndex]; // Get the LRU state for the cache set

    uint8_t lineIdxInVector = state.indexMap[lineIndex]; // Get the position of the loaded line index in the vector

    std::swap(state.lineIndices[lineIdxInVector], state.lineIndices.back()); // Swap the loaded line index with the last element in the vector (most recently used)
    state.lineIndices.pop_back(); // Remove the last element (the loaded line index) from its current position in the vector

    state.lineIndices.push_back(lineIndex); // Move the loaded line index to the back of the list (most recently used)

    state.indexMap[lineIndex] = uint8_t(state.lineIndices.size() - 1); // Update the index map with the new position of the loaded line index

}


uint8_t PLRUTree::selectLineToReplace()
{
    uint8_t node = 0; // Start at the root of the PLRU tree

    while (node < bits.size()) // Traverse the tree until reaching a leaf node
    {
        node = bits[node] ? (2 * node + 2) : (2 * node + 1); // Move left or right based on the bit value at the current node( 1 for right, 0 for left)
    }

    return node - bits.size(); // Return the line index corresponding to the leaf node reached
}

void PLRUTree::updateLine(uint8_t lineIndex)
{
    uint8_t node = lineIndex + bits.size(); // Start at the leaf node corresponding to the accessed line index

    while (node > 0) // Traverse the tree until reaching a leaf node
    {
        uint8_t parent = (node - 1) / 2; // Calculate the parent node index
        bits[parent] = node = 2 * parent + 1 ? 0 : 1; // Update the bit at the parent node to indicate the direction taken (1 for right, 0 for left) and move up to the parent node
        node = parent;
    }
}


PLRUReplacementPolicy::PLRUReplacementPolicy(const uint8_t& numSets, const uint8_t& associativity)
{
    for (uint8_t i = 0; i < numSets; ++i)
    {
        initializeSet(i, associativity); // Initialize the PLRU state for each cache set based on the number of sets and associativity
    }
}

void PLRUReplacementPolicy::initializeSet(uint8_t setIndex, uint8_t associativity)
{
    plruMap[setIndex] = PLRUTree(associativity); // Initialize the PLRU tree for the cache set based on the associativity
}

uint8_t PLRUReplacementPolicy::selectLineToReplace(CacheSet& set)
{
    PLRUTree& tree = plruMap[set.setIndex]; // Get the PLRU tree for the cache set
    if (tree.validLines < set.lines.size()) // If there are still invalid lines in the set
    {
        return tree.validLines++; // Return the next free line index and increment the count of valid lines
    }

    return tree.selectLineToReplace(); // Select the line to replace based on the PLRU tree for the cache set
}

void PLRUReplacementPolicy::updateOnAccess(CacheSet& set, uint8_t lineIndex)
{
    PLRUTree& tree = plruMap[set.setIndex]; // Get the PLRU tree for the cache set
    tree.updateLine(lineIndex); // Update the PLRU tree on cache access to reflect the most recently used line
}

void PLRUReplacementPolicy::onLineLoaded(CacheSet& set, uint8_t lineIndex)
{
    PLRUTree& tree = plruMap[set.setIndex]; // Get the PLRU tree for the cache set
    tree.updateLine(lineIndex); // Update the PLRU tree when a line is loaded into the cache to reflect the most recently used line
}


uint8_t RandomReplacementPolicy::selectLineToReplace(CacheSet& set)
{
    std::random_device rd; // Obtain a seeder for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(0, set.lines.size() - 1); // Uniform distribution to select a random line index from the set

    return dis(gen); // Return a randomly selected line index from the set
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CacheStorage::CacheStorage(uint8_t numSets, uint8_t associativity)
{
    sets.resize(numSets); // Resize the vector of cache sets based on the number of sets

    for (uint8_t i = 0; i < numSets; ++i)
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

CacheLine* CacheStorage::findLine(uint8_t setIndex, uint64_t tag)
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

uint8_t CacheStorage::findLineIndex(uint8_t setIndex, uint64_t tag)
{
    CacheSet& set = sets[setIndex];
    for (uint8_t i = 0; i < set.lines.size(); ++i)
        if (set.lines[i].valid && set.lines[i].tag == tag)
            return i;

    return -1; // miss
}

void CacheStorage::invalidateLine(uint8_t setIndex, uint8_t lineIndex)
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
    for (uint8_t i = 0; i < sets.size(); ++i) // Loop through all cache sets
    {
        for (uint8_t j = 0; j < sets[i].lines.size(); ++j) // Loop through all lines in the cache set
        {
            invalidateLine(i, j); // Invalidate each line in the cache set
        }
    }
}


void CacheStorage::invalidateLineByAddress(AddressInfo addressInfo)
{
    uint8_t lineIndex = findLineIndex(addressInfo.setIndex, addressInfo.tag); // Find the line index based on the set index and tag from the address information

    if (lineIndex < 0) [[unlikely]] // If no matching line is found, return without invalidating
        return;

    invalidateLine(addressInfo.setIndex, lineIndex); // Invalidate the specific cache line based on the set index and line index
}

void CacheStorage::flush(auto&& memoryWriteFunction)
{
    for (CacheSet& set : sets) // Loop through all cache sets
    {
        for (CacheLine& line : set.lines) // Loop through all lines in the cache set
        {
            if (line.valid && line.dirty) // Check if the line is valid and dirty before flushing
            {
                memoryWriteFunction(set, line);
                // Example: memory.write(line.tag * CACHE_LINE_SIZE, line.data);
                line.dirty = false; // Mark the line as not dirty after flushing
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RequestScheduler::tick()
{
    for (auto it = pendingRequests.begin(); it != pendingRequests.end(); )
    {
        if (it->state == RequestState::WAITING_LATENCY) // Check if the request is in WAITING state
        {
            --(it->remainingLatency); // Decrement the remaining latency for the request

            if (it->remainingLatency <= 0) // If the request is ready to be processed
            {
                it->state = RequestState::READY_TO_PROCESS; // Update the state to READY_TO_PROCESS
                cacheControllerCallback(*(it->request)); // Call the cache controller callback function to process the request
                it = pendingRequests.erase(it); // Remove the request from the pending requests vector after processing
            }
            else
            {
                ++it; // Move to the next request in the vector
            }
        }
        else
        {
            ++it; // Move to the next request in the vector if it's not in WAITING state
        }
    }
}
/*
/// CacheLevel class implementation
CacheLevel::CacheLevel(uint64_t size, uint64_t associativity, uint64_t latency, Bus& bus, CacheLevel* nextLevel, CacheLevel* parentLevel)
    : cacheSize(size), associativity(associativity), numSets(size / (associativity * CACHE_LINE_SIZE)), bus(bus), nextLevel(nextLevel), parentLevel(parentLevel), latency_cycles(latency), controller(numSets, associativity)
{
    //resizing the vector of sets
    std::vector<CacheSet>& sets = controller.getSets();
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

/// Function to LOAD an entire cache line into the cache
void CacheLevel::load(uint64_t setIndex, uint64_t tag, const CacheLine& data, uint64_t freePosition)
{
    // Load data into the cache line at the specified set index and tag
    CacheSet& set = controller.getSets()[setIndex]; // Get the cache set based on the set index

    CacheLine& line = set.lines[freePosition]; // Get the cache line at the free position

    line = data;
    line.tag = tag;
    line.valid = true;

    line.lastAccessTime = bus.getClock().getCycles(); // Update the last access time

    debugLog("Loaded data into cache at set index: " + std::to_string(setIndex) + ", tag: " + std::to_string(tag));



}

/// Function to find a cache line in a set(for read and write operations or managing replacement policy)
CacheLine* CacheLevel::findLine(CacheSet& set , uint64_t tag)
{

    for(uint64_t i = 0; i < set.lines.size(); ++i)
    {
        if (set.lines[i].valid && set.lines[i].tag == tag)
        {
            return (&set.lines[i]);
        }
    }

    return nullptr; // Return nullptr if no matching line is found
}

/// Function to invalidate a specific cache line based on address
void CacheLevel::invalidate(uint64_t address)
{
    uint64_t setIndex = (address / CACHE_LINE_SIZE) % numSets; // Calculate the set index
    uint64_t tag = address / (CACHE_LINE_SIZE * numSets); // Calculate the tag

    CacheSet& set = controller.getSets()[setIndex]; // Get the cache set based on the set index

    for (CacheLine& line : set.lines) // Loop through the lines in the set
    {
        

        if (line.valid && line.tag == tag) // Check if the line is valid and the tag matches
        {
            //check if the line is dirty
            if (line.dirty)
            {
                bus.getMemory().writeGeneric( (line.tag * numSets + setIndex) * CACHE_LINE_SIZE, line.data);
            }
            
            line.valid = false; // Invalidate the line
            line.dirty = false; // Mark the line as not dirty
            debugLog("Invalidated cache line at set index: " + std::to_string(setIndex) + ", tag: " + std::to_string(tag));
            return; // Exit the function
        }
    }

    debugLog("No matching cache line found to invalidate at address: " + to_string_hex(address));

}

/// Function to invalidate all cache lines in the cache
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

/// Function to flush the cache (write back all dirty lines to memory)
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

    debugLog("Cache flushed");

}

//helper function to print the state of the cache for debugging purposes
void CacheLevel::printCacheState() const
{
    // Print the state of the cache for debugging purposes
    debugLog("Cache State:");
    for (const CacheSet& set : sets) // Loop through the cache sets
    {
        debugLog("Set Index: " + std::to_string(set.setIndex));
        for (const CacheLine& line : set.lines) // Loop through the lines in the set
        {
            debugLog("  Line Tag: " + std::to_string(line.tag) + ", Valid: " + std::to_string(line.valid) + ", Dirty: " + std::to_string(line.dirty));
            for(int i = 0; i < 8; ++i)
            {
                std::ostringstream oss;
                oss << std::hex << std::setw(2) << std::setfill('0') << +line.data[i];
                debugLog("    Data[" + std::to_string(i) + "]: 0x" + oss.str());
            }
        }
    }
}


/// Function to find a free line in a set or manage replacement policy if none are free
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

/// Function to manage the replacement policy (LRU) and return the index of the line to be replaced
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
            nextLevel->write( (set.lines[index].tag * numSets + set.setIndex) * CACHE_LINE_SIZE, set.lines[index].data, CACHE_LINE_SIZE, nullptr);
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

/// Function to read data from the cache
Result<CacheLine> CacheLevel::readSingle(uint64_t address)
{
    //craetion of the structure for the result
    Result<CacheLine> result;

    uint64_t offset = address % CACHE_LINE_SIZE; // Calculate the offset within the cache line

    debugLog("Offset: " + std::to_string(offset));
    //manage the offset for the read operation
    if (offset_cache(EventType::CACHE_READ_ERROR, ErrorType::READ_FAIL, result, offset, address)) 
    {
        debugLog("Cache read error at address: " + to_string_hex(address));
        return result;
    }

    debugLog("Offset after check: " + std::to_string(offset));

    uint64_t setIndex = (address / CACHE_LINE_SIZE) % numSets; // Calculate the set index
    debugLog("Set index: " + std::to_string(setIndex));
    uint64_t tag = address / (CACHE_LINE_SIZE * numSets); // Calculate the tag
    debugLog("Tag: " + std::to_string(tag));

    CacheSet& set = sets[setIndex]; // Get the cache set
    auto* line = findLine(set, tag); // Check if the line is in the cache

    if(line != nullptr)
    {
        // Cache hit

        // Read the data from the cache line
        result.data = *line;

        line->lastAccessTime = bus.getClock().getCycles(); // Update the last access time

        // Set success to true
        result.success = true; 

        // Set the event type to CACHE_HIT
        result.errorInfo.event = EventType::CACHE_HIT; // Set the event type to CACHE_HIT
        result.errorInfo.source = ComponentType::CACHE; // Set the source to CACHE
        result.errorInfo.message = "Cache hit at address: " + std::to_string(address); // Set the message for debugging
        result.errorInfo.error = ErrorType::NONE; // Set the error type to NONE

        debugLog("Cache hit at address: " + to_string_hex(address));
        

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

        debugLog("Cache miss at address: " + to_string_hex(address)); // Print the cache miss message

        return result; // Return the result
    }
    
    
}

Result<std::array<uint8_t, 2*CACHE_LINE_SIZE>> CacheLevel::readCrossLines(uint64_t address, uint64_t size)
{
    // Read data that spans across two cache lines
    Result<std::array<uint8_t, 2*CACHE_LINE_SIZE>> result;
    std::array<uint8_t, 2*CACHE_LINE_SIZE> data{};

    uint64_t offset = address % CACHE_LINE_SIZE; // Calculate the offset within the cache line

    // Read the first part of the data from the first cache line
    Result<CacheLine> result1 = readSingle(address); // Read from the cache
    if (!result1.success)
    {
        result.success = false;
        result.errorInfo = result1.errorInfo;
        return result;
    }

    std::memcpy(data.data(), &result1.data.data[offset], CACHE_LINE_SIZE - offset); // Copy the first part of the data to the result array

    uint64_t new_address = address + (CACHE_LINE_SIZE - offset); // Calculate the address for the second cache line

    // Read the second part of the data from the second cache line
    Result<CacheLine> result2 = readSingle(new_address); // Read from the cache
    if (!result2.success)
    {
        result.success = false;
        result.errorInfo = result2.errorInfo;
        return result; 
    }

    std::memcpy(data.data() + (CACHE_LINE_SIZE - offset), &result2.data.data[0], size - (CACHE_LINE_SIZE - offset)); // Copy the second part of the data to the result array
    result.data = data; // Set the data in the result structure
    result.success = true; // Set success to true
    result.errorInfo.event = EventType::CACHE_HIT; // Set the event type to CACHE_HIT
    result.errorInfo.source = ComponentType::CACHE; // Set the source to CACHE
    result.errorInfo.message = "Cache hit (cross lines) at address: " + to_string_hex(address); // Set the message for debugging
    result.errorInfo.error = ErrorType::NONE; // Set the error type to NONE

    return result; // Return the result
}

Result<std::array<uint8_t, 2*CACHE_LINE_SIZE>> CacheLevel::read(uint64_t address, uint64_t size, bool* twoLines)
{
    // Read data from the cache based on a cache request
    Result<std::array<uint8_t, 2*CACHE_LINE_SIZE>> result;
    std::array<uint8_t, 2*CACHE_LINE_SIZE> data{};
    Result<CacheLine> singleLineResult;
    uint64_t offset = address % CACHE_LINE_SIZE; // Calculate the offset within the cache line

    if(offset + size > CACHE_LINE_SIZE)
    {
        *twoLines = true;
        result = readCrossLines(address, size); // Read from the cache handling the case of crossing cache lines
    }
    else
    {
        *twoLines = false;
        // The read is within a single cache line
        singleLineResult = readSingle(address); // Read from the cache

        if (singleLineResult.success)
        {
            // Extract the requested data from the cache line
            
            std::memcpy(data.data(), &singleLineResult.data.data[offset], size); // Copy the requested data to the result array
            result.data = data; // Set the data in the result structure
            result.success = true; // Set success to true
        }
        else
        {
            result.data.fill(0); // Fill the data with zeros in case of a read failure
            result.success = false; // Set success to false
            result.errorInfo = singleLineResult.errorInfo; // Set the error information from the single line read result
           
        }
    }

    if(!result.success)
    {
        return result; // Return the result with the error information if the read operation failed
    }
    else
    {
        result.errorInfo.event = EventType::CACHE_HIT; // Set the event type to CACHE_HIT
        result.errorInfo.source = ComponentType::CACHE; // Set the source to CACHE
        result.errorInfo.message = "Cache hit at address: " + to_string_hex(address); // Set the message for debugging
        result.errorInfo.error = ErrorType::NONE; // Set the error type to NONE
    }
    return result; // Return the result

   
}

Result<void> CacheLevel::write(uint64_t address, const std::array<uint8_t, CACHE_LINE_SIZE>& data, uint64_t size, bool* twoLines)
{
    // Create a result structure for the write operation
    Result<void> result;

    uint64_t offset = address % CACHE_LINE_SIZE; // Calculate the offset within the cache line

    //manage the offset for the write operation
    if (offset_cache(EventType::CACHE_WRITE_ERROR, ErrorType::WRITE_FAIL, result, offset, address))
    {   
        debugLog("Write request at address: " + to_string_hex(address) + " exceeds cache line boundary with offset: " + std::to_string(offset));
        return result; // Return the result with the error information
        
    }

    if(offset + size > CACHE_LINE_SIZE)
    {
       result = writeCrossLines(address, data, size); // Write to the cache handling the case of crossing cache lines
       *twoLines = true;
    }
    else
    {
        result = writeSingleLine(address, data, size); // Write to a single line
        *twoLines = false;
    }

    return result; // Return the result of the write operation
}

Result<void> CacheLevel::writeSingleLine(uint64_t address, const std::array<uint8_t, CACHE_LINE_SIZE>& data, uint64_t size)
{
    Result<void> result;

    uint64_t offset = address % CACHE_LINE_SIZE; // Calculate the offset within the cache line

    //manage the offset for the write operation
    if (offset_cache(EventType::CACHE_WRITE_ERROR, ErrorType::WRITE_FAIL, result, offset, address))
    {   
        debugLog("Write request at address: " + to_string_hex(address) + " exceeds cache line boundary with offset: " + std::to_string(offset));
        return result; // Return the result with the error information
        
    }

    uint64_t setIndex = (address / CACHE_LINE_SIZE) % numSets; // Calculate the set index
    uint64_t tag = address / (CACHE_LINE_SIZE * numSets); // Calculate the tag


     if (offset_cache(EventType::CACHE_WRITE_ERROR, ErrorType::WRITE_FAIL, result, offset, address))
    {   
        debugLog("Write request at address: " + to_string_hex(address) + " exceeds cache line boundary with offset: " + std::to_string(offset));
        return result; // Return the result with the error information
        
    }

    CacheSet& set = sets[setIndex]; // Get the cache set

    auto* line = findLine(set, tag); // Check if the line is in the cache

    if(line != nullptr)
    {
        
        // Cache hit
        line->dirty = true; // Mark the line as dirty
        line->lastAccessTime = bus.getClock().getCycles(); // Update the last access time

        // Write the data to the cache line
        std::memcpy(&line->data[offset], &data, size);
        result.success = true; // Set success to true

        // Set the event type to CACHE_HIT
        result.errorInfo.event = EventType::CACHE_HIT; // Set the event type to CACHE_HIT
        result.errorInfo.source = ComponentType::CACHE; // Set the source to CACHE
        result.errorInfo.message = "Cache hit at address: " + to_string_hex(address); // Set the message for debugging
        result.errorInfo.error = ErrorType::NONE; // Set the error type to NONE

        debugLog("Cache hit at address: " + to_string_hex(address));

        return result; // Return the result

    }
    else
    {
        // Cache miss
        result.success = false; // Set success to false

        // Set the event type to CACHE_MISS
        result.errorInfo.event = EventType::CACHE_MISS; // Set the event type to CACHE_MISS
        result.errorInfo.source = ComponentType::CACHE; // Set the source to CACHE
        result.errorInfo.message = "Cache miss at address: " + to_string_hex(address); // Set the message for debugging
        result.errorInfo.error = ErrorType::NONE;

        debugLog("Cache miss at address: " + to_string_hex(address));

        return result; // Return the result
    }
    
}

Result<void> CacheLevel::writeCrossLines(uint64_t address, const std::array<uint8_t, CACHE_LINE_SIZE>& data, uint64_t size)
{
    Result<void> result;

    uint64_t offset = address % CACHE_LINE_SIZE;

    uint64_t totalSize = size;

    uint64_t firstSize =
        CACHE_LINE_SIZE - offset;

    uint64_t secondSize =
        totalSize - firstSize;

    auto bytes =
        reinterpret_cast<const uint8_t*>(&data);

    std::array<uint8_t,CACHE_LINE_SIZE> first{};
    std::array<uint8_t,CACHE_LINE_SIZE> second{};

    memcpy(first.data(),bytes,firstSize);
    memcpy(second.data(),
           bytes+firstSize,
           secondSize);

    result=writeSingleLine(
        address,
        first,
        firstSize
    );

    if(!result.success)
        return result;

    return writeSingleLine(
        address+firstSize,
        second,
        secondSize
    );
}

void CacheLevel::execute_operation()
{
   processRequest(); // Process cache requests
}

void CacheLevel::processRequest()
{
    debugLog("Processing cache requests for cache level with latency: " + std::to_string(latency_cycles) + " cycles");

    for (auto it = pendingRequests.begin(); it != pendingRequests.end();)
    {
        debugLog("Processing pending request with remaining latency: " + std::to_string(it->remainingLatency) + " cycles");
        if (it->remainingLatency > 1)
        {
            debugLog("Request still has latency remaining, decrementing latency and moving to next request");
            --it->remainingLatency;
            ++it;
            continue;
        }

        if (!it->request)
        {
            it = pendingRequests.erase(it);
            continue;
        }

        CacheRequest& request = *it->request;
        CacheLine* fillData = &(it->line); // Get the cache line data for fill requests
        debugLog("Processing request of type: " + std::to_string(static_cast<int>(request.type)) + " at address: " + to_string_hex(request.address));

        uint64_t offset = request.address % CACHE_LINE_SIZE; // Calculate the offset within the cache line
        uint64_t setIndex = (request.address / CACHE_LINE_SIZE) % numSets;
        uint64_t tag = request.address / (CACHE_LINE_SIZE * numSets);

        debugLog("Calculated set index: " + std::to_string(setIndex) + ", tag: " + std::to_string(tag) + ", offset: " + std::to_string(offset));

        CacheSet& set = sets[setIndex];
        CacheLine* line = findLine(set, tag);
        uint64_t startSecondLineAddress = request.address + (CACHE_LINE_SIZE - offset);
        uint64_t setIndexSecondLine = (startSecondLineAddress / CACHE_LINE_SIZE) % numSets;
        uint64_t tagSecondLine = startSecondLineAddress / (CACHE_LINE_SIZE * numSets);
        CacheLine* secondLine = findLine(sets[setIndexSecondLine], tagSecondLine);

        debugLog("Cache line found: " + std::to_string(line != nullptr) + " for set index: " + std::to_string(setIndex) + ", tag: " + std::to_string(tag));

        //per futuro, possibile uso di laod per semplificare il codice e ridurre la duplicazione, ma per ora lascio così per chiarezza
        if (request.type == RequestType::FILL)
        {
            if (fillData)
            {
                if (line)
                {
                    debugLog("Filling existing cache line at set index: " + std::to_string(setIndex) + ", tag: " + std::to_string(tag));
                    *line = *fillData; // Update the existing cache line with the new data
                    line->lastAccessTime = bus.getClock().getCycles();
                }
                else
                {
                    debugLog("Creating new cache line at set index: " + std::to_string(setIndex) + ", tag: " + std::to_string(tag));
                    CacheLine newLine{};
                    newLine.valid = true;
                    newLine.dirty = false;
                    newLine.tag = tag;
                    newLine.data = fillData->data;
                    uint64_t freePos = findFreeLineIndex(set);
                    load(setIndex, tag, newLine, freePos);
                }
            }

            it = pendingRequests.erase(it);
            continue;
        }
        bool fill_next_level = false; // Flag to determine if the request should be forwarded to the next cache level
        bool twoLines = false;
        TypeofData dataTypeSize = request.dataType; // Get the data type for the request

        if (request.type == RequestType::READ)
        {   

            debugLog("Processing read request at address: " + to_string_hex(request.address) + " with offset: " + std::to_string(offset));
            std::unique_ptr<Result<anydata>> response = std::make_unique<Result<anydata>>();
            debugLog("a");

            Result<std::array<uint8_t, 2*CACHE_LINE_SIZE>> readResult= read(request.address, uint64_t(dataTypeSize), &twoLines); // Read from the cache based on the request data type size 
           
            response->success = readResult.success;
            response->errorInfo = readResult.errorInfo;
            std::cout<< readResult.errorInfo.event << std::endl;
            debugLog("Read operation completed with success: " + std::to_string(readResult.success) + " for address: " + to_string_hex(request.address));
            if (readResult.success)
            {
                switch (dataTypeSize)
                {
                case TypeofData::UINT_8T:
                    uint8_t value8;
                    std::memcpy(&value8, readResult.data.data(), sizeof(uint8_t));
                    response->data = value8;
                    break;
                case TypeofData::UINT_16T:
                    uint16_t value16;
                    std::memcpy(&value16, readResult.data.data(), sizeof(uint16_t));
                    response->data = value16;
                    break;
                case TypeofData::UINT_32T:
                    uint32_t value32;
                    std::memcpy(&value32, readResult.data.data(), sizeof(uint32_t));
                    response->data = value32;
                    break;
                case TypeofData::UINT_64T:
                    uint64_t value64;
                    std::memcpy(&value64, readResult.data.data(), sizeof(uint64_t));
                    response->data = value64;
                    break;
                case TypeofData::ARRAY_16B:
                    std::array<uint8_t, 16> valueArray;
                    std::memcpy(&valueArray, readResult.data.data(), sizeof(std::array<uint8_t, 16>));
                    response->data = valueArray;
                    break;
                case TypeofData::ARRAY_64B:
                    std::array<uint8_t, CACHE_LINE_SIZE> valueLine;
                    std::memcpy(&valueLine, readResult.data.data(), sizeof(std::array<uint8_t, CACHE_LINE_SIZE>));
                    response->data = valueLine;
                    break;
                case TypeofData::ARRAY_128B:
                    std::array<uint8_t, 2*CACHE_LINE_SIZE> valueDoubleLine;
                    std::memcpy(&valueDoubleLine, readResult.data.data(), sizeof(std::array<uint8_t, 2*CACHE_LINE_SIZE>));
                    response->data = valueDoubleLine;
                    break;
                
                default:
                    break;
                }
            }
            else
            {
                response->data = std::monostate{};
            }
            fill_next_level = readResult.errorInfo.event == EventType::CACHE_MISS;

            debugLog("Read request processing completed with success: " + std::to_string(response->success) + " for address: " + to_string_hex(request.address));
            
            if (response->success)
            {
                CacheLevel* topLevel = this;
                while (topLevel->getParentLevel())
                {
                    topLevel = topLevel->getParentLevel();
                }

                if (line)
                {
                    for (CacheLevel* level = getParentLevel(); level && level != topLevel; level = level->getParentLevel())
                    {
                        auto fillRequest = std::make_unique<CacheRequest>();
                        fillRequest->type = RequestType::FILL;
                        fillRequest->address = request.address;
                        fillRequest->requestID = 0;
                        fillRequest->callback = nullptr;
                        level->pendingRequests.emplace_back(std::move(fillRequest), static_cast<int>(level->latency_cycles), *line);

                        if(twoLines)
                        {
                            auto fillRequest2 = std::make_unique<CacheRequest>();
                            fillRequest2->type = RequestType::FILL;
                            fillRequest2->address = request.address + CACHE_LINE_SIZE; // Address for the second line
                            fillRequest2->requestID = 0;
                            fillRequest2->callback = nullptr;
                            level->pendingRequests.emplace_back(std::move(fillRequest2), static_cast<int>(level->latency_cycles), *secondLine);
                        }
                    }
                }

                bus.getCPU().cacheResponseQueue[request.requestID] = std::move(response);
                if (request.callback)
                {
                    debugLog("Invoking read callback for request ID: " + std::to_string(request.requestID));
                    request.callback();
                }
            }
            else
            {
                debugLog("Read request resulted in cache miss for address: " + to_string_hex(request.address));
                if (fill_next_level)
                {
                    if (nextLevel)
                    {
                        debugLog("Forwarding read request to next cache level with latency: " + std::to_string(nextLevel->latency_cycles) + " cycles");
                        nextLevel->pendingRequests.emplace_back(std::move(it->request), static_cast<int>(nextLevel->latency_cycles));
                    }
                    else
                    {
                        debugLog("Forwarding read request to main memory");
                        bus.getCPU().getCacheManager().enqueueMemoryRequest(std::move(it->request));
                    }
                        
                }
                
                
            }

            it = pendingRequests.erase(it);
            continue;
        }

        if (request.type == RequestType::WRITE)
        {
            std::unique_ptr<Result<anydata>> response = std::make_unique<Result<anydata>>();

            
            Result<void> writeResult = write(request.address, reinterpret_cast<const std::array<uint8_t, CACHE_LINE_SIZE>&>(request.data), uint64_t(request.dataType),&twoLines); // Write to the cache based on the request data type size
            response->success = writeResult.success;
            response->errorInfo = writeResult.errorInfo;
            response->data = std::monostate{};
            fill_next_level = writeResult.errorInfo.event == EventType::CACHE_MISS;

            if (response->success)
            {
                CacheLevel* topLevel = this;
                while (topLevel->getParentLevel())
                {
                    topLevel = topLevel->getParentLevel();
                }

                if (line)
                {

                    for (CacheLevel* level = getParentLevel(); level && level != topLevel; level = level->getParentLevel())
                    {
                        auto fillRequest = std::make_unique<CacheRequest>();
                        fillRequest->type = RequestType::FILL;
                        fillRequest->address = request.address;
                        fillRequest->requestID = 0;
                        fillRequest->callback = nullptr;
                        level->pendingRequests.emplace_back(std::move(fillRequest), static_cast<int>(level->latency_cycles), *line);

                        if(twoLines)
                        {
                            auto fillRequest2 = std::make_unique<CacheRequest>();
                            fillRequest2->type = RequestType::FILL;
                            fillRequest2->address = request.address + CACHE_LINE_SIZE; // Address for the second line
                            fillRequest2->requestID = 0;
                            fillRequest2->callback = nullptr;
                            level->pendingRequests.emplace_back(std::move(fillRequest2), static_cast<int>(level->latency_cycles), *secondLine);
                        }
                    }
                }
                bus.getCPU().cacheResponseQueue[request.requestID] = std::move(response);
                if (request.callback)
                {
                    debugLog("Invoking write callback for request ID: " + std::to_string(request.requestID));
                    request.callback();
                }
            }
            else if (fill_next_level)
            {
                if (nextLevel)
                {
                    debugLog("Forwarding write request to next cache level with latency: " + std::to_string(nextLevel->latency_cycles) + " cycles");
                    nextLevel->pendingRequests.emplace_back(std::move(it->request), static_cast<int>(nextLevel->latency_cycles));
                }
                else
                {   debugLog("Forwarding write request to main memory");
                    bus.getCPU().getCacheManager().enqueueMemoryRequest(std::move(it->request));
                }
            }

            it = pendingRequests.erase(it);
            continue;
        }

        it = pendingRequests.erase(it);
    }
    debugLog("Finished processing cache requests.");
}

//helpers functions specializations for offset_cache
template<>
bool offset_cache(EventType event, ErrorType error, Result<void>& result, uint64_t offset, uint64_t address)
{
    if (offset >= CACHE_LINE_SIZE)
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
    if (offset >= CACHE_LINE_SIZE)
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

template<>
bool offset_cache(EventType event, ErrorType error, Result<CacheLine>& result, uint64_t offset, uint64_t address)
{
    (void)event;
    (void)error;
    (void)result;
    (void)offset;
    (void)address;
    return false; // Cache line reads ignore offset checks
}




// CacheManager class implementation
CacheManager::CacheManager(Bus& bus, uint64_t l1Size, uint64_t l2Size, uint64_t l3Size,uint64_t l1Associativity, uint64_t l2Associativity, uint64_t l3Associativity, uint64_t  l1Latency, uint64_t l2Latency, uint64_t l3Latency) 
                         :L1Cache(l1Size, l1Associativity, l1Latency, bus, &L2Cache), L2Cache(l2Size, l2Associativity, l2Latency, bus, &L3Cache), L3Cache(l3Size, l3Associativity, l3Latency, bus, nullptr), bus(bus), memoryScheduler(memory_latency)
{   
    L2Cache.setParentLevel(&L1Cache);
    L3Cache.setParentLevel(&L2Cache);
    debugLog("CacheManager initialized with L1 size: " + std::to_string(l1Size) + " bytes, L2 size: " + std::to_string(l2Size) + " bytes, L3 size: " + std::to_string(l3Size) + " bytes");
    debugLog("L1 associativity: " + std::to_string(l1Associativity) + ", L2 associativity: " + std::to_string(l2Associativity) + ", L3 associativity: " + std::to_string(l3Associativity));
   
    
}

CacheManager::~CacheManager()
{
    // No dynamic memory to free, but can be used for cleanup if needed
}


void CacheManager::execute_operation()
{
    processRequest(); // Process cache requests
}

void CacheManager::processRequest()
{   
    
    if(!requestQueue.empty())
    {
        debugLog("Enqueuing request to L1 cache. Remaining requests in queue: " + std::to_string(requestQueue.size()));
        auto request = std::move(requestQueue.front());
        requestQueue.pop();
        L1Cache.pendingRequests.emplace_back(std::move(request), static_cast<int>(L1Cache.getLatency()));
    }

    if (!requestQueueMemory.empty())
    {
        debugLog("Enqueuing request to memory. Remaining memory requests in queue: " + std::to_string(requestQueueMemory.size()));
        auto request = std::move(requestQueueMemory.front());
        requestQueueMemory.pop();
        pendingMemoryRequests.emplace_back(std::move(request), memory_latency);
    }

    for (auto it = pendingMemoryRequests.begin(); it != pendingMemoryRequests.end();)
    {
        debugLog("Processing pending memory request with remaining latency: " + std::to_string(it->remainingLatency) + " cycles");
        if (it->remainingLatency > 1)
        {
            debugLog("Memory request still has latency remaining, decrementing latency and moving to next request");
            --it->remainingLatency;
            ++it;
            continue;
        }

        if (!it->request)
        {
            it = pendingMemoryRequests.erase(it);
            continue;
        }

        debugLog("Memory request latency completed, processing request at address: " + to_string_hex(it->request->address) + " of type: " + std::to_string(static_cast<int>(it->request->type)));

        auto& request = *it->request;

        std::unique_ptr<Result<anydata>> response = std::make_unique<Result<anydata>>();
        CacheLine line{};
        uint64_t lineStart = request.address - (request.address % CACHE_LINE_SIZE);

        const bool isRead = request.type == RequestType::READ;
        const bool isWrite = request.type == RequestType::WRITE;
        bool success = false;

        debugLog("Calculated line start address: " + to_string_hex(lineStart) + " for request address: " + to_string_hex(request.address));

        auto enqueueFill = [&](CacheLevel& level,  CacheLine& fillLine) {
                fillLine.tag = lineStart / (CACHE_LINE_SIZE * level.getNumSets());
                auto fillRequest = std::make_unique<CacheRequest>();
                fillRequest->type = RequestType::FILL;
                fillRequest->address = lineStart;
                fillRequest->requestID = 0;
                fillRequest->callback = nullptr;
                level.pendingRequests.emplace_back(std::move(fillRequest), static_cast<int>(level.getLatency()), fillLine);
            };
        
        debugLog("a");
        if (!isRead && !isWrite)
        {
            response->success = false;
            response->errorInfo.event = EventType::ERROR;
            response->errorInfo.source = ComponentType::RAM;
            response->errorInfo.message = "Unsupported cache request type.";
            response->errorInfo.error = ErrorType::UNKNOWN;
        }
        else
        {
            debugLog("b");
            uint64_t offset = request.address - lineStart;

            Result<std::array<uint8_t, CACHE_LINE_SIZE>> readResult;
            Result<void> writeResult;
           

            switch(request.type)
            {
                case RequestType::READ:
                {
                    debugLog("Performing memory read for address: " + to_string_hex(request.address) + " with offset: " + std::to_string(offset) + " and size: " + std::to_string(static_cast<int>(request.dataType)));
                    readResult = bus.getMemory().template readGeneric<std::array<uint8_t, CACHE_LINE_SIZE>>(lineStart);
                    debugLog("Memory read completed with success: " + std::to_string(readResult.success) + " for address: " + to_string_hex(request.address));
                    if (!readResult.success)
                    {
                        response->success = false;
                        response->errorInfo = readResult.errorInfo;
                        response->data = std::monostate{};
                    }
                    else
                    {   response->success = true;
                        success = true;
                        debugLog("Memory read successful for address: " + to_string_hex(request.address));
                        response->errorInfo.event = EventType::RAM_ACCESS;
                        response->errorInfo.source = ComponentType::RAM;
                        response->errorInfo.error = ErrorType::NONE;
                        
                        switch(request.dataType)
                        {
                            case TypeofData::UINT_8T:
                                uint8_t value8;
                                std::memcpy(&value8, readResult.data.data() + offset, sizeof(uint8_t));
                                response->data = value8;
                                break;
                            case TypeofData::UINT_16T:
                                uint16_t value16;
                                std::memcpy(&value16, readResult.data.data() + offset, sizeof(uint16_t));
                                response->data = value16;
                                break;
                            case TypeofData::UINT_32T:
                                uint32_t value32;
                                std::memcpy(&value32, readResult.data.data() + offset, sizeof(uint32_t));
                                response->data = value32;
                                break;
                            case TypeofData::UINT_64T:
                                uint64_t value64;
                                std::memcpy(&value64, readResult.data.data() + offset, sizeof(uint64_t));
                                response->data = value64;
                                break;
                            case TypeofData::ARRAY_16B:
                                std::array<uint8_t, 16> valueArray;
                                std::memcpy(&valueArray, readResult.data.data() + offset, sizeof(std::array<uint8_t, 16>));
                                response->data = valueArray;
                                break;
                            case TypeofData::ARRAY_64B:
                                std::array<uint8_t, CACHE_LINE_SIZE> valueLine;
                                std::memcpy(&valueLine, readResult.data.data() + offset, sizeof(std::array<uint8_t, CACHE_LINE_SIZE>));
                                response->data = valueLine;
                                break;
                            case TypeofData::ARRAY_128B:
                                std::array<uint8_t, 2*CACHE_LINE_SIZE> valueDoubleLine;
                                std::memcpy(&valueDoubleLine, readResult.data.data() + offset, sizeof(std::array<uint8_t, 2*CACHE_LINE_SIZE>));
                                response->data = valueDoubleLine;
                                break;
                            
                            default:
                                response->data = std::monostate{};
                                break;
                        }
                        
                        
                        line.valid = true;
                        line.dirty = false;
                        line.tag = 0;
                        line.data = readResult.data;

                        debugLog("Prepared cache line for filling with data from memory read at address: " + to_string_hex(request.address) + " with offset: " + std::to_string(offset));

                    }
                    break;
                }
                case RequestType::WRITE:
                {
                    std::array<uint8_t, CACHE_LINE_SIZE> lineData{};
                    
                    writeResult = bus.getMemory().template writeGeneric<std::array<uint8_t, CACHE_LINE_SIZE>>(lineStart, lineData);
              
                    if (!writeResult.success)
                    {
                        response->success = false;
                        response->errorInfo.event = EventType::RAM_WRITE_ERROR;
                        response->errorInfo.source = ComponentType::RAM;
                        response->errorInfo.message = "Failed to write to RAM at address: " + std::to_string(request.address);
                        response->errorInfo.error = ErrorType::WRITE_FAIL;
                        response->data = std::monostate{};
                    }
                    else
                    {
                        response->success = true;
                        success = true;
                        response->errorInfo.event = EventType::RAM_ACCESS;
                        response->errorInfo.source = ComponentType::RAM;
                        response->errorInfo.error = ErrorType::NONE;
                        response->data = std::monostate{};

                        line.valid = true;
                        line.dirty = false;
                        line.data = lineData;

                    }
                    break;
                }
                default:
                    response->success = false;
                    response->errorInfo.event = EventType::ERROR;
                    response->errorInfo.source = ComponentType::RAM;
                    response->errorInfo.message = "Unsupported cache request type.";
                    response->errorInfo.error = ErrorType::UNKNOWN;
                    response->data = std::monostate{};
                    break;
            }
        }
        debugLog("Memory request processing completed for address: " + to_string_hex(request.address) + " with success: " + std::to_string(response->success));
        debugLog(std::to_string(response->success));
        bus.getCPU().cacheResponseQueue[request.requestID] = std::move(response);
        
        debugLog("Response enqueued for CPU with request ID: " + std::to_string(request.requestID));
        if(success)
        {
            debugLog("Enqueuing fill requests to cache levels for address: " + to_string_hex(request.address));
            enqueueFill(L3Cache, line);
            enqueueFill(L2Cache, line);
            enqueueFill(L1Cache, line);

        

        }
        else debugLog("Memory request failed for address: " + to_string_hex(request.address) + ", not enqueuing fill requests to cache levels.");
        if (request.callback)
        {
            request.callback();
        }
        
        it = pendingMemoryRequests.erase(it);
    }
    
    L3Cache.processRequest();
    L2Cache.processRequest();
    L1Cache.processRequest();
    

}










// Flush all caches (L1, L2, L3)
void CacheManager::flushAllCaches()
{
    L1Cache.flush(); // Flush L1 cache
    L2Cache.flush(); // Flush L2 cache
    L3Cache.flush(); // Flush L3 cache
}

// Invalidate all caches (L1, L2, L3)
void CacheManager::invalidateAllCaches()
{
    L1Cache.invalidateAll(); // Invalidate all lines in L1 cache
    L2Cache.invalidateAll(); // Invalidate all lines in L2 cache
    L3Cache.invalidateAll(); // Invalidate all lines in L3 cache
}

// Function to print the state of all caches for debugging purposes
void CacheManager::printCacheState() const
{
    debugLog("Cache Manager State:");
    L1Cache.printCacheState(); // Print L1 cache state
    L2Cache.printCacheState(); // Print L2 cache state
    L3Cache.printCacheState(); // Print L3 cache state
}
    */

