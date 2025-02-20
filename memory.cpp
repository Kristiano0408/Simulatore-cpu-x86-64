#include "memory.hpp"


//Constructor
Memory::Memory(size_t size) : size(size) {

    data.resize(size, 0); //initialize the memory with 0
};


//methods for reading and writing bytes
uint8_t Memory::readByte(uint64_t address) const
{
    return data[address];
};

void Memory::writeByte(uint64_t address, uint8_t value)
{
    data[address] = value;
};


//others methods for reading and writing 16, 32, 64 bit values
uint16_t Memory::readWord(uint64_t address) const
{
    return  (static_cast<uint16_t>(data[address]) | (static_cast<uint16_t>(data[address + 1]) << 8));

};         

void Memory::writeWord(uint64_t address, uint16_t value)
{
    data[address] = value & 0xFF;
    data[address + 1] = (value >> 8) & 0xFF;

};

uint32_t Memory::readDWord(uint64_t address) const
{
    return  (static_cast<uint32_t>(data[address]) | (static_cast<uint32_t>(data[address + 1]) << 8) | (static_cast<uint32_t>(data[address + 2]) << 16) | (static_cast<uint32_t>(data[address + 3]) << 24));

};

void Memory::writeDWord(uint64_t address, uint32_t value)
{
    while (value > 0)
    {
        data[address] = value & 0xFF;
        value >>= 8;
        address++;
    }
    

};

uint64_t Memory::readQWord(uint64_t address) const
{
    return  (static_cast<uint64_t>(data[address]) | (static_cast<uint64_t>(data[address + 1]) << 8) | (static_cast<uint64_t>(data[address + 2]) << 16) | (static_cast<uint64_t>(data[address + 3]) << 24) | (static_cast<uint64_t>(data[address + 4]) << 32) | (static_cast<uint64_t>(data[address + 5]) << 40) | (static_cast<uint64_t>(data[address + 6]) << 48) | (static_cast<uint64_t>(data[address + 7]) << 56));

};

void Memory::writeQWord(uint64_t address, uint64_t value)
{
    while (value > 0)
    {
        data[address] = value & 0xFF;
        value >>= 8;
        address++;
    }

};




//clear the memory
void Memory::clear()
{
    data.clear();
    data.resize(size, 0);
};