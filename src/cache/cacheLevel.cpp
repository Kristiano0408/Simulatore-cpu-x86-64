#include "cache/cacheLevel.hpp"
#include "bus.hpp"
#include "eventLog.hpp"

CacheLevel::CacheLevel(uint64_t size, uint8_t associativity, uint8_t latency, uint8_t fillLatency, Bus& bus, CacheLevel *nextLevel, CacheLevel* parentLevel1, CacheLevel* parentLevel2, CacheLevelType type, std::array<CacheLevel*, 2> L1Icourse, std::array<CacheLevel*, 2> L1Dcourse)
    : cacheSize(size), numSets((uint32_t)(size / (associativity * CACHE_LINE_SIZE))), bus(bus), nextLevel(nextLevel), parentLevel1(parentLevel1), parentLevel2(parentLevel2), L1Ipath(L1Icourse), L1Dpath(L1Dcourse), associativity(associativity), latencyCycles(latency), latencyFill(fillLatency), type(type),
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
    eventHandler.registerLookupCacheEvent(&CacheLevel::lookupWrapper); // Register the lookupWrapper function to handle cache lookup events

    
    DEBUG_LOG(debugLog("inizializzazione cache level completata"));

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

LookUpResult CacheLevel::lookupWrapper(void* context, CacheLookupPayload& payload)
{
    DEBUG_LOG(debugLog("lookup cache"));
    CacheLevel* cacheLevel = static_cast<CacheLevel*>(context); // Cast the context pointer to a CacheLevel pointer
    return cacheLevel->lookupCache(payload.addressInfo, payload.dataType); // Call the lookupCache function to perform the cache lookup and return the result
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CacheLevel::execute_operation()
{
    #ifdef DEBUG
    std::string cache;


    switch (type)
    {
    case CacheLevelType::L1I:
        cache = "L1I";
        break;
    case CacheLevelType::L1D:
        cache = "L1D";
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
            EventLog::getInstance().pushCacheDataLogEntry({std::move(result)}, nullptr, nullptr, addressInfo, AddressInfo(0, 0, 0, 0)); // Log the cache hit event with the address and data from the cache line
            return LookUpResult::HIT; // Return HIT if it's a hit within a single cache line
        }
        
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
            EventLog& log = EventLog::getInstance();
            log.pushCacheDataLogEntry({std::move(result)}, static_cast<LineData*>(nullptr), static_cast<LineData*>(nullptr), addressInfo, AddressInfo(uint64_t(0), uint64_t(0), uint64_t(0), uint64_t(0))); // Log the cache miss event with the address and data from the first cache line
            return LookUpResult::MISS; // Return MISS if the second cache line for cross-line access is not found, indicating a miss across two cache lines
       
    }
    
        result.success = false; // Set the result to failure for a miss within a single cache line
        result.errorInfo.source = getComponentTypeFromCacheLevelType(type); // Set the source of the error information based on the cache level type
        result.errorInfo.event = EventType::CACHE_MISS;
        result.errorInfo.error = ErrorType::OUT_OF_BOUNDS;
        EventLog::getInstance().pushCacheDataLogEntry(std::move(result), static_cast<LineData*>(nullptr), static_cast<LineData*>(nullptr), addressInfo, AddressInfo(uint64_t(0), uint64_t(0), uint64_t(0), uint64_t(0))); // Log the cache miss event with the address and data from the first cache line
        return LookUpResult::MISS; // Return MISS if no matching cache line is found
   
}

