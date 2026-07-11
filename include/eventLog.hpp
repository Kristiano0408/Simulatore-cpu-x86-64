#ifndef EVENT_LOG_HPP
#define EVENT_LOG_HPP


#include "helpers.hpp"
#include "device.hpp"
#include <vector>
#include <optional>
#include <any>

struct AddressInfo;

struct LogEntry {
    Result result; // Result of the event or error
    uint64_t timestamp; // Timestamp of the event or error
};

struct CacheDataLogEntry {
    LogEntry logEntry; // Log entry containing the result and timestamp
    LineData* line1; // Data of the first cache line
    LineData* line2; // Data of the second cache line (if applicable)
    AddressInfo addressInfo1; // Address information of the first cache line
    AddressInfo addressInfo2; // Address information of the second cache line (if applicable)
};

struct MemoryDataLogEntry {
    LogEntry logEntry; // Log entry containing the result and timestamp
    LineData* lineData; // Data of the memory line
    uint64_t addressLine; // Address of the memory line
};

struct OperandDataLogEntry {
    LogEntry logEntry; // Log entry containing the result and timestamp
    uint64_t operandValue; // Value of the operand
};


struct LogStorage {
    QueueCacheFriendly<CacheDataLogEntry> cacheDataLogs; // Queue to store cache data log entries
    QueueCacheFriendly<MemoryDataLogEntry> memoryDataLogs; // Queue to store memory data log entries
    QueueCacheFriendly<OperandDataLogEntry> operandDataLogs; // Queue to store operand data log entries
    QueueCacheFriendly<LogEntry> generalLogs; // Queue to store general log entries

    LogStorage();

    

};


class EventLog
{
    public:
        static EventLog& getInstance();
        void bindTicks(uint64_t* ticks); // Bind the clock ticks reference to the EventLog
        
        bool isLogEmpty() const; // Check if the log is empty

        void pushCacheDataLogEntry(Result&& result, LineData* line1, LineData* line2, AddressInfo addressInfo1, AddressInfo addressInfo2);
        void pushMemoryDataLogEntry(Result&& result, LineData* lineData, uint64_t addressLine);
        void pushOperandDataLogEntry(Result&& result, uint64_t operandValue);
        void pushGeneralLogEntry(Result&& result);
        
        size_t getCacheDataLogSize() const { return logs.cacheDataLogs.size(); }
        size_t getMemoryDataLogSize() const { return logs.memoryDataLogs.size(); }
        size_t getOperandDataLogSize() const { return logs.operandDataLogs.size(); }
        size_t getGeneralLogSize() const { return logs.generalLogs.size(); }


        CacheDataLogEntry getCacheDataLogEntry() { return logs.cacheDataLogs.pop(); }
        MemoryDataLogEntry getMemoryDataLogEntry() { return logs.memoryDataLogs.pop(); }
        OperandDataLogEntry getOperandDataLogEntry() { return logs.operandDataLogs.pop(); }
        LogEntry getGeneralLogEntry() { return logs.generalLogs.pop(); }
        
    private:
        EventLog() = default;
        EventLog(const EventLog&) = delete;
        EventLog& operator=(const EventLog&) = delete;

        LogStorage logs; // Store the log entries and related data

        uint64_t* clockTicks = nullptr; // Clock ticks since the start of the program

    

};


#endif //EVENT_LOG_HPP