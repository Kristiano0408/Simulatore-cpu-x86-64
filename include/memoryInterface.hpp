#ifndef MEMORYINTERFACE_HPP
#define MEMORYINTERFACE_HPP

#include "helpers.hpp"

//in future we can add more functionality to these classes, like translation of virtual addresses to physical addresses, or other memory management features. For now, they serve as a simple interface to distinguish between instruction and data memory access.

class InstructionMemoryInterface
{
    public:
        CacheLevelType type = CacheLevelType::L1I; // Type of cache level for instruction memory interface
};

class DataMemoryInterface
{
    public:
        CacheLevelType type = CacheLevelType::L1D; // Type of cache level for data memory interface
};



#endif // MEMORYINTERFACE_HPP