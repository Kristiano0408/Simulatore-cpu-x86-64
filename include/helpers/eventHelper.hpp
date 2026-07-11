#ifndef EVENT_HELPER_HPP
#define EVENT_HELPER_HPP


enum class EventHandlerCacheEventType : uint8_t 
{
    CACHE_HIT,
    CACHE_HIT_CROSS_LINES,
    CACHE_MISS,
    CACHE_FILL,
    CACHE_LOOKUP,
    COUNT // Represents the total number of event types
};

enum class EventHandlerPipelineEventType : uint8_t 
{
    FETCH_COMPLETE,
    DECODE_COMPLETE,
    OPERAND_FETCH_COMPLETE,
    EXECUTE_COMPLETE,
    MEMORY_COMPLETE,
    WRITE_BACK_COMPLETE,
    
    MEMORY_WAITING,
    MEMORY_DONE,
    MEMORY_WAITING_EXECUTE,
    MEMORY_DONE_EXECUTE,
    MEMORY_WAITING_FETCH,
    MEMORY_DONE_FETCH,
    COUNT // Represents the total number of event types
};

#endif // EVENT_HELPER_HPP