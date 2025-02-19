#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstdint>
#include <vector>

// Class that represents the memory RAM of the CPU
class Memory {
public:
    Memory(size_t size);

    uint8_t readByte(uint64_t address) const;
    void writeByte(uint64_t address, uint8_t value);
    //others methods for reading and writing 16, 32, 64 bit values


    
private:
    std::vector<uint8_t> data;
};

#endif // MEMORY_HPP
