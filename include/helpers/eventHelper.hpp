#ifndef EVENT_HELPER_HPP
#define EVENT_HELPER_HPP
#include <cstdint>

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


enum class EventHandlerExecuteEngineEventType : uint8_t 
{

    EXECUTE_PHASE_COMPLETE = 0,
    MEMORY_PHASE_COMPLETE = 1,
    OPERAND_COMPLETE_READ_EXECUTION = 2,
    OPERAND_COMPLETE_WRITE_MEMORY = 3,  
    OPERAND_COMPLETE_WRITE_WRITEBACK = 4,

    MEMORY_WAITING = 5,
    MEMORY_DONE = 6,
    MEMORY_WAITING_EXECUTE = 7,
    MEMORY_DONE_EXECUTE = 8,

    COUNT = 9 // Represents the total number of event types
};

#endif // EVENT_HELPER_HPP