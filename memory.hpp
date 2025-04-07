#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstdint>
#include <vector>

class CPU; //forward declaration of the CPU class

// Class that represents the memory RAM of the CPU
class Memory {
public:
    Memory(size_t size, CPU& cpu); //constructor that takes the size of the memory and a reference to the CPU


    //methods for reading and writing bytes
    uint8_t readByte(uint64_t address) const;
    void writeByte(uint64_t address, uint8_t value);

    //others methods for reading and writing 16, 32, 64 bit values
    uint16_t readWord(uint64_t address) const;
    void writeWord(uint64_t address, uint16_t value);

    uint32_t readDWord(uint64_t address) const;
    void writeDWord(uint64_t address, uint32_t value);

    uint64_t readQWord(uint64_t address) const;
    void writeQWord(uint64_t address, uint64_t value);

    //get the size of the memory
    size_t getSize() const;

    //get the data of the memory
    const std::vector<uint8_t>& getData() const;

    //set the data of the memory
    void setData(const std::vector<uint8_t>& data);

    //clear the memory
    void clear();


    //get the stack pointer
    uint64_t getStackPointer() const;

    //set the stack pointer
    void setStackPointer(uint64_t value);

    //get the base pointer
    uint64_t getBasePointer() const;

    //set the base pointer
    void setBasePointer(uint64_t value);

    //push and pop methods for the stack
    void push(uint64_t value);

    uint64_t pop();





private:
    std::vector<uint8_t> data;
    size_t size;
    size_t size_stack;
    uint64_t* RSP;
    uint64_t* RBP;




};

#endif // MEMORY_HPP
