#include "memory.hpp"
#include "cpu.hpp"
#include <cmath>
//ricordarsi di convertire indirizzi dec per accedere al vettore e in binario per utilizzare l'indirizzo

//ricordasri controllo offset e size per evitare buffer overflow

//Constructor
Memory::Memory(size_t size, CPU& cpu) : size(size), RSP(cpu.getRegisters().getReg(Register::RSP).raw()),
                                         RBP(cpu.getRegisters().getReg(Register::RBP).raw()) {
    

    data.resize(size, 0); //initialize the memory with 0

    size_stack = size / 4; //initialize the stack size to 1/4 of the memory size

    RSP = size - 1; //initialize the stack pointer to the end of the memory
    RBP = size - 1; //initialize the base pointer to the end of the memory


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


//get the size of the memory
size_t Memory::getSize() const
{
    return size;
};

//get the data of the memory
const std::vector<uint8_t>& Memory::getData() const
{
    return data;
};

//set the data of the memory
void Memory::setData(const std::vector<uint8_t>& data)
{
    this->data = data;
};

//clear the memory
void Memory::clear()
{
    data.clear();
    data.resize(size, 0);
};

void Memory::setStackPointer(uint64_t value)
{
    RSP = value;
};

uint64_t Memory::getStackPointer() const
{
    return RSP;
};

void Memory::setBasePointer(uint64_t value)
{
    RBP = value;
};

uint64_t Memory::getBasePointer() const
{
    return RBP;
};

void Memory::push(uint64_t value)
{
    //controllo overflow
    if (RSP - 8 < size - size_stack) //check if the stack pointer is out of bounds
    {
        std::cerr << "Stack overflow!" << std::endl; //print error message
        return; //return from the function
    }


    RSP -= 8; //decrement the stack pointer
    writeQWord(RSP, value); //write the value in the stack
};


uint64_t Memory::pop()
{
    //controllo underflow
    if (RSP > size - 8) //check if the stack pointer is out of bounds
    {
        std::cerr << "Stack underflow!" << std::endl; //print error message
        return 0; //return 0 from the function
    }


    uint64_t value = readQWord(RSP); //read the value from the stack
    RSP += 8; //increment the stack pointer
    return value; //return the value
};