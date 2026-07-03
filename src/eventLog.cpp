#include "eventLog.hpp"
#include <vector>

LogStorage::LogStorage() : logEntries(), positionalLogEntries(), logData() {}




void LogStorage::pushLogEntry(Result&& result, std::optional<std::any>&& data, uint64_t timestamp) 
{
    LogEntry entry{std::move(result), timestamp};
    if (data.has_value()) 
        logData.push_back(std::move(data.value()));
    else 
        logData.push_back(std::any{}); // Store an empty std::any if no data is provided

    logEntries.push_back(std::move(entry));
    positionalLogEntries.push_back(logEntries.size() - 1); // Store the position of the new log entry
}

bool LogStorage::isLogEmpty() const 
{
    return logEntries.empty();
}

size_t LogStorage::getLogSize() const 
{
    return logEntries.size();
}

Result LogStorage::getLogEntry() 
{

    Result result;
    size_t lastIndex = positionalLogEntries.back(); // Get the index of the last log entry
    size_t indexEntryPop = positionalLogEntries.front(); // Get the index of the first log entry
    std::swap(logEntries[lastIndex], logEntries[indexEntryPop]);
    std::swap(logData[lastIndex], logData[indexEntryPop]);

    result = std::move(logEntries.back().result);
    positionalLogEntries[0] = lastIndex; // Update the positional log entries to reflect the swap
    logEntries.pop_back();
    positionalLogEntries.pop_back(); // Remove the last index from the positional log entries
    
    return result;

}

std::any LogStorage::getLogData() 
{
    std::any data;
    data = std::move(logData.back()); //we dont need to do the swap becasue teh data is always fetched after the log entry, so the swap is already done in getLogEntry()
    logData.pop_back();
    return data;
}


EventLog& EventLog::getInstance() 
{
    static EventLog instance;
    return instance;
}

void EventLog::bindTicks(uint64_t* ticks) 
{
    clockTicks = ticks;
}

void EventLog::submitLog(Result&& result, std::any&& data) 
{
    pushLogEntry(std::move(result), std::make_optional(std::move(data)));
}

void EventLog::submitLog(Result&& result) 
{
    pushLogEntry(std::move(result));
}

Result EventLog::getLogEntry()
{
    return logs.getLogEntry();
}

std::any EventLog::getLogData()
{
    return logs.getLogData();
}

void EventLog::pushLogEntry(Result&& result, std::optional<std::any>&& data) 
{
    uint64_t timestamp = clockTicks ? *clockTicks : 0; // Use the bound clock ticks if available, otherwise default to 0
    logs.pushLogEntry(std::move(result), std::move(data), timestamp);
}

bool EventLog::isLogEmpty() const 
{
    return logs.isLogEmpty();
}