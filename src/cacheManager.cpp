#include "cacheManager.hpp"
#include "memory.hpp"
#include <iostream>
#include "cpu.hpp"
#include <random>
#include "bus.hpp"
#include "eventLog.hpp"


LRUReplacementPolicy::LRUReplacementPolicy(const uint32_t& numSets, const uint8_t& associativity)
{
    lruStates.reserve(numSets); // Reserve space for the LRU states based on the number of sets
    for (uint32_t i = 0; i < numSets; ++i)
    {
        initializeSet(associativity); // Initialize the LRU state for each cache set based on the number of sets and associativity
    }
}

void LRUReplacementPolicy::initializeSet(uint8_t associativity)
{
    LRUState state;
    for (uint8_t i = 0; i < associativity; ++i)
    {
        state.queue.push(i); // Initialize the queue with line indices based on the associativity of the cache set
    }
    lruStates.push_back(state); // Add the LRU state for the cache set to the LRU queue
}

uint8_t LRUReplacementPolicy::selectLineToReplace(CacheSet& set)
{
    DEBUG_LOG(debugLog("selezione linea"));
    LRUState& state = lruStates[set.setIndex];

    for(uint8_t i = 0; i < set.lines.size(); ++i)
    {
        if (!set.lines[i].valid) // If there is an invalid line in the set, return its index for replacement
        {
            return i;
        }
    }

    return state.queue.getLastRecent(); // Select the line to replace based on the LRU queue for the cache set

}

void LRUReplacementPolicy::updateOnAccess(CacheSet& set, uint8_t lineIndex)  
{
    lruStates[set.setIndex].queue.touch(lineIndex); // Update the LRU queue on cache access to reflect the most recently used line
}

void LRUReplacementPolicy::onLineLoaded(CacheSet& set, uint8_t lineIndex) 
{
    lruStates[set.setIndex].queue.touch(lineIndex); // Update the LRU queue when a line is loaded into the cache to reflect the most recently used line

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
        bits[parent] = (node == 2 * parent + 1) ? 0 : 1; // Update the bit at the parent node to indicate the direction taken (1 for right, 0 for left) and move up to the parent node
        
        node = parent;
    }
}


PLRUReplacementPolicy::PLRUReplacementPolicy(const uint32_t& numSets, const uint8_t& associativity)
{
    plruTrees.reserve(numSets); // Reserve space for the PLRU trees based on the number of sets
    for (uint32_t i = 0; i < numSets; ++i)
    {
        initializeSet(associativity); // Initialize the PLRU state for each cache set based on the number of sets and associativity
    }
}

void PLRUReplacementPolicy::initializeSet(uint8_t associativity)
{
    PLRUTree tree(associativity); // Create a PLRU tree for the cache set based on the associativity
    for (uint8_t i = 0; i < associativity; ++i)
    {
        tree.updateLine(i); // Initialize the PLRU tree by updating the lines based on the associativity
    }
    plruTrees.push_back(tree); // Add the initialized PLRU tree to the vector
}

uint8_t PLRUReplacementPolicy::selectLineToReplace(CacheSet& set)
{
    auto& tree = plruTrees[set.setIndex]; // Get the PLRU tree for the cache set
    for (uint8_t i = 0; i < set.lines.size(); ++i)
    {
        if (!set.lines[i].valid) // If there is an invalid line in the set, return its index for replacement
        {
            return i;
        }
    }

    return tree.selectLineToReplace(); // Select the line to replace based on the PLRU tree for the cache set
}

void PLRUReplacementPolicy::updateOnAccess(CacheSet& set, uint8_t lineIndex)
{
    auto& tree = plruTrees[set.setIndex]; // Get the PLRU tree for the cache set
    tree.updateLine(lineIndex); // Update the PLRU tree on cache access to reflect the most recently used line
}

void PLRUReplacementPolicy::onLineLoaded(CacheSet& set, uint8_t lineIndex)
{
    auto& tree = plruTrees[set.setIndex]; // Get the PLRU tree for the cache set
    tree.updateLine(lineIndex); // Update the PLRU tree when a line is loaded into the cache to reflect the most recently used line
}


uint8_t RandomReplacementPolicy::selectLineToReplace(CacheSet& set)
{
    
    
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

int8_t CacheStorage::findLineIndex(uint8_t setIndex, uint64_t tag)
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
    int8_t lineIndex = findLineIndex(addressInfo.setIndex, addressInfo.tag); // Find the line index based on the set index and tag from the address information

    if (lineIndex < 0) [[unlikely]] // If no matching line is found, return without invalidating
        return;

    invalidateLine(addressInfo.setIndex, lineIndex); // Invalidate the specific cache line based on the set index and line index
}

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

