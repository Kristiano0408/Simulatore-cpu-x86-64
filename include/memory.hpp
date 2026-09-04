#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "device.hpp"
#include <cstdint>
#include <cstring>
#include <vector>

class Bus; // Forward declaration of Bus class

// Class that represents the memory RAM of the CPU
class Memory : public FaultDevice
{
public:
    Memory(size_t memorySize, Bus& busRef); // constructor that takes the size of the memory and a reference to the Bus

    // methods for reading and writing from memory
    LineData read(uint64_t address);
    void write(uint64_t address, LineData line);

    uint8_t readTest(uint64_t addressLine);
    void writeTest(uint64_t addressLine, uint8_t value);

    // get the size of the memory
    size_t getSize() const { return size; }

    // get the data of the memory
    const std::vector<uint8_t>& getData() const { return data; }

    // set the data of the memory
    void setData(const std::vector<uint8_t>& d) { data = d; }

    void setDataPartial(const std::vector<uint8_t>& newData, size_t offset);

    // clear the memory
    void clear();

    // get the stack pointer
    uint64_t getStackPointer() const { return RSP; }

    // set the stack pointer
    void setStackPointer(uint64_t value) { RSP = value; }

    // get the base pointer
    uint64_t getBasePointer() const { return RBP; }

    // set the base pointer
    void setBasePointer(uint64_t value) { RBP = value; }

    // push and pop methods for the stack
    void push(uint64_t value);

    uint64_t pop();

private:
    std::vector<uint8_t> data;
    uint64_t& RSP;
    uint64_t& RBP;
    size_t size;
    size_t size_stack;
};

#endif // MEMORY_HPP
