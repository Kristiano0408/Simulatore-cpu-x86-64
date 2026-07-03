#ifndef CACHE_TYPES_HPP
#define CACHE_TYPES_HPP

#include <cstdint>
#include <functional>
#include "types.hpp"
#include "result.hpp"

// Request type enumeration 
enum class RequestType
{
    READ,
    READ_MEMORY_FOR_WRITE_MISS,
    WRITE,
    FILL, // For filling cache lines during a miss
    READ_AFTER_FILL,
    WRITE_AFTER_FILL,
    PREFETCH, // For prefetching cache lines
    NONE,
    WRITE_BACK, // For writing back dirty cache lines to lower levels or RAM
    //LINE_FROM_RAM // For getting lines on l3 misses
};

////////////////////////////////////////////////////////////////////////////7

//RequestState enum class 

enum class RequestState
{
    WAITING_LATENCY,
    READY_TO_PROCESS,
    IDLE,
    PROCESSING,
    HIT_READY,
    MISS_PENDING,
    DONE
};

////////////////////////////////////////////////////////////////////////////////////////

enum class TypeofData
{
    UINT8_T = 1,
    UINT16_T = 2,
    UINT32_T = 4,
    UINT64_T = 8,
    ARRAY_16B = 16,
    ARRAY_64B = 64,
    ARRAY_128B = 128,
    UNKNOWN = 0
};


struct CacheRequest
{
    RequestType type = RequestType::NONE; // Type of request (READ or WRITE)
    TypeofData dataType = TypeofData::UNKNOWN; // Type of data for the request
    uint64_t address = 0; // Memory address
    MaxCPUInstructionLength data{}; // Data for write requests (up to 15 bytes, maximun size for an instruction with prefixes and opcode, 16 bytes to align)
    bool completed = false; // Indicates if the request has been completed
    int requestID = 0; // Unique ID for the request
    std::function<void()> callback; // Callback function to be called when the request is completed

    CacheRequest(): type(RequestType::NONE), dataType(TypeofData::UNKNOWN), address(0), data{}, completed(false), requestID(0), callback(nullptr) {}

    CacheRequest(RequestType type, TypeofData dataType, uint64_t address, const MaxCPUInstructionLength& data, bool completed, uint64_t requestID, std::function<void()> callback)
        : type(type), dataType(dataType), address(address), data(data), completed(completed), requestID(requestID), callback(callback) {}
};


enum class CacheLevelType
{
    L1,
    L2,
    L3,
    NONE //placeholder for basic class

};


enum class LookUpResult 
{
    HIT,
    MISS,
    HIT_CROSS_LINES,
    ERROR
};



ComponentType getComponentTypeFromCacheLevelType(CacheLevelType type);









#endif // CACHE_TYPES_HPP