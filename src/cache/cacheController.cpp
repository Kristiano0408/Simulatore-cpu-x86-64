#include "cache/cacheController.hpp"



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