void CacheLevel::onHit(const AddressInfo& addressInfo, CacheRequest& request)
{
    if(type != CacheLevelType::L1I && type != CacheLevelType::L1D)
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
        replacementPolicy->updateOnAccess(storage.getSet(addressInfo.setIndex), (uint8_t)lineIndex);
        CacheLine* line = storage.findLine(addressInfo.setIndex, addressInfo.tag);

        

    
       
        request.typeofL1 == CacheLevelType::L1I ? fillToL1I(line, addressInfo) : fillToL1D(line, addressInfo);
        
        
        
        //recreating the original request for L1
        PendingRequest originalPendingRequest = PendingRequest();
        originalPendingRequest.request = request;

        originalPendingRequest.request.type =  originalPendingRequest.request.type == RequestType::READ ? RequestType::READ_AFTER_FILL : RequestType::WRITE_AFTER_FILL;
        switch(request.typeofL1)
        {
            case CacheLevelType::L1I:
                L1Ipath[1]->schedulePendingRequest(std::move(originalPendingRequest));
                break;
            case CacheLevelType::L1D:
                L1Dpath[1]->schedulePendingRequest(std::move(originalPendingRequest));
                break;
            default:
                //error handling
                break;
        }
        

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
     if(type != CacheLevelType::L1I && type != CacheLevelType::L1D)
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
        replacementPolicy->updateOnAccess(storage.getSet(addressInfo1.setIndex), (uint8_t)lineIndex1);
        CacheLine* line1 = storage.findLine(addressInfo1.setIndex, addressInfo1.tag);


        int8_t lineIndex2 = storage.findLineIndex(addressInfo2.setIndex, addressInfo2.tag);
        if (lineIndex2 < 0)
        {
            //error Hendling
        }
        replacementPolicy->updateOnAccess(storage.getSet(addressInfo2.setIndex), (uint8_t)lineIndex2);
        CacheLine* line2 = storage.findLine(addressInfo2.setIndex, addressInfo2.tag);

        request.typeofL1 == CacheLevelType::L1I ? fillToL1Idouble(line1, line2, addressInfo1, addressInfo2) : fillToL1Ddouble(line1, line2, addressInfo1, addressInfo2);
        
        //recreating the original request for L1
        PendingRequest originalPendingRequest = PendingRequest();
        originalPendingRequest.request = request;

        originalPendingRequest.request.type =  originalPendingRequest.request.type == RequestType::READ ? RequestType::READ_AFTER_FILL : RequestType::WRITE_AFTER_FILL;
        switch(request.typeofL1)
        {
            case CacheLevelType::L1I:
                L1Ipath[1]->schedulePendingRequest(std::move(originalPendingRequest));
                break;
            case CacheLevelType::L1D:
                L1Dpath[1]->schedulePendingRequest(std::move(originalPendingRequest));
                break;
            default:
                //error handling
                break;
        }

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
        EventLog::getInstance().pushCacheDataLogEntry(std::move(result), &(line->data), static_cast<LineData*>(nullptr), addressInfo, AddressInfo(0, 0, 0, 0)); // Log the cache hit event with the address and data from the cache line
    }
    else
    {
        result.success = false; // Set the result to failure for a miss within a single cache line
        result.errorInfo.source = getComponentTypeFromCacheLevelType(type); // Set the source of the error information based on the cache level type
        result.errorInfo.event = EventType::CACHE_READ;
        result.errorInfo.error = ErrorType::OUT_OF_BOUNDS;
        EventLog::getInstance().pushCacheDataLogEntry(std::move(result), static_cast<LineData*>(nullptr), static_cast<LineData*>(nullptr), AddressInfo(0, 0, 0, 0), AddressInfo(0, 0, 0, 0));
           
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
        EventLog::getInstance().pushCacheDataLogEntry(std::move(result), static_cast<LineData*>(nullptr), static_cast<LineData*>(nullptr), AddressInfo(0, 0, 0, 0), AddressInfo(0, 0, 0, 0));
    
    }

    bus.getCPU().cacheResponseQueue[request.requestID] =std::move(response); // Add the completed request to the CPU's cache response queue for further processing by the CPU

    if(success && (request.callback != nullptr)) // If the request was successful and there is a callback function defined for the request
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
        EventLog::getInstance().pushCacheDataLogEntry(std::move(result), &(line->data), static_cast<LineData*>(nullptr), addressInfo, AddressInfo(0, 0, 0, 0)); // Log the cache hit event with the address and data from the cache line

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
        EventLog::getInstance().pushCacheDataLogEntry(std::move(result), static_cast<LineData*>(nullptr), static_cast<LineData*>(nullptr), AddressInfo(0, 0, 0, 0), AddressInfo(0, 0, 0, 0));
    }

    bus.getCPU().cacheResponseQueue[request.requestID] = std::move(response); // Add the completed request to the CPU's cache response queue for further processing by the CPU

    if(success && (request.callback != nullptr)) // If the request was successful and there is a callback function defined for the request
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
        EventLog::getInstance().pushCacheDataLogEntry(std::move(result), static_cast<LineData*>(nullptr), static_cast<LineData*>(nullptr), AddressInfo(0, 0, 0, 0), AddressInfo(0, 0, 0, 0));
    }


    bus.getCPU().cacheResponseQueue[request.requestID] = std::move(response); // Add the completed request to the CPU's cache response queue for further processing by the CPU

    if(success && (request.callback != nullptr)) // If the request was successful and there is a callback function defined for the request
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
        auto offsetBits = (uint8_t)std::countr_zero(CACHE_LINE_SIZE); // Calculate the number of offset bits based on the cache line size using std::countr_zero to count trailing zeros
        auto setIndexBits = (uint8_t)std::countr_zero(numSets); // Calculate the number of set index bits based on the number of sets in the cache using std::countr_zero to count trailing zeros
        uint64_t victimAddress = (victim.tag << (offsetBits + setIndexBits)) | (addressInfo.setIndex << offsetBits);
        resultEviction.success = true;
        resultEviction.errorInfo.source = getComponentTypeFromCacheLevelType(type);
        resultEviction.errorInfo.event = EventType::CACHE_EVICT;
        resultEviction.errorInfo.error = ErrorType::NONE;
        EventLog::getInstance().pushCacheDataLogEntry(std::move(resultEviction), static_cast<LineData*>(nullptr), static_cast<LineData*>(nullptr), addressInfo, AddressInfo(0, 0, 0, 0));
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
    EventLog::getInstance().pushCacheDataLogEntry(std::move(resultFill), static_cast<LineData*>(nullptr), static_cast<LineData*>(nullptr), AddressInfo(0, 0, 0, 0), AddressInfo(0, 0, 0, 0));

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

