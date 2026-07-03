#ifndef OPERAND_TYPES_HPP
#define OPERAND_TYPES_HPP

#include <cstdint>


enum class OperandStatus {
    OK,
    WAITING_MEMORY,
    ERROR
};

enum class OperandType {
    REGISTER,
    MEMORY,
    IMMEDIATE,
    NONE // Placeholder for uninitialized operand
};


struct OperandResult {
    OperandStatus status;
    uint64_t value {}; // Only valid if status is OK
};








#endif // OPERAND_TYPES_HPP