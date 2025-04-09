#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstdint>
#include <vector>
#include <cstring>
#include <iostream>


class CPU; //forward declaration of the CPU class


// Class that represents the memory RAM of the CPU
class Memory {
public:
    Memory(size_t size, CPU& cpu); //constructor that takes the size of the memory and a reference to the CPU

    //methods for reading and writing from memory
    template <typename T>
    T readGeneric(uint64_t address) 
    {
        if (address + sizeof(T) > size) //check if the address is out of bounds
        {
            std::cerr << "Memory access out of bounds!" << std::endl; //print error message
            return 0; //return 0 from the function
        }

        T value = 0;
        std::memcpy(&value, &data[address], sizeof(T));
        return value;
    }

    template <typename T>
    void writeGeneric(uint64_t address, T value) 
    {
        if (address + sizeof(T) > size) //check if the address is out of bounds
        {
            std::cerr << "Memory access out of bounds!" << std::endl; //print error message
            return; //return from the function
        }


        std::memcpy(&data[address], &value, sizeof(T));
    }

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
    uint64_t& RSP;
    uint64_t& RBP;




};

#endif // MEMORY_HPP