void CacheLevel::fillToL1D(CacheLine* line, const AddressInfo& addressInfo)
{
    auto MakeFillPendingRequest = [&]()
        {
            CacheRequest fillRequest = CacheRequest();
            fillRequest.address = addressInfo.address - addressInfo.offset; //adress start line
            fillRequest.dataType = TypeofData::ARRAY_64B;
            fillRequest.type = RequestType::FILL;
            fillRequest.typeofL1 = CacheLevelType::NONE;
            PendingRequest fillPendingRequest = PendingRequest();
            fillPendingRequest.line = *line;
            fillPendingRequest.request = fillRequest;
            
            return fillPendingRequest;
        };

    for (CacheLevel* parentLevel :L1Dpath)
    {
        if(parentLevel != nullptr)
        {
            parentLevel->schedulePendingRequest(MakeFillPendingRequest());
        }
    }
}

void CacheLevel::fillToL1I(CacheLine* line, const AddressInfo& addressInfo)
{
    auto MakeFillPendingRequest = [&]()
        {
            CacheRequest fillRequest = CacheRequest();
            fillRequest.address = addressInfo.address - addressInfo.offset; //adress start line
            fillRequest.dataType = TypeofData::ARRAY_64B;
            fillRequest.type = RequestType::FILL;
            fillRequest.typeofL1 = CacheLevelType::NONE;
            PendingRequest fillPendingRequest = PendingRequest();
            fillPendingRequest.line = *line;
            fillPendingRequest.request = fillRequest;
            
            return fillPendingRequest;
        };

    for (CacheLevel* parentLevel :L1Ipath)
    {
        if(parentLevel != nullptr)
        {
            parentLevel->schedulePendingRequest(MakeFillPendingRequest());
        }
    }
}

void CacheLevel::fillToL1Idouble(CacheLine* line1, CacheLine* line2, const AddressInfo& addressInfo1, const AddressInfo& addressInfo2)
{
    auto MakeFillPendingRequest1 = [&]()
        {
            CacheRequest fillRequest = CacheRequest();
            fillRequest.address = addressInfo1.address - addressInfo1.offset; //adress start line
            fillRequest.dataType = TypeofData::ARRAY_64B;
            fillRequest.type = RequestType::FILL;
            fillRequest.typeofL1 = CacheLevelType::NONE;
            PendingRequest fillPendingRequest = PendingRequest();
            fillPendingRequest.line = *line1;
            fillPendingRequest.request = fillRequest;
            
            return fillPendingRequest;
        };

    auto MakeFillPendingRequest2 = [&]()
        {
            CacheRequest fillRequest = CacheRequest();
            fillRequest.address = addressInfo2.address - addressInfo2.offset; //adress start line
            fillRequest.dataType = TypeofData::ARRAY_64B;
            fillRequest.type = RequestType::FILL;
            fillRequest.typeofL1 = CacheLevelType::NONE;
            PendingRequest fillPendingRequest = PendingRequest();
            fillPendingRequest.line = *line2;
            fillPendingRequest.request = fillRequest;
            
            return fillPendingRequest;
        };

    for (CacheLevel* parentLevel :L1Ipath)
    {
        if(parentLevel != nullptr)
        {
            parentLevel->schedulePendingRequest(MakeFillPendingRequest1());
            parentLevel->schedulePendingRequest(MakeFillPendingRequest2());
        }
    }
}

void CacheLevel::fillToL1Ddouble(CacheLine* line1, CacheLine* line2, const AddressInfo& addressInfo1, const AddressInfo& addressInfo2)
{
    auto MakeFillPendingRequest1 = [&]()
        {
            CacheRequest fillRequest = CacheRequest();
            fillRequest.address = addressInfo1.address - addressInfo1.offset; //adress start line
            fillRequest.dataType = TypeofData::ARRAY_64B;
            fillRequest.type = RequestType::FILL;
            fillRequest.typeofL1 = CacheLevelType::NONE;
            PendingRequest fillPendingRequest = PendingRequest();
            fillPendingRequest.line = *line1;
            fillPendingRequest.request = fillRequest;
            
            return fillPendingRequest;
        };

    auto MakeFillPendingRequest2 = [&]()
        {
            CacheRequest fillRequest = CacheRequest();
            fillRequest.address = addressInfo2.address - addressInfo2.offset; //adress start line
            fillRequest.dataType = TypeofData::ARRAY_64B;
            fillRequest.type = RequestType::FILL;
            fillRequest.typeofL1 = CacheLevelType::NONE;
            PendingRequest fillPendingRequest = PendingRequest();
            fillPendingRequest.line = *line2;
            fillPendingRequest.request = fillRequest;
            
            return fillPendingRequest;
        };

    for (CacheLevel* parentLevel :L1Dpath)
    {
        if(parentLevel != nullptr)
        {
            parentLevel->schedulePendingRequest(MakeFillPendingRequest1());
            parentLevel->schedulePendingRequest(MakeFillPendingRequest2());
        }
    }
}
