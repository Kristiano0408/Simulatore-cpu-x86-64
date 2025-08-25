#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstdint>
#include <vector>
#include <cstring>
#include <iostream>
#include "helpers.hpp"



class Bus; // Forward declaration of Bus class

// Class that represents the memory RAM of the CPU
class Memory {
public:
    Memory(size_t size, Bus& bus); //constructor that takes the size of the memory and a reference to the Bus

    //methods for reading and writing from memory
    template <typename T>
    Result<T> readGeneric(uint64_t address) 
    {
        Result<T> result;
        if (address + sizeof(T) > size) //check if the address is out of bounds
        {
            std::cerr << "Memory access out of bounds!" << std::endl; //print error message
            result.success = false;
            result.errorInfo = {ComponentType::RAM, EventType::RAM_READ_ERROR, ErrorType::OUT_OF_BOUNDS, "Memory access out of bounds!"};
            return result;
        }

        T value = 0;
        std::memcpy(&value, &data[address], sizeof(T));
        result.success = true;
        result.data = value;
        result.errorInfo = {ComponentType::RAM, EventType::NONE, ErrorType::NONE, ""};
        return result;
    }

    template <typename T>
    Result<void> writeGeneric(uint64_t address, T value) 
    {
        Result<void> result;

        if (address + sizeof(T) > size) //check if the address is out of bounds
        {
            std::cerr << "Memory access out of bounds!" << std::endl; //print error message
            result.success = false;
            result.errorInfo = {ComponentType::RAM, EventType::RAM_WRITE_ERROR, ErrorType::OUT_OF_BOUNDS, "Memory access out of bounds!"};
            return result;
        }

        std::memcpy(&data[address], &value, sizeof(T));
        result.success = true;
        result.errorInfo = {ComponentType::RAM, EventType::NONE, ErrorType::NONE, ""};
        return result;
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
    Result<void> push(uint64_t value);

    Result<uint64_t> pop();





private:
    std::vector<uint8_t> data;
    size_t size;
    size_t size_stack;
    uint64_t& RSP;
    uint64_t& RBP;




};

#endif // MEMORY_HPP
