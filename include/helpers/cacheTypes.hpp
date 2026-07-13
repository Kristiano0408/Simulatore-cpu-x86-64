#ifndef CACHE_TYPES_HPP
#define CACHE_TYPES_HPP

#include <cstdint>
#include <functional>
#include "types.hpp"
#include "result.hpp"

// Request type enumeration 
enum class RequestType : uint8_t
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
enum class RequestState : uint8_t
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

// Type of data being requested or stored in the cache
enum class TypeofData : uint8_t
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

//////////////////////////////////////////////////////////////////////////////////////////////

struct AddressInfo
{
    uint64_t address;
    uint64_t setIndex;
    uint64_t tag;
    uint64_t offset;

    AddressInfo(uint64_t addr, uint64_t setIdx, uint64_t tg, uint64_t off)
        : address(addr), setIndex(setIdx), tag(tg), offset(off) {}

    ///AddressInfo() : address(0), setIndex(0), tag(0), offset(0) {}
};

////////////////////////////////////////////////////////////////////////////////////////////////

enum class CacheLevelType : uint8_t
{
    L1I,
    L1D,
    L2,
    L3,
    NONE //placeholder for basic class

};

////////////////////////////////////////////////////////////////////////////////////////////////////

enum class LookUpResult : uint8_t
{
    HIT,
    MISS,
    HIT_CROSS_LINES,
    ERROR
};

///////////////////////////////////////////////////////////////////////////////////////////////////////

//basic structure for the cache line
//it contains the data, the tag, the valid bit and the dirty bit
struct CacheLine // in caso separare metadati da dati per cache locability
{
    alignas(CACHE_LINE_SIZE) LineData data;  // Data stored in the cache line

    uint64_t tag;
    uint64_t lastAccessTime; // use it as a counter for replacement policy, syncronized with clock when access

    bool valid;
    bool dirty; // Indicates if the line has been modified
   

    CacheLine() : data{}, tag{}, lastAccessTime{}, valid(false), dirty(false) {}


};


/// Cache set structure
/// Contains multiple cache lines and the set index
struct CacheSet
{
    std::vector<CacheLine> lines; // Lines in the cache set
    uint64_t setIndex;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct CacheRequest
{
    using CallbackType = void(*)(void* context); // Callback function type for request completion

    uint64_t address = 0; // Memory address
    MaxCPUInstructionLength data{}; // Data for write requests (up to 15 bytes, maximun size for an instruction with prefixes and opcode, 16 bytes to align)
    uint64_t requestID = 0; // Unique ID for the request
    void* callbackContext = nullptr; // Context pointer for the callback function
    CallbackType callback = nullptr; // Callback function to be called when the request is completed
    bool completed = false; // Indicates if the request has been completed
    RequestType type = RequestType::NONE; // Type of request (READ or WRITE)
    TypeofData dataType = TypeofData::UNKNOWN; // Type of data for the request
    CacheLevelType typeofL1 = CacheLevelType::NONE;

    CacheRequest():  address(0), data{}, requestID(0), callbackContext(nullptr), callback(nullptr), completed(false), type(RequestType::NONE), dataType(TypeofData::UNKNOWN), typeofL1(CacheLevelType::NONE){}

    CacheRequest(RequestType type, TypeofData dataType, CacheLevelType typeofL1, uint64_t address, const MaxCPUInstructionLength& data, bool completed, uint64_t requestID, void* callbackContext, CallbackType callback)
        : address(address), data(data), requestID(requestID), callbackContext(callbackContext), callback(callback), completed(completed), type(type), dataType(dataType), typeofL1(typeofL1) {}
};

struct PendingRequest
{   
    
    CacheLine line= CacheLine{}; //copy of the line 
    CacheRequest request;
    uint16_t remainingLatency; // Remaining latency in ticks
    RequestState state = RequestState::IDLE;
    
    

    PendingRequest(CacheRequest&& req, uint16_t latency)
        : request(std::move(req)), remainingLatency(latency) {}

    PendingRequest(CacheRequest&& req, uint16_t latency, CacheLine res)
        : line(std::move(res)), request(std::move(req)), remainingLatency(latency)  {}
    PendingRequest(CacheRequest&& req, uint16_t latency, RequestState st)
        : request(std::move(req)), remainingLatency(latency), state(st) {}
    PendingRequest(CacheRequest&& req, CacheLine res)
        : line(std::move(res)), request(std::move(req)) {}
    PendingRequest() = default;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct CacheEventPayload
{
    AddressInfo addressInfo1;
    AddressInfo addressInfo2;
    CacheRequest& request;
    CacheLine* line; // Pointer to the cache line involved in the event, if applicable

    CacheEventPayload(const AddressInfo& addrInfo1, const AddressInfo& addrInfo2, CacheRequest& req) : addressInfo1(addrInfo1), addressInfo2(addrInfo2), request(req) {}
    CacheEventPayload(const AddressInfo& addrInfo, CacheRequest& req) : addressInfo1(addrInfo), addressInfo2(0, 0, 0, 0), request(req), line(nullptr) {}
    CacheEventPayload(const AddressInfo& addrInfo, CacheRequest& req, CacheLine* cacheLine) : addressInfo1(addrInfo), addressInfo2(0, 0, 0, 0), request(req), line(cacheLine) {}
};

struct CacheLookupPayload
{
    AddressInfo addressInfo;
    TypeofData dataType;

    CacheLookupPayload(const AddressInfo& addrInfo, TypeofData dt) : addressInfo(addrInfo), dataType(dt) {}
};







ComponentType getComponentTypeFromCacheLevelType(CacheLevelType type);









#endif // CACHE_TYPES_HPP