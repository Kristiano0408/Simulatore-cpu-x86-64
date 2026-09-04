#ifndef OPERAND_TYPES_HPP
#define OPERAND_TYPES_HPP

#include <cstdint>


enum class OperandStatus : uint8_t 
{
    OK,
    WAITING_MEMORY,
    ERROR
};

enum class OperandType : uint8_t 
{
    REGISTER = 0,
    MEMORY = 1,
    IMMEDIATE = 2,
    NONE = 3 // Placeholder for uninitialized operand
};


struct OperandResult {
    OperandStatus status;
    uint64_t value {}; // Only valid if status is OK
    
};








#endif // OPERAND_TYPES_HPP