void CacheStorage::loadLine(uint8_t setIndex, const CacheLine& line, uint8_t lineIndex)
{
    CacheSet& set = sets[setIndex]; // Get the cache set based on the set index
    CacheLine& targetLine = set.lines[lineIndex]; // Get the target cache line based on the line index

    targetLine.data = line.data; // Load the data into the target cache line
    targetLine.tag = line.tag; // Set the tag for the target cache line
    targetLine.valid = true; // Mark the target cache line as valid
    targetLine.dirty = false; // Mark the target cache line as not dirty since it's being loaded with new data

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RequestScheduler::processRequests()
{
    for (auto it = pendingRequests.begin(); it != pendingRequests.end();)
    {
        if (it->state == RequestState::WAITING_LATENCY) // Check if the request is in WAITING state
        {
            --(it->remainingLatency); // Decrement the remaining latency for the request( IT CANT BE ZERO)

            if (it->remainingLatency == 0) // If the request is ready to be processed
            {
                it->state = RequestState::READY_TO_PROCESS; // Update the state to READY_TO_PROCESS
                DEBUG_LOG(debugLog("n di richeiste: " + std::to_string(pendingRequests.size())));
                cacheControllerCallback(CallbackContext, *it); // Call the callback function to process the request using the provided context
                it = pendingRequests.erase(it); // Remove the request from the pending requests vector after processing
                DEBUG_LOG(debugLog("n di richeiste dopo esecuzione: " + std::to_string(pendingRequests.size())));

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

void RequestScheduler::scheduleRequest(CacheRequest&& request)
{
    DEBUG_LOG(debugLog("richiesta inserita"));
    uint8_t latency = ((request.type == (RequestType::FILL)) || (request.type == (RequestType::READ_AFTER_FILL)) || (request.type == (RequestType::WRITE_AFTER_FILL))) ? this->fillLatency : this->latency; 
    pendingRequests.emplace_back(std::move(request), latency, RequestState::WAITING_LATENCY); // Add a new request to the pending requests vector with the specified latency
}

void RequestScheduler::schedulePendingRequest(PendingRequest&& pendingRequest)
{   
    pendingRequest.state = RequestState::WAITING_LATENCY; // Set the state of the pending request to WAITING_LATENCY before scheduling
    pendingRequest.remainingLatency = ((pendingRequest.request.type == (RequestType::FILL)) || (pendingRequest.request.type == (RequestType::READ_AFTER_FILL)) || (pendingRequest.request.type == (RequestType::WRITE_AFTER_FILL)) ) ? this->fillLatency : this->latency; 
    pendingRequests.push_back(std::move(pendingRequest)); // Add a pending request to the pending requests vector for processing
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


AddressInfo CacheController::decodeAddress(uint64_t address)
{
    uint8_t offsetBits = std::countr_zero(CACHE_LINE_SIZE); // Calculate the number of bits for the offset based on the cache line size
    uint8_t setIndexBits = std::countr_zero(numSets); // Calculate the number of bits for the set index based on the number of cache sets

    uint64_t offset = address & ((1ULL << offsetBits) - 1); // Extract the offset from the address using bitwise operations
    uint64_t setIndex = (address >> offsetBits) & ((1ULL << setIndexBits) - 1); // Extract the set index from the address using bitwise operations
    uint64_t tag = address >> (offsetBits + setIndexBits); // Extract the tag from the address by shifting right by the total number of bits for offset and set index

    return AddressInfo(address, setIndex, tag, offset); // Return an AddressInfo struct containing the decoded address information
}

void CacheController::handleRequest(PendingRequest& request)
{

    CacheRequest& cacheRequest = request.request; // Get a reference to the cache request from the pending request
     
    DEBUG_LOG(debugLog("processing request "+ std::to_string(request.request.requestID)));
    // Handle the incoming cache request and coordinate the cache operations based on the request type and address information
    // This function will involve looking up the cache, determining hits or misses, and performing the necessary actions (e.g., loading data, writing back dirty lines, etc.)
    AddressInfo addressInfo = decodeAddress(cacheRequest.address); // Decode the memory address to get set index, tag, and offset information
    CacheEventPayload payload = CacheEventPayload(addressInfo, AddressInfo(0, 0, 0, 0), cacheRequest);

    if(cacheRequest.type == RequestType::FILL) // If the request is a FILL request and it's a hit, we need to fill the cache line with data from the next level or memory
    {
        DEBUG_LOG(debugLog("fill request ricevuta"));
        CacheEventPayload payloadFill = CacheEventPayload(addressInfo, cacheRequest, &(request.line)); // Create a payload for the fill event with the address information and cache request
        cacheEventHandler.triggerHandleRequestEvent(EventHandlerCacheEventType::CACHE_FILL, payloadFill); // Trigger a cache fill event for logging or debugging purposes
        return;
    }
    else if (cacheRequest.type == RequestType::READ_AFTER_FILL || cacheRequest.type == RequestType::WRITE_AFTER_FILL)
    {
        DEBUG_LOG(debugLog("cache hit"));
        cacheEventHandler.triggerHandleRequestEvent(EventHandlerCacheEventType::CACHE_HIT, payload); // Trigger a cache hit event for logging or debugging purposes

        return;

    }

    LookUpResult result = lookupCache(addressInfo, cacheRequest.dataType); // Perform a cache lookup based on the decoded address information to determine if it's a hit or miss

    switch (result)
    {
        case LookUpResult::HIT:
        DEBUG_LOG(debugLog("hit cache"));
            cacheEventHandler.triggerHandleRequestEvent(EventHandlerCacheEventType::CACHE_HIT, payload); // Trigger a cache hit event for logging or debugging purposes
            break;
        case LookUpResult::MISS:
        DEBUG_LOG(debugLog("miss_cache"));
            cacheEventHandler.triggerHandleRequestEvent(EventHandlerCacheEventType::CACHE_MISS, payload); // Trigger a cache miss event for logging or debugging purposes
            break;
        case LookUpResult::HIT_CROSS_LINES:
        DEBUG_LOG(debugLog("hit cross cache"));
            payload.addressInfo2 = decodeAddress(cacheRequest.address + (CACHE_LINE_SIZE - addressInfo.offset)); // Decode the address of the second cache line for cross-line access
            cacheEventHandler.triggerHandleRequestEvent(EventHandlerCacheEventType::CACHE_HIT_CROSS_LINES, payload); // Trigger a cache hit cross lines event for logging or debugging purposes
            break;
        case LookUpResult::ERROR:
        DEBUG_LOG(debugLog("error cache"));
            break;
    }

    return;
}

LookUpResult CacheController::lookupCache(const AddressInfo& addressInfo, TypeofData dataType)
{
    // Perform a cache lookup based on the decoded address information to determine if it's a hit or miss
    LookUpResult result = LookUpResult::ERROR; // Initialize the result to error
    CacheLookupPayload payload = CacheLookupPayload(addressInfo, dataType); // Create a payload for the cache lookup event with the address information and type of data being accessed
    result = cacheEventHandler.triggerLookupCacheEvent(payload); // Trigger a cache lookup event for logging or debugging purposes
    return result;
}

void CacheController::CallBackWrapperScheduler(void* context, PendingRequest& request)
{
    DEBUG_LOG(debugLog("callback scheduler"));
    CacheController* controller = static_cast<CacheController*>(context); // Cast the context pointer to a CacheController pointer
    controller->handleRequest(request); // Call the handleRequest function to process the cache request
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CacheLevel::CacheLevel(uint32_t size, uint8_t associativity, uint8_t latency, uint8_t fillLatency, Bus& bus, CacheLevel *nextLevel, CacheLevel* parentLevel, CacheLevelType cacheType)
    : cacheSize(size), numSets(size / (associativity * CACHE_LINE_SIZE)), bus(bus), nextLevel(nextLevel), parentLevel(parentLevel),associativity(associativity), latencyCycles(latency), latencyFill(fillLatency), type(cacheType),
    storage(numSets, associativity), controller(eventHandler, numSets), scheduler(latencyCycles, fillLatency, &CacheController::CallBackWrapperScheduler, static_cast<void*>(&this->controller))
{
    DEBUG_LOG(debugLog("inizializzazione cache level"));
    replacementPolicy = std::make_unique<LRUReplacementPolicy>(numSets, associativity);
    writePolicy = std::make_unique<WriteThroughAllocate>();

    eventHandler.setContext(static_cast<void*>(this)); // Set the context pointer for the cache controller to access its own state or data when processing requests

    eventHandler.registerHandleRequestEvent(EventHandlerCacheEventType::CACHE_HIT, &CacheLevel::onHitWrapper); // Register the onHitWrapper function to handle cache hit events
    eventHandler.registerHandleRequestEvent(EventHandlerCacheEventType::CACHE_MISS, &CacheLevel::onMissWrapper); // Register the onMissWrapper function to handle cache miss events
    eventHandler.registerHandleRequestEvent(EventHandlerCacheEventType::CACHE_HIT_CROSS_LINES, &CacheLevel::onHitCrossLinesWrapper); // Register the onHitCrossLinesWrapper function to handle cache hit cross lines events
    eventHandler.registerHandleRequestEvent(EventHandlerCacheEventType::CACHE_FILL, &CacheLevel::onFillWrapper); // Register the onFillWrapper function to handle filling a cache line with data from the next level or memory based on the cache request and address
    eventHandler.registerLookupCacheEvent(&CacheLevel::LookupWrapper); // Register the LookupWrapper function to handle cache lookup events

    
    DEBUG_LOG(debugLog("inizializzazione cache level completata"));

}


void CacheLevel::onHitWrapper(void* context, CacheEventPayload& payload)
{
    CacheLevel* cacheLevel = static_cast<CacheLevel*>(context); // Cast the context pointer to a CacheLevel pointer
    cacheLevel->onHit(payload.addressInfo1, payload.request); // Call the onHit function to handle cache hit events
}

void CacheLevel::onMissWrapper(void* context, CacheEventPayload& payload)
{
    CacheLevel* cacheLevel = static_cast<CacheLevel*>(context); // Cast the context pointer to a CacheLevel pointer
    cacheLevel->onMiss(payload.request); // Call the onMiss function to handle cache miss events
}

void CacheLevel::onHitCrossLinesWrapper(void* context, CacheEventPayload& payload)
{
    CacheLevel* cacheLevel = static_cast<CacheLevel*>(context); // Cast the context pointer to a CacheLevel pointer
    cacheLevel->onHitCrossLines(payload.addressInfo1, payload.addressInfo2, payload.request); // Call the onHitCrossLines function to handle cache hit cross lines events
}

void CacheLevel::onFillWrapper(void* context, CacheEventPayload& payload)
{
    CacheLevel* cacheLevel = static_cast<CacheLevel*>(context); // Cast the context pointer to a CacheLevel pointer
    cacheLevel->onFill(payload.addressInfo1, payload.request, *(payload.line)); // Call the onFill function to handle filling a cache line with data from the next level or memory based on the cache request and address information
}

LookUpResult CacheLevel::LookupWrapper(void* context, CacheLookupPayload& payload)
{
    DEBUG_LOG(debugLog("lookup cache"));
    CacheLevel* cacheLevel = static_cast<CacheLevel*>(context); // Cast the context pointer to a CacheLevel pointer
    return cacheLevel->lookupCache(payload.addressInfo, payload.dataType); // Call the lookupCache function to perform the cache lookup and return the result
}

void CacheLevel::execute_operation()
{
    #ifdef DEBUG
    std::string cache;


    switch (type)
    {
    case CacheLevelType::L1:
        cache = "L1";
        break;
    case CacheLevelType::L2:
        cache = "L2";
        break;
    case CacheLevelType::L3:
        cache = "L3";
        break;
    
    default:
        break;
    }
    #endif

    DEBUG_LOG(debugLog("executing operation for cache" + cache));
    scheduler.processRequests(); // Process pending cache requests based on their scheduled latency
}

void CacheLevel::scheduleRequest(CacheRequest&& request)
{
    scheduler.scheduleRequest(std::move(request)); // Schedule a new cache request for processing by the cache scheduler
}

void CacheLevel::schedulePendingRequest(PendingRequest&& pendingRequest)
{
    scheduler.schedulePendingRequest(std::move(pendingRequest)); // Schedule a pending request for processing by the cache scheduler
}

LookUpResult CacheLevel::lookupCache(const AddressInfo& addressInfo, TypeofData dataType)
{
    CacheLine* line = storage.findLine(addressInfo.setIndex, addressInfo.tag); // Find the cache line based on the set index and tag from the address information
    CacheLine* line2 = nullptr; // Initialize a pointer for the second cache line in case of cross-line access
    Result result{};

    if (line != nullptr) // If a matching cache line is found
    {
        if (addressInfo.offset + uint8_t(dataType) <= CACHE_LINE_SIZE) // Check if the access is within a single cache line
        {
            result.success = true; // Set the result to success for a hit within a single cache line
            result.errorInfo.source = getComponentTypeFromCacheLevelType(type); // Set the source of the error information based on the cache level type
            result.errorInfo.event = EventType::CACHE_HIT;
            result.errorInfo.error = ErrorType::NONE;
            EventLog::getInstance().pushCacheDataLogEntry(std::move(result), nullptr, nullptr, addressInfo, AddressInfo()); // Log the cache hit event with the address and data from the cache line
            return LookUpResult::HIT; // Return HIT if it's a hit within a single cache line
        }
        else
        {
            AddressInfo addressInfo2 = controller.decodeAddress(addressInfo.address + (CACHE_LINE_SIZE - addressInfo.offset)); // Decode the address of the second cache line for cross-line access
            line2 = storage.findLine(addressInfo2.setIndex, addressInfo2.tag); // Find the second cache line for cross-line access based on the next set index and the same tag
            if (line2 != nullptr) // If a matching second cache line is found for cross-line access
            {
                result.success = true; // Set the result to success for a hit across two cache lines
                result.errorInfo.source = getComponentTypeFromCacheLevelType(type); // Set the source of the error information based on the cache level type
                result.errorInfo.event = EventType::CACHE_HIT;
                result.errorInfo.error = ErrorType::NONE;
                EventLog::getInstance().pushCacheDataLogEntry(std::move(result), static_cast<LineData*>(nullptr), static_cast<LineData*>(nullptr), addressInfo, addressInfo2); // Log the cache hit event with the address and data from both cache lines
                return LookUpResult::HIT_CROSS_LINES; // Return HIT_CROSS_LINES if it's a hit that spans across two cache lines
            }
            result.success = false; // Set the result to failure for a miss across two cache lines
            result.errorInfo.source = getComponentTypeFromCacheLevelType(type); // Set the source of the error information based on the cache level type
            result.errorInfo.event = EventType::CACHE_MISS;
            result.errorInfo.error = ErrorType::OUT_OF_BOUNDS;
            EventLog::getInstance().pushCacheDataLogEntry(std::move(result), static_cast<LineData*>(nullptr), static_cast<LineData*>(nullptr), addressInfo, AddressInfo()); // Log the cache miss event with the address and data from the first cache line
            return LookUpResult::MISS; // Return MISS if the second cache line for cross-line access is not found, indicating a miss across two cache lines
        }
    }
    else
    {
        result.success = false; // Set the result to failure for a miss within a single cache line
        result.errorInfo.source = getComponentTypeFromCacheLevelType(type); // Set the source of the error information based on the cache level type
        result.errorInfo.event = EventType::CACHE_MISS;
        result.errorInfo.error = ErrorType::OUT_OF_BOUNDS;
        EventLog::getInstance().pushCacheDataLogEntry(std::move(result), static_cast<LineData*>(nullptr), static_cast<LineData*>(nullptr), addressInfo, AddressInfo()); // Log the cache miss event with the address and data from the first cache line
        return LookUpResult::MISS; // Return MISS if no matching cache line is found
    }
}

void CacheLevel::onHit(const AddressInfo& addressInfo, CacheRequest& request)
{
    if(type != CacheLevelType::L1)
    {   
        if(!writePolicy->writeAllocateOnMiss() && request.type == RequestType::WRITE)
        {
            writeSingleLine(addressInfo, request); // Call the writeCrossLines function to handle cache write operations that span across two cache lines based on the address information and cache request
            if(int8_t lineIndex = storage.findLineIndex(addressInfo.setIndex, addressInfo.tag); lineIndex >= 0) // If a matching cache line is found based on the set index and tag from the address information
            {
                replacementPolicy->updateOnAccess(storage.getSet(addressInfo.setIndex), uint8_t(lineIndex)); // Update the replacement policy on cache access to reflect the most recently used line
            }
            return;

        }

        int8_t lineIndex = storage.findLineIndex(addressInfo.setIndex, addressInfo.tag);
        if (lineIndex < 0)
        {
            //error Hendling
        }
        replacementPolicy->updateOnAccess(storage.getSet(addressInfo.setIndex), lineIndex);
        CacheLine* line = storage.findLine(addressInfo.setIndex, addressInfo.tag);

        auto MakeFillPendingRequest = [&]()
        {
            CacheRequest fillRequest = CacheRequest();
            fillRequest.address = addressInfo.address - addressInfo.offset; //adress start line
            fillRequest.dataType = TypeofData::ARRAY_64B;
            fillRequest.type = RequestType::FILL;
            PendingRequest fillPendingRequest = PendingRequest();
            fillPendingRequest.line = *line;
            fillPendingRequest.request = std::move(fillRequest);
            
            return fillPendingRequest;
        };

        CacheLevel* topLevel = parentLevel;

        while (topLevel != nullptr)
        {
            topLevel->schedulePendingRequest(MakeFillPendingRequest());
            topLevel = topLevel->parentLevel;
        }
        
        //recreating the original request for L1
        PendingRequest originalPendingRequest = PendingRequest();
        originalPendingRequest.request = request;

        originalPendingRequest.request.type =  originalPendingRequest.request.type == RequestType::READ ? RequestType::READ_AFTER_FILL : RequestType::WRITE_AFTER_FILL;
        bus.getCPU().getCacheManager().getL1Cache().schedulePendingRequest(std::move(originalPendingRequest));

        return;

    }
    switch (request.type)
    {
    case RequestType::READ:
        readSingleLine(addressInfo, request); // Call the read function to handle cache read operations based on
        DEBUG_LOG(debugLog("read single line"));
        if(int8_t lineIndex = storage.findLineIndex(addressInfo.setIndex, addressInfo.tag); lineIndex >= 0) // If a matching cache line is found based on the set index and tag from the address information
        {
            replacementPolicy->updateOnAccess(storage.getSet(addressInfo.setIndex), uint8_t(lineIndex)); // Update the replacement policy on cache access to reflect the most recently used line
        }
        break;

    case RequestType::WRITE:
        writeSingleLine(addressInfo, request); // Call the write function to handle cache write operations based on the address information and cache request
        if(int8_t lineIndex = storage.findLineIndex(addressInfo.setIndex, addressInfo.tag); lineIndex >= 0) // If a matching cache line is found based on the set index and tag from the address information
        {
            replacementPolicy->updateOnAccess(storage.getSet(addressInfo.setIndex), uint8_t(lineIndex)); // Update the replacement policy on cache access to reflect the most recently used line
        }
        break;
    
    
    default:
        break;
    }

}

void CacheLevel::onHitCrossLines(const AddressInfo& addressInfo1, const AddressInfo& addressInfo2, CacheRequest& request)
{
     if(type != CacheLevelType::L1)
    {   
        if(!writePolicy->writeAllocateOnMiss() && request.type == RequestType::WRITE)
        {
            writeCrossLines(addressInfo1, addressInfo2, request); // Call the writeCrossLines function to handle cache write operations that span across two cache lines based on the address information and cache request
            if(int8_t lineIndex = storage.findLineIndex(addressInfo1.setIndex, addressInfo1.tag); lineIndex >= 0) // If a matching cache line is found based on the set index and tag from the address information
            {
                replacementPolicy->updateOnAccess(storage.getSet(addressInfo1.setIndex), uint8_t(lineIndex)); // Update the replacement policy on cache access to reflect the most recently used line
            }
            if(int8_t lineIndex = storage.findLineIndex(addressInfo2.setIndex, addressInfo2.tag); lineIndex >= 0) // If a matching cache line is found based on the set index and tag from the address information
            {
                replacementPolicy->updateOnAccess(storage.getSet(addressInfo2.setIndex), uint8_t(lineIndex)); // Update the replacement policy on cache access to reflect the most recently used line
            }
            return;

        }

        int8_t lineIndex1 = storage.findLineIndex(addressInfo1.setIndex, addressInfo1.tag);
        if (lineIndex1 < 0)
        {
            //error Hendling
        }
        replacementPolicy->updateOnAccess(storage.getSet(addressInfo1.setIndex), lineIndex1);
        CacheLine* line1 = storage.findLine(addressInfo1.setIndex, addressInfo1.tag);

        auto MakeFillPendingRequest1 = [&]()
        {
            CacheRequest fillRequest = CacheRequest();
            fillRequest.address = addressInfo1.address - addressInfo1.offset; //adress start line
            fillRequest.dataType = TypeofData::ARRAY_64B;
            fillRequest.type = RequestType::FILL;
            PendingRequest fillPendingRequest = PendingRequest();
            fillPendingRequest.line = *line1;
            fillPendingRequest.request = std::move(fillRequest);
            
            return fillPendingRequest;
        };

        int8_t lineIndex2 = storage.findLineIndex(addressInfo2.setIndex, addressInfo2.tag);
        if (lineIndex2 < 0)
        {
            //error Hendling
        }
        replacementPolicy->updateOnAccess(storage.getSet(addressInfo2.setIndex), lineIndex2);
        CacheLine* line2 = storage.findLine(addressInfo2.setIndex, addressInfo2.tag);

        auto MakeFillPendingRequest2 = [&]()
        {
            CacheRequest fillRequest = CacheRequest();
            fillRequest.address = addressInfo2.address - addressInfo2.offset; //adress start line
            fillRequest.dataType = TypeofData::ARRAY_64B;
            fillRequest.type = RequestType::FILL;
            PendingRequest fillPendingRequest = PendingRequest();
            fillPendingRequest.line = *line2;
            fillPendingRequest.request = std::move(fillRequest);
            
            return fillPendingRequest;
        };




        CacheLevel* topLevel = parentLevel;

        while (topLevel != nullptr)
        {
            topLevel->schedulePendingRequest(MakeFillPendingRequest1());
            topLevel->schedulePendingRequest(MakeFillPendingRequest2());
            topLevel = topLevel->parentLevel;
        }
        
        //recreating the original request for L1
        PendingRequest originalPendingRequest = PendingRequest();
        originalPendingRequest.request = request;

        originalPendingRequest.request.type =  originalPendingRequest.request.type == RequestType::READ ? RequestType::READ_AFTER_FILL : RequestType::WRITE_AFTER_FILL;
        bus.getCPU().getCacheManager().getL1Cache().schedulePendingRequest(std::move(originalPendingRequest));

        return;


    }

    // Handle cache hit events that span across two cache lines based on the address information and cache request
    // This function will involve reading or writing data that spans across two cache lines and updating the replacement policy accordingly
    switch (request.type)
    {
    case RequestType::READ:
        readCrossLines(addressInfo1, addressInfo2, request); // Call the readCrossLines function to handle cache read operations that span across two cache lines based on the address information and cache request
        if(int8_t lineIndex = storage.findLineIndex(addressInfo1.setIndex, addressInfo1.tag); lineIndex >= 0) // If a matching cache line is found based on the set index and tag from the address information
        {
            replacementPolicy->updateOnAccess(storage.getSet(addressInfo1.setIndex), uint8_t(lineIndex)); // Update the replacement policy on cache access to reflect the most recently used line
        }
        if(int8_t lineIndex = storage.findLineIndex(addressInfo2.setIndex, addressInfo2.tag); lineIndex >= 0) // If a matching cache line is found based on the set index and tag from the address information
        {
            replacementPolicy->updateOnAccess(storage.getSet(addressInfo2.setIndex), uint8_t(lineIndex)); // Update the replacement policy on cache access to reflect the most recently used line
        }
        break;

    case RequestType::WRITE:
        writeCrossLines(addressInfo1, addressInfo2, request); // Call the writeCrossLines function to handle cache write operations that span across two cache lines based on the address information and cache request
        if(int8_t lineIndex = storage.findLineIndex(addressInfo1.setIndex, addressInfo1.tag); lineIndex >= 0) // If a matching cache line is found based on the set index and tag from the address information
        {
            replacementPolicy->updateOnAccess(storage.getSet(addressInfo1.setIndex), uint8_t(lineIndex)); // Update the replacement policy on cache access to reflect the most recently used line
        }
        if(int8_t lineIndex = storage.findLineIndex(addressInfo2.setIndex, addressInfo2.tag); lineIndex >= 0) // If a matching cache line is found based on the set index and tag from the address information
        {
            replacementPolicy->updateOnAccess(storage.getSet(addressInfo2.setIndex), uint8_t(lineIndex)); // Update the replacement policy on cache access to reflect the most recently used line
        }
        break;
    
    default:
        break;
    }
}

void CacheLevel::onMiss(CacheRequest& request)
{

    DEBUG_LOG(debugLog("gestione miss"));
    if(type != CacheLevelType::L3) // If there is a next cache level (L2 or L3)
    {
        DEBUG_LOG(debugLog("invio a livello successivo"));
        nextLevel->scheduleRequest(std::move(request)); // Schedule the request to be processed by the next cache level
    }
    else
    {
        DEBUG_LOG(debugLog("invio ram"));
        if(request.type == RequestType::WRITE)
            request.type = RequestType::READ_MEMORY_FOR_WRITE_MISS;

        bus.getCPU().getCacheManager().getMemoryScheduler().scheduleMemoryRequest(std::move(request)); // Schedule the request to be processed by the memory scheduler if there is no next cache level
    }
}


void CacheLevel::propagateWriteToNextLevel(CacheRequest& request, bool propagateCallback)
{   
    auto writeRequest = CacheRequest(); // Create a new cache request based on the original request to propagate the write operation to the next cache level or memory
    writeRequest.type = RequestType::WRITE; // Set the request type to WRITE for the new cache request
    writeRequest.address = request.address; // Set the address for the new cache request to
    writeRequest.dataType = request.dataType; // Set the data type for the new cache request to match the original request
    writeRequest.data = request.data; // Set the data for the new cache request to match the original request
    writeRequest.callback = propagateCallback ? request.callback : nullptr; // Set the callback for the new cache request based on the write policy's write allocate behavior(if miss we have to propagate the callback to the next level, otherwise we don't need to propagate the callback)

   
    if(nextLevel != nullptr) // If there is a next cache level (L2 or L3)
    {
        nextLevel->scheduleRequest(std::move(writeRequest)); // Schedule the write request to be processed by the next cache level
    }
    else
    {
        bus.getCPU().getCacheManager().getMemoryScheduler().scheduleMemoryRequest(std::move(writeRequest)); // Schedule the write request to be processed by the memory scheduler if there is no next cache level
    }
}

void CacheLevel::readSingleLine(const AddressInfo& addressInfo, CacheRequest& request)
{
    MaxCPUInstructionLength response {};
    bool success = false; // Initialize a variable to track the success status of the request
    CacheLine* line = storage.findLine(addressInfo.setIndex, addressInfo.tag); // Find the cache line based on the set index and tag from the address informatio
    Result result{};
    if (line != nullptr) // If a matching cache line is found
    {

        std::memcpy(response.data(), line->data.data() + addressInfo.offset, size_t(request.dataType)); // Copy the requested data from the cache line to the request's data buffer based on the offset and data type
        success = true; // Indicate that the request was successful if a matching cache line is found and the data is extracted successfully
        result.success = true; // Set the result to success for a hit within a single cache line
        result.errorInfo.source = getComponentTypeFromCacheLevelType(type); // Set the source of the error information based on the cache level type
        result.errorInfo.event = EventType::CACHE_READ;
        result.errorInfo.error = ErrorType::NONE;
        EventLog::getInstance().pushCacheDataLogEntry(std::move(result), &(line->data), static_cast<LineData*>(nullptr), addressInfo, AddressInfo()); // Log the cache hit event with the address and data from the cache line
    }
    else
    {
        result.success = false; // Set the result to failure for a miss within a single cache line
        result.errorInfo.source = getComponentTypeFromCacheLevelType(type); // Set the source of the error information based on the cache level type
        result.errorInfo.event = EventType::CACHE_READ;
        result.errorInfo.error = ErrorType::OUT_OF_BOUNDS;
        EventLog::getInstance().pushCacheDataLogEntry(std::move(result), static_cast<LineData*>(nullptr), static_cast<LineData*>(nullptr), AddressInfo(), AddressInfo());
           
    }   

    bus.getCPU().cacheResponseQueue[request.requestID] =std::move(response); // Add the completed request to the CPU's cache response queue for further processing by the CPU
    
    if(success && request.callback) // If the request was successful and there is a callback function defined for the request
    {
        request.callback(request.callbackContext);
    }
}

void CacheLevel::readCrossLines(const AddressInfo& addressInfo1, const AddressInfo& addressInfo2, CacheRequest& request)
{
    MaxCPUInstructionLength response {};
    CacheLine* line1 = storage.findLine(addressInfo1.setIndex, addressInfo1.tag); // Find the first cache line based on the set index and tag from the address information
    CacheLine* line2 = storage.findLine(addressInfo2.setIndex, addressInfo2.tag); // Find the second cache line based on the set index and tag from the address information
    bool success = false; // Initialize a variable to track the success status of the request
    Result result{};

    if(line1 != nullptr && line2 != nullptr) // If both cache lines are found for the cross-line access
    {
        std::memcpy(response.data(), line1->data.data() + addressInfo1.offset, (CACHE_LINE_SIZE - addressInfo1.offset)); // Copy the requested data from the first cache line to the data buffer based on the offset and data type
        std::memcpy(response.data() + (CACHE_LINE_SIZE - addressInfo1.offset), line2->data.data(), size_t(request.dataType) - (CACHE_LINE_SIZE - addressInfo1.offset)); // Copy the remaining data from the second cache line to the data buffer based on the offset and data type
        success = true; // Indicate that the request was successful if both cache lines are found and the data is extracted successfully
        result.success = true; // Set the result to success for a hit across two cache lines
        result.errorInfo.source = getComponentTypeFromCacheLevelType(type); // Set the source of the error information based on the cache level type
        result.errorInfo.event = EventType::CACHE_READ;
        result.errorInfo.error = ErrorType::NONE;
        EventLog::getInstance().pushCacheDataLogEntry(std::move(result), &(line1->data), &(line2->data), addressInfo1, addressInfo2);
    }
    else
    {
        result.success = false; // Set the result to failure for a miss across two cache lines
        result.errorInfo.source = getComponentTypeFromCacheLevelType(type); // Set the source of the error information based on the cache level type
        result.errorInfo.event = EventType::CACHE_READ;
        result.errorInfo.error = ErrorType::OUT_OF_BOUNDS;
        EventLog::getInstance().pushCacheDataLogEntry(std::move(result), static_cast<LineData*>(nullptr), static_cast<LineData*>(nullptr), AddressInfo(), AddressInfo());
    
    }

    bus.getCPU().cacheResponseQueue[request.requestID] =std::move(response); // Add the completed request to the CPU's cache response queue for further processing by the CPU

    if(success && request.callback) // If the request was successful and there is a callback function defined for the request
    {
        request.callback(request.callbackContext);
    }
}

void CacheLevel::writeSingleLine(const AddressInfo& addressInfo, CacheRequest& request)
{
    CacheLine* line = storage.findLine(addressInfo.setIndex, addressInfo.tag); // Find the cache line based on the set index and tag from the address information
    MaxCPUInstructionLength response {};
    bool success = false; // Initialize a variable to track the success status of the request
    Result result{};

    if (line != nullptr) // If a matching cache line is found
    {
        std::memcpy(line->data.data() + addressInfo.offset, request.data.data(), size_t(request.dataType)); // Copy the data from the request's data buffer to the cache line based on the offset and data type
        
        success = true; // Indicate that the request was successful if a matching cache line is found and the data is written successfully
        result.success = true; // Set the result to success for a hit within a single cache line
        result.errorInfo.source = getComponentTypeFromCacheLevelType(type); // Set the source of the error information based on the cache level type
        result.errorInfo.event = EventType::CACHE_WRITE;
        result.errorInfo.error = ErrorType::NONE;
        EventLog::getInstance().pushCacheDataLogEntry(std::move(result), &(line->data), static_cast<LineData*>(nullptr), addressInfo, AddressInfo()); // Log the cache hit event with the address and data from the cache line

        if(writePolicy->writeThroughOnHit()) // If the write policy is set to write-through
        {
            line->dirty = false; // Mark the cache line as not dirty since the write is propagated to the next level or memory
            propagateWriteToNextLevel(request, false); // Propagate the write to the next cache level or memory if the write policy is write-through
        }
        else // If the write policy is set to write-back
        {
            line->dirty = true; // Mark the cache line as dirty since it has been modified
        }


    }
    else
    {
        success = false; // Indicate that the request was not successful if no matching cache line is found
        result.success = false; // Set the result to failure for a miss within a single cache line
        result.errorInfo.source = getComponentTypeFromCacheLevelType(type); // Set the source of the error information based on the cache level type
        result.errorInfo.event = EventType::CACHE_WRITE;
        result.errorInfo.error = ErrorType::OUT_OF_BOUNDS;
        EventLog::getInstance().pushCacheDataLogEntry(std::move(result), static_cast<LineData*>(nullptr), static_cast<LineData*>(nullptr), AddressInfo(), AddressInfo());
    }

    bus.getCPU().cacheResponseQueue[request.requestID] = std::move(response); // Add the completed request to the CPU's cache response queue for further processing by the CPU

    if(success && request.callback) // If the request was successful and there is a callback function defined for the request
    {
        request.callback(request.callbackContext);
    }
}

void CacheLevel::writeCrossLines(const AddressInfo& addressInfo1, const AddressInfo& addressInfo2, CacheRequest& request)
{
    CacheLine* line1 = storage.findLine(addressInfo1.setIndex, addressInfo1.tag); // Find the first cache line based on the set index and tag from the address information
    CacheLine* line2 = storage.findLine(addressInfo2.setIndex, addressInfo2.tag); // Find the second cache line based on the set index and tag from the address information
    MaxCPUInstructionLength response {};
    bool success = false; // Initialize a variable to track the success status of the request
    Result result{};

    if (line1 != nullptr && line2 != nullptr) // If both cache lines are found for the cross-line access
    {
        std::memcpy(line1->data.data() + addressInfo1.offset, request.data.data(), (CACHE_LINE_SIZE - addressInfo1.offset)); // Copy the data from the request's data buffer to the first cache line based on the offset and data type
        std::memcpy(line2->data.data(), request.data.data() + (CACHE_LINE_SIZE - addressInfo1.offset), size_t(request.dataType) - (CACHE_LINE_SIZE - addressInfo1.offset)); // Copy the remaining data from the request's data buffer to the second cache line based on the offset and data type

        success = true; // Indicate that the request was successful if both cache lines are found and the data is written successfully
        result.success = true; // Set the result to success for a hit across two cache lines
        result.errorInfo.source = getComponentTypeFromCacheLevelType(type); // Set the source of the error information based on the cache level type
        result.errorInfo.event = EventType::CACHE_WRITE;
        result.errorInfo.error = ErrorType::NONE;
        EventLog::getInstance().pushCacheDataLogEntry(std::move(result), &(line1->data), &(line2->data), addressInfo1, addressInfo2);

        if(writePolicy->writeThroughOnHit()) // If the write policy is set to write-through
        {
            line1->dirty = false; // Mark the first cache line as not dirty since the write is propagated to the next level or memory
            line2->dirty = false; // Mark the second cache line as not dirty since the write is propagated to the next level or memory
            propagateWriteToNextLevel(request, false); // Propagate the write to the next cache level or memory for both cache lines if the write policy is write-through
        }
        else // If the write policy is set to write-back
        {
            line1->dirty = true; // Mark the first cache line as dirty since it has been modified
            line2->dirty = true; // Mark the second cache line as dirty since it has been modified
        }

    }
    else
    {   
        result.success = false; // Set the result to failure for a miss across two cache lines
        result.errorInfo.source = getComponentTypeFromCacheLevelType(type); // Set the source of the error information based on the cache level type
        result.errorInfo.event = EventType::CACHE_WRITE;
        result.errorInfo.error = ErrorType::OUT_OF_BOUNDS;
        EventLog::getInstance().pushCacheDataLogEntry(std::move(result), static_cast<LineData*>(nullptr), static_cast<LineData*>(nullptr), AddressInfo(), AddressInfo());
    }


    bus.getCPU().cacheResponseQueue[request.requestID] = std::move(response); // Add the completed request to the CPU's cache response queue for further processing by the CPU

    if(success && request.callback) // If the request was successful and there is a callback function defined for the request
    {
        request.callback(request.callbackContext);
    }
}

void CacheLevel::onFill(const AddressInfo& addressInfo, [[maybe_unused]] CacheRequest& request, CacheLine& line)
{   
    Result resultFill{};

    DEBUG_LOG(debugLog("riempimento fill"));
    CacheSet& set = storage.getSet(addressInfo.setIndex);
    DEBUG_LOG(debugLog("set ottenuto"));
    uint8_t lineIndex = replacementPolicy->selectLineToReplace(storage.getSet(addressInfo.setIndex));
    DEBUG_LOG(debugLog("linea trovata"));
    CacheLine& victim =  set.lines[lineIndex];
    if(victim.valid && victim.dirty)
    {
        Result resultEviction{};
        uint8_t offsetBits = std::countr_zero(CACHE_LINE_SIZE); // Calculate the number of offset bits based on the cache line size using std::countr_zero to count trailing zeros
        uint8_t setIndexBits = std::countr_zero(numSets); // Calculate the number of set index bits based on the number of sets in the cache using std::countr_zero to count trailing zeros
        uint64_t victimAddress = (victim.tag << (offsetBits + setIndexBits)) | (addressInfo.setIndex << offsetBits);
        resultEviction.success = true;
        resultEviction.errorInfo.source = getComponentTypeFromCacheLevelType(type);
        resultEviction.errorInfo.event = EventType::CACHE_EVICT;
        resultEviction.errorInfo.error = ErrorType::NONE;
        EventLog::getInstance().pushCacheDataLogEntry(std::move(resultEviction), static_cast<LineData*>(nullptr), static_cast<LineData*>(nullptr), addressInfo, AddressInfo());
        onEviction(controller.decodeAddress(victimAddress),victim);
    }

   //no need to check dirty, because this mecanic is done when we create the fill request in others levels
    DEBUG_LOG(debugLog("CARICAMENTO LINEA"));
    storage.loadLine(addressInfo.setIndex, line, lineIndex); // Load the data into the cache line at the specified set index and tag using the storage's load function, which will handle the actual filling of the cache line with data from the lower level or memory based on the cache line provided in the request
    DEBUG_LOG(debugLog("RIGA CARICATA"));
    replacementPolicy->onLineLoaded(set, lineIndex);
    
    resultFill.success = true;
    resultFill.errorInfo.source = getComponentTypeFromCacheLevelType(type);
    resultFill.errorInfo.event = EventType::CACHE_FILL;
    resultFill.errorInfo.error = ErrorType::NONE;
    EventLog::getInstance().pushCacheDataLogEntry(std::move(resultFill), static_cast<LineData*>(nullptr), static_cast<LineData*>(nullptr), AddressInfo(), AddressInfo());

    // The actual filling of the cache line with data from the lower level or memory will be handled in the onHit or onMiss functions based on whether the line was found or not, and the replacement policy will be updated accordingly to reflect the most recently used line
}

void CacheLevel::onEviction(const AddressInfo& addressInfo, const CacheLine& line)
{
    CacheRequest writeBack;
    writeBack.type = RequestType::WRITE;
    writeBack.address = addressInfo.address; // indirizzo ricostruito della linea vittima
    writeBack.dataType = TypeofData::ARRAY_64B;
    writeBack.requestID = 0;
    writeBack.callback = nullptr; // fire-and-forget, nessuno aspetta

    CacheLine victim = line;

    PendingRequest pendingRequest = PendingRequest();
    pendingRequest.request = std::move(writeBack);
    pendingRequest.line = std::move(victim);

    if (nextLevel != nullptr)
        nextLevel->schedulePendingRequest(std::move(pendingRequest));
    else
        bus.getCPU().getCacheManager().getMemoryScheduler().schedulePendingRequest(std::move(pendingRequest));



}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////7


void MemoryScheduler::scheduleMemoryRequest(CacheRequest&& request)
{
    DEBUG_LOG(debugLog("richeista inserita in ram"));
    PendingRequest pendingRequest = PendingRequest(); // Create a new pending memory request to be scheduled for processing by the memory scheduler
    pendingRequest.request = std::move(request);   
    pendingRequest.remainingLatency = memoryLatency; // Set the remaining latency for the memory request based on the specified memory latency
    pendingRequest.state = RequestState::WAITING_LATENCY; // Set the initial state of the memory request to WAITING_LATENCY to indicate that it is waiting for the specified memory latency before being processed
    memoryRequestQueue.push_back(std::move(pendingRequest)); // Add the pending memory request to the queue of pending requests to be processed by the memory scheduler
}

void MemoryScheduler::schedulePendingRequest(PendingRequest&& pendingRequest)
{
    pendingRequest.remainingLatency = memoryLatency; // Set the remaining latency for the pending request based on the specified memory latency
    pendingRequest.state = RequestState::WAITING_LATENCY; // Set the initial state of the pending request to WAITING_LATENCY to indicate that it is waiting for the specified memory latency before being processed
    memoryRequestQueue.push_back(std::move(pendingRequest)); // Add the pending request to the queue of pending requests to be processed by the memory scheduler
}

void MemoryScheduler::processMemoryRequests()
{
    for (auto it = memoryRequestQueue.begin(); it != memoryRequestQueue.end();)
    {
        PendingRequest& pendingRequest = *it;

        if (pendingRequest.state == RequestState::WAITING_LATENCY) // If the pending request is in the WAITING_LATENCY state
        {
            if (pendingRequest.remainingLatency > 0) // If there is remaining latency for the pending request
            {
                --pendingRequest.remainingLatency; // Decrement the remaining latency for the pending request
            }
            if (pendingRequest.remainingLatency == 0) // If the remaining latency for the pending request has reached zero
            {
                pendingRequest.state = RequestState::READY_TO_PROCESS; // Set the state of the pending request to READY_TO_PROCESS to indicate that it is ready to be processed by the memory scheduler
            }
        }

        if (pendingRequest.state == RequestState::READY_TO_PROCESS) // If the pending request is in the READY_TO_PROCESS state
        {   
            processMemoryRequest(std::move(pendingRequest)); // Process the memory request using the processMemoryRequest function, which will handle the actual processing of the memory request based on its type and data
            it = memoryRequestQueue.erase(it); // Remove the processed memory request from the queue of pending requests
        }
        else
            ++it; // Move to the next pending request in the queue if the current request is not ready to be processed
    }
    
}

void MemoryScheduler::processMemoryRequest(PendingRequest&& pendingRequest)
{
    DEBUG_LOG(debugLog("esecuzione ram richeista:" + std::to_string(pendingRequest.request.requestID)));
    CacheRequest& originalRequest = pendingRequest.request; // Get the cache request from the pending request to be processed by the memory scheduler
    LineData readResponse;

    //calculating the adress for the start of the line
    uint64_t lineAddress = originalRequest.address & ~(CACHE_LINE_SIZE - 1); //BITMASK FOR REMOVING 6BITS FINALS TAHT RAPPRESENTS THE OFFSET



    switch (originalRequest.type) // Process the memory request based on its type (READ or WRITE) and handle the data accordingly
    {
        case RequestType::READ:
        {
            DEBUG_LOG(debugLog("lettura ram"));
            readResponse = bus.getMemory().read(lineAddress);
            
            DEBUG_LOG(debugLog("lettura ram completata"));

                auto makeFillPending =[&]() ->PendingRequest
                {
                    auto fillRequest = CacheRequest();
                    fillRequest.address = lineAddress;
                    fillRequest.dataType = TypeofData::ARRAY_64B;
                    fillRequest.type = RequestType::FILL;

                    CacheLine line = CacheLine();
                    line.data = readResponse;

                    auto fillPendingRequest = PendingRequest();
                    fillPendingRequest.line= std::move(line);
                    fillPendingRequest.request = std::move(fillRequest);

                    return fillPendingRequest;

                };

               
              

                bus.getCPU().getCacheManager().getL3Cache().schedulePendingRequest(makeFillPending());

        
                bus.getCPU().getCacheManager().getL2Cache().schedulePendingRequest(makeFillPending());

                bus.getCPU().getCacheManager().getL1Cache().schedulePendingRequest(makeFillPending());
                
                DEBUG_LOG(debugLog("fill inviate"));

                //sending to L1 also teh original request to hit and responde to cpu
                
                bus.getCPU().getCacheManager().getL1Cache().schedulePendingRequest(std::move(pendingRequest));

                DEBUG_LOG(debugLog("invio originale"));

            break;
        }
        case RequestType::READ_MEMORY_FOR_WRITE_MISS:
        {
            readResponse = bus.getMemory().read(lineAddress);
           
                auto fillRequest = CacheRequest();
                fillRequest.address = lineAddress;
                fillRequest.dataType = TypeofData::ARRAY_64B;
                fillRequest.type = RequestType::FILL;

                CacheLine line = CacheLine();
                line.data = readResponse;

                auto fillPendingRequest = PendingRequest();
                fillPendingRequest.request = std::move(fillRequest);
                fillPendingRequest.line = std::move(line);

                DEBUG_LOG(debugLog("invio fill L3"));
                bus.getCPU().getCacheManager().getL3Cache().schedulePendingRequest(std::move(fillPendingRequest));

                //sending to L1 also teh original request to hit and responde to cpu
                originalRequest.type = RequestType::WRITE;
                DEBUG_LOG(debugLog("invio richeista originale"));
                bus.getCPU().getCacheManager().getL3Cache().schedulePendingRequest(std::move(pendingRequest));
                
            break;
        }
        case RequestType::WRITE:
        {
            bus.getMemory().write(lineAddress, pendingRequest.line.data);

            //gestire in futuro errori

            break;
        }
        default:
            break;
    }

    
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////7

CacheManager::CacheManager(Bus& bus,uint32_t l1Size, uint32_t l2Size, uint32_t l3Size, uint8_t l1Assoc, uint8_t l2Assoc, uint8_t l3Assoc, uint8_t l1Latency, uint8_t l2Latency, uint8_t l3Latency, uint8_t l1FillLatency, uint8_t l2FillLatency, uint8_t l3FillLatency)
                            :   L3Cache(l3Size, l3Assoc, l3Latency, l3FillLatency, bus, nullptr, nullptr, CacheLevelType::L3),
                                L2Cache(l2Size, l2Assoc, l2Latency, l2FillLatency, bus, nullptr, nullptr, CacheLevelType::L2),
                                L1Cache(l1Size, l1Assoc, l1Latency, l1FillLatency, bus, nullptr, nullptr, CacheLevelType::L1),
                                memoryScheduler(bus,memoryLatency),
                                bus(bus) 
{
    DEBUG_LOG(debugLog("inizializzazione cache manager"));
    L3Cache.setParentLevel(&L2Cache);

    L2Cache.setParentLevel(&L1Cache);
    L2Cache.setNextLevel(&L3Cache);

    L1Cache.setNextLevel(&L2Cache);
}

CacheManager::~CacheManager(){}

void CacheManager::execute_operation()
{   
    
    for (auto it = requestQueue.begin(); it != requestQueue.end();)
    {
        CacheRequest request =(*it);
        L1Cache.scheduleRequest(std::move(request));

        it = requestQueue.erase(it);
    }

    memoryScheduler.processMemoryRequests();
    L3Cache.execute_operation();
    L2Cache.execute_operation();
    L1Cache.execute_operation();
    


}

void CacheManager::flushAllCaches()
{
    L3Cache.getStorage().flush([&](uint64_t address, const LineData& line){bus.getMemory().write(address, line);});
    L2Cache.getStorage().flush([&](uint64_t address, const LineData& line){bus.getMemory().write(address, line);});
    L1Cache.getStorage().flush([&](uint64_t address, const LineData& line){bus.getMemory().write(address, line);});
}