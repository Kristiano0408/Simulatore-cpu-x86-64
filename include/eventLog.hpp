#ifndef EVENT_LOG_HPP
#define EVENT_LOG_HPP


#include "helpers.hpp"
#include "device.hpp"
#include <vector>
#include <optional>
#include <any>

struct LogEntry {
    Result result; // Result of the event or error
    uint64_t timestamp; // Timestamp of the event or error
};

struct LogStorage {
    std::vector<LogEntry> logEntries; // Store the log entries
    std::vector<size_t> positionalLogEntries; // Store the positions of log entries for quick access
    std::vector<std::any> logData; // Store any additional data related to the event

    LogStorage();

    void pushLogEntry(Result&& result, std::optional<std::any>&& data = std::nullopt, uint64_t timestamp = 0);
    Result getLogEntry();
    std::any getLogData();

    bool isLogEmpty() const; // Check if the log is empty
    size_t getLogSize() const; // Get the number of log entries


};


class EventLog
{
    public:
        static EventLog& getInstance();
        void bindTicks(uint64_t* ticks); // Bind the clock ticks reference to the EventLog
        void submitLog(Result&& result, std::any&& data);
        void submitLog(Result&& result);

        Result getLogEntry();
        std::any getLogData();

        bool isLogEmpty() const; // Check if the log is empty
        
        

    private:
        EventLog() = default;
        EventLog(const EventLog&) = delete;
        EventLog& operator=(const EventLog&) = delete;

        void pushLogEntry(Result&& result, std::optional<std::any>&& data = std::nullopt);

        LogStorage logs; // Store the log entries and related data

        uint64_t* clockTicks = nullptr; // Clock ticks since the start of the program

    

};


#endif //EVENT_LOG_HPP