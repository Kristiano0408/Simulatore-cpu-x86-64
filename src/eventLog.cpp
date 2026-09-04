#include "eventLog.hpp"
#include "cache/cacheManager.hpp"
#include <vector>

LogStorage::LogStorage() : cacheDataLogs(), memoryDataLogs(), operandDataLogs(), generalLogs() 
{}


EventLog& EventLog::getInstance() 
{
    static EventLog instance;
    return instance;
}

void EventLog::bindTicks(uint64_t* ticks) 
{
    clockTicks = ticks;
}

bool EventLog::isLogEmpty() const 
{
    return logs.cacheDataLogs.size() == 0 && logs.memoryDataLogs.size() == 0 &&
           logs.operandDataLogs.size() == 0 && logs.generalLogs.size() == 0;
}

void EventLog::pushCacheDataLogEntry(Result&& result, LineData* line1, LineData* line2, AddressInfo addressInfo1, AddressInfo addressInfo2) 
{
    CacheDataLogEntry entry{};
    entry.logEntry.result = std::move(result);
    entry.logEntry.timestamp = (clockTicks != nullptr) ? *clockTicks : 0; // Use the current clock ticks if available, otherwise default to 0
    entry.line1 = line1;
    entry.line2 = line2;
    entry.addressInfo1 = addressInfo1;
    entry.addressInfo2 = addressInfo2;
    logs.cacheDataLogs.push(std::move(entry));
}

void EventLog::pushMemoryDataLogEntry(Result&& result, LineData* lineData, uint64_t addressLine) 
{
    MemoryDataLogEntry entry{};
    entry.logEntry.result = std::move(result);
    entry.logEntry.timestamp = (clockTicks != nullptr) ? *clockTicks : 0; // Use the current clock ticks if available, otherwise default to 0
    entry.lineData = lineData;
    entry.addressLine = addressLine;
    logs.memoryDataLogs.push(std::move(entry));
}

void EventLog::pushOperandDataLogEntry(Result&& result, uint64_t operandValue) 
{
    OperandDataLogEntry entry{};
    entry.logEntry.result = std::move(result);
    entry.logEntry.timestamp = (clockTicks != nullptr) ? *clockTicks : 0; // Use the current clock ticks if available, otherwise default to 0
    entry.operandValue = operandValue;
    logs.operandDataLogs.push(std::move(entry));
}

void EventLog::pushGeneralLogEntry(Result&& result) 
{
    LogEntry entry{};
    entry.result = std::move(result);
    entry.timestamp = (clockTicks != nullptr) ? *clockTicks : 0; // Use the current clock ticks if available, otherwise default to 0
    logs.generalLogs.push(std::move(entry));
}

void EventLog::reset() {
    logs.cacheDataLogs = FixedSizeQueueCacheFriendly<CacheDataLogEntry, 1024>();
    logs.memoryDataLogs = FixedSizeQueueCacheFriendly<MemoryDataLogEntry, 1024>();
    logs.operandDataLogs = FixedSizeQueueCacheFriendly<OperandDataLogEntry, 1024>();
    logs.generalLogs = FixedSizeQueueCacheFriendly<LogEntry, 1024>();
}