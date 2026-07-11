#ifndef EVENT_HANDLER_HPP
#define EVENT_HANDLER_HPP

#include <functional>
#include <unordered_map>
#include <string>
#include <any>
#include <variant>
#include <array>
#include <optional>
#include "helpers.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct CacheLookupPayload;
struct CacheEventPayload;
enum class EventHandlerCacheEventType : uint8_t;

class CacheEventHandler 
{
    using CallbackLookupType = LookUpResult(*)(void* context, CacheLookupPayload&);
    using CallbackHandleRequestType = void(*)(void* context, CacheEventPayload&);
    using ArrayCallbackHandleRequestType = std::array<CallbackHandleRequestType, static_cast<size_t>(EventHandlerCacheEventType::COUNT)>;
    public:
        CacheEventHandler();

        void setContext(void* context) { callbackContext = context; } // Set the context pointer for the cache controller to access its own state or data when processing requests

        void registerHandleRequestEvent(const EventHandlerCacheEventType eventType, CallbackHandleRequestType callback);

        void registerLookupCacheEvent(CallbackLookupType callback);

        void triggerHandleRequestEvent(const EventHandlerCacheEventType eventType, CacheEventPayload& payload);

        LookUpResult triggerLookupCacheEvent(CacheLookupPayload& payload);

        const ArrayCallbackHandleRequestType& getHandleRequestCallbacks() const { return handleRequestCallbacks; }

        CallbackLookupType getLookupCacheCallback() const { return lookupCacheCallback; }




      
    private:
        ArrayCallbackHandleRequestType handleRequestCallbacks; // Vector to hold controller-specific events and callbacks
        void* callbackContext; // Context pointer to be passed to the callback function for additional information or state management
        CallbackLookupType lookupCacheCallback;
};


enum class EventHandlerPipelineEventType : uint8_t;
enum class StageType : uint8_t;

class PipelineEventHandler
{
    using CallbackPipelineEventType = void(*)(void* context);
    using ArrayCallbackPipelineEventType = std::array<CallbackPipelineEventType, static_cast<size_t>(EventHandlerPipelineEventType::COUNT)>;
    public:
        PipelineEventHandler();

        void setContext(void* context) { callbackContext = context; } // Set the context pointer for the cache controller to access its own state or data when processing requests

        void* getContext() const { return callbackContext; } // Get the context pointer for the cache controller to access its own state or data when processing requests
        
        void registerPipelineEvent(const EventHandlerPipelineEventType eventName, CallbackPipelineEventType callback);

        void triggerPipelineEvent(const EventHandlerPipelineEventType eventName);

        CallbackPipelineEventType getCallback(const EventHandlerPipelineEventType eventName) const;

    private:
        ArrayCallbackPipelineEventType pipelineCallbacks;
        void* callbackContext; // Context pointer to be passed to the callback function for additional information or state management
};
#endif // EVENT_HANDLER_HPP