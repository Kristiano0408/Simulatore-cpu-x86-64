#ifndef RESULT_HPP
#define RESULT_HPP


#include <cstdint>

enum class ComponentType : uint8_t
{
    CACHE,
    CACHE_L1,
    CACHE_L2,
    CACHE_L3,
    RAM,
    OPERAND,
    ALU,
    FPU,
    UNKNOWN,
};

enum class ErrorType : uint8_t
{
    NONE,
    INVALID_ADDRESS,
    INVALID_SIZE,
    OUT_OF_BOUNDS,
    WRITE_FAIL,
    READ_FAIL,
    UNKNOWN,
    STACK_OVERFLOW,
    WAITING_MEMORY
};

enum class EventType : uint8_t
{
    NONE,
    CACHE_HIT,
    CACHE_MISS,
    RAM_ACCESS,
    ERROR,
    CACHE_READ,
    CACHE_WRITE,
    CACHE_EVICT,
    CACHE_FILL,
    CACHE_READ_ERROR,
    CACHE_WRITE_ERROR,
    RAM_READ_ERROR,
    RAM_WRITE_ERROR,
    OPERAND_SET_VALUE,
    OPERAND_GET_VALUE,
};

struct Error_Event_Info {
    ComponentType source = ComponentType::UNKNOWN; // Source of the error or event
    EventType event = EventType::NONE;
    ErrorType error = ErrorType::NONE;
};

struct Result 
{
    bool success;
    Error_Event_Info errorInfo; // Error information if any
};






#endif // RESULT_HPP