#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstdint>
#include <vector>
#include <cstring>
#include <iostream>
#include "helpers.hpp"

using LineData = std::array<uint8_t,CACHE_LINE_SIZE>;

class Bus; // Forward declaration of Bus class

// Class that represents the memory RAM of the CPU
class Memory {
public:
    Memory(size_t size, Bus& bus); //constructor that takes the size of the memory and a reference to the Bus

    //methods for reading and writing from memory
    Result<std::array<uint8_t,CACHE_LINE_SIZE>>read(uint64_t address);
   

    Result<void> write(uint64_t address, LineData line);


    //get the size of the memory
    size_t getSize() const;

    //get the data of the memory
    const std::vector<uint8_t>& getData() const;

    //set the data of the memory
    void setData(const std::vector<uint8_t>& data);

    void setDataPartial(const std::vector<uint8_t>& newData, size_t offset);

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
    Result<void> push([[maybe_unused]] uint64_t value);

    Result<uint64_t> pop();





private:
    std::vector<uint8_t> data;
    uint64_t& RSP;
    uint64_t& RBP;
    size_t size;
    size_t size_stack;
    




};

#endif // MEMORY_HPP
