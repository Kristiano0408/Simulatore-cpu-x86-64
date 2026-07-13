#include "eventHandler.hpp"
#include "cache/cacheManager.hpp"
#include "pipelineController.hpp"

CacheEventHandler::CacheEventHandler() : callbackContext(nullptr),lookupCacheCallback(nullptr)
{
    handleRequestCallbacks.fill(nullptr); // Fill the array with nullptr values
}

void CacheEventHandler::registerHandleRequestEvent(EventHandlerCacheEventType eventType, CallbackHandleRequestType callback) 
{
    uint8_t eventTypeIndex = static_cast<uint8_t>(eventType); // Convert the event type to an index for the vector


    handleRequestCallbacks[eventTypeIndex] = callback; // Register the callback function for the specified event type in the vector
}


void CacheEventHandler::registerLookupCacheEvent(CallbackLookupType callback) 
{
    lookupCacheCallback = callback; // Register the callback function for cache lookup events
}

void CacheEventHandler::triggerHandleRequestEvent(EventHandlerCacheEventType eventType, CacheEventPayload& payload)
{
    uint8_t eventTypeIndex = static_cast<uint8_t>(eventType); // Convert the event type to an index for the vector

    if(handleRequestCallbacks[eventTypeIndex] != nullptr) // Check if a callback function is registered for the specified event type
    {
        handleRequestCallbacks[eventTypeIndex](callbackContext, payload); // Call the registered callback function for the specified event type with the context and payload
    }
}

LookUpResult CacheEventHandler::triggerLookupCacheEvent(CacheLookupPayload& payload)
{
    if (lookupCacheCallback != nullptr) // Check if a callback function for cache lookup events is registered
    {
        return lookupCacheCallback(callbackContext, payload); // Call the registered callback function with the context and payload and return the result
    }
    debugLog("No callback registered for cache lookup event"); // Log a message indicating that no callback is registered for cache lookup events
    return LookUpResult::ERROR; // Return an error result if no callback is registered
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////

PipelineEventHandler::PipelineEventHandler() : callbackContext(nullptr)
{
    pipelineCallbacks.fill(nullptr); // Fill the array with nullptr values
}

void PipelineEventHandler::registerPipelineEvent(const EventHandlerPipelineEventType eventName, CallbackPipelineEventType callback) 
{
    uint8_t eventTypeIndex = static_cast<uint8_t>(eventName); // Convert the event type to an index for the vector

    pipelineCallbacks[eventTypeIndex] = callback; // Register the callback function for the specified event type in the vector
}

void PipelineEventHandler::triggerPipelineEvent(const EventHandlerPipelineEventType eventName) 
{
    uint8_t eventTypeIndex = static_cast<uint8_t>(eventName); // Convert the event type to an index for the vector

    if(pipelineCallbacks[eventTypeIndex] != nullptr) // Check if a callback function is registered for the specified event type
    {
        pipelineCallbacks[eventTypeIndex](callbackContext); // Call the registered callback function for the specified event type with the context
    }
}

PipelineEventHandler::CallbackPipelineEventType PipelineEventHandler::getCallback(const EventHandlerPipelineEventType eventName) const 
{
    uint8_t eventTypeIndex = static_cast<uint8_t>(eventName); // Convert the event type to an index for the vector
    return pipelineCallbacks[eventTypeIndex]; // Return the registered callback function for the specified event type
}

