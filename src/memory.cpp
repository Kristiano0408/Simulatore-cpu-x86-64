#include "memory.hpp"
#include "bus.hpp"
#include "cpu.hpp"
#include "eventLog.hpp"
#include <algorithm>
#include <cmath>
// ricordarsi di convertire indirizzi dec per accedere al vettore e in binario per utilizzare l'indirizzo

// ricordasri controllo offset e size per evitare buffer overflow

// Constructor
Memory::Memory(size_t memorySize, Bus& busRef) : RSP(busRef.getCPU().getRegisters().getReg(Register::RSP).raw()), RBP(busRef.getCPU().getRegisters().getReg(Register::RBP).raw())
{

    data.resize(memorySize, 0); // initialize the memory with 0
    this->size = memorySize;
    size_stack = memorySize / 4; // initialize the stack size to 1/4 of the memory size
    RSP = this->size - 1;  // initialize the stack pointer to the end of the memory
    RBP = this->size - 1;  // initialize the base pointer to the end of the memory
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
// da sistemare
void Memory::push(uint64_t value)
{
    #ifdef GUI_ENABLED
    Result result{};
    #endif

    // controllo overflow
    if (RSP - 8 < size - size_stack) // check if the stack pointer is out of bounds
    {
        #ifdef GUI_ENABLED
        pushResultFailure(RSP) return;
        #else
        return;
        #endif
    }

    RSP -= 8;                                                             // decrement the stack pointer
    uint64_t lineAddress = RSP - (RSP % CACHE_LINE_SIZE);                 // calculate the start of the line address to write to the stack
    LineData lineData = read(lineAddress);                                // read the line from memory
    std::memcpy(&lineData[RSP % CACHE_LINE_SIZE], &value, sizeof(value)); // copy the value to the line data
    write(lineAddress, lineData);

    #ifdef GUI_ENABLED
    pushResultSuccess(lineData, RSP);
    #endif
};

/// da sistemare
uint64_t Memory::pop()
{
    uint64_t value{};
    LineData readResult{};
    #ifdef GUI_ENABLED
    Result result{};
    #endif

    // controllo underflow
    if (RSP > size - 8) // check if the stack pointer is out of bounds
    {
        #ifdef GUI_ENABLED
        pushResultFailure(RSP) return value;
        #else
        return value;
        #endif
    }

    // calculating the start of the line address to read from the stack

    uint64_t lineAddress = RSP - (RSP % CACHE_LINE_SIZE); // calculate the start of the line address to read from the stack

    readResult = read(lineAddress);
    #ifdef GUI_ENABLED
    pushResultSuccess(readResult, RSP)
    #endif

        return value;
};

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