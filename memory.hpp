#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstdint>
#include <vector>

// Class that represents the memory RAM of the CPU
class Memory {
public:
    Memory(size_t size);


    //methods for reading and writing bytes
    uint8_t readByte(int64_t address) const;
    void writeByte(int64_t address, int8_t value);

    //others methods for reading and writing 16, 32, 64 bit values
    uint16_t readWord(int64_t address) const;
    void writeWord(int64_t address, int16_t value);

    uint32_t readDWord(int64_t address) const;
    void writeDWord(int64_t address, int32_t value);

    uint64_t readQWord(int64_t address) const;
    void writeQWord(int64_t address, int64_t value);

    //get the size of the memory
    size_t getSize() const;

    //get the data of the memory
    const std::vector<uint8_t>& getData() const;

    //set the data of the memory
    void setData(const std::vector<uint8_t>& data);

    //clear the memory
    void clear();


    
private:
    std::vector<uint8_t> data;
    size_t size;
};

#endif // MEMORY_HPP
