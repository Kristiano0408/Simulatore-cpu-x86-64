#include "memory.hpp"
#include "bus.hpp"
#include "cpu.hpp"
#include "eventLog.hpp"
#include <algorithm>
#include <cmath>
// ricordarsi di convertire indirizzi dec per accedere al vettore e in binario per utilizzare l'indirizzo

// ricordasri controllo offset e size per evitare buffer overflow

// Constructor
Memory::Memory(size_t memorySize)
{

    data.resize(memorySize, 0); // initialize the memory with 0
    this->size = memorySize;
};

void Memory::setDataPartial(const std::vector<uint8_t>& newData, size_t offset)
{
    std::ranges::copy(newData, data.begin() + static_cast<std::ptrdiff_t>(offset));
}

// clear the memory
void Memory::clear()
{
    data.clear();
    data.resize(size, 0);
};

#ifdef GUI_ENABLED
#define pushResultSuccess(line, address)                                                                                                                                           \
    result.success = true;                                                                                                                                                         \
    result.errorInfo.source = ComponentType::RAM;                                                                                                                                  \
    result.errorInfo.event = EventType::RAM_ACCESS;                                                                                                                                \
    result.errorInfo.error = ErrorType::NONE;                                                                                                                                      \
    EventLog::getInstance().pushMemoryDataLogEntry(std::move(result), &(line), address);

#define pushResultFailure(address)                                                                                                                                                 \
    result.success = false;                                                                                                                                                        \
    result.errorInfo.source = ComponentType::RAM;                                                                                                                                  \
    result.errorInfo.event = EventType::ERROR;                                                                                                                                     \
    result.errorInfo.error = ErrorType::INVALID_ADDRESS;                                                                                                                           \
    EventLog::getInstance().pushMemoryDataLogEntry(std::move(result), static_cast<LineData*>(nullptr), address);
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t Memory::readTest(uint64_t addressLine)
{
    uint8_t value{};
    #ifdef GUI_ENABLED
    Result result{};
    #endif

    if (addressLine + sizeof(value) > size) // check if the address is out of bounds
    {
        #ifdef GUI_ENABLED
        pushResultFailure(addressLine) return value;
        #else
        return value;
        #endif
    }
    std::memcpy(&value, &data[addressLine], sizeof(value));


    return value;
}


void Memory::writeTest(uint64_t addressLine, uint8_t value)
{
    #ifdef GUI_ENABLED
    Result result{};
    #endif
    if (addressLine + sizeof(value) > size) // check if the address is out of bounds
    {
        #ifdef GUI_ENABLED
        pushResultFailure(addressLine) return;
        #else
        return;
        #endif
    }

    std::memcpy(&data[addressLine], &value, sizeof(value));
}


LineData Memory::read(uint64_t addressLine)
{
    LineData lineData{};
    #ifdef GUI_ENABLED
    Result result{};
    #endif

    if (addressLine + sizeof(LineData) > size) // check if the address is out of bounds
    {
        #ifdef GUI_ENABLED
        pushResultFailure(addressLine) return lineData;
        #else
        return lineData;
        #endif
    }
    std::memcpy(lineData.data(), &data[addressLine], sizeof(LineData));

    #ifdef GUI_ENABLED
    pushResultSuccess(lineData, addressLine)
    #endif

        return lineData;
}

void Memory::write(uint64_t addressLine, LineData line)
{   
    #ifdef GUI_ENABLED
    Result result{};
    #endif
    if (addressLine + sizeof(LineData) > size) // check if the address is out of bounds
    {
        #ifdef GUI_ENABLED
        pushResultFailure(addressLine) return;
        #else
        return;
        #endif
    }

    std::memcpy(&data[addressLine], &line, sizeof(LineData));

    #ifdef GUI_ENABLED
    pushResultSuccess(line, addressLine)
    #endif
}