#include "memory.hpp"


//Constructor
Memory::Memory(size_t size) : size(size) {

    data.resize(size, 0); //initialize the memory with 0
};