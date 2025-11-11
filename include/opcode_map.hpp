#ifndef OPCODE_MAP_HPP
#define OPCODE_MAP_HPP

#include <unordered_map>
#include <cstdint>

// Struct for memorizing instruction details
struct InstructionDetails {
    size_t totalLength;
    size_t opcodeLength;
    size_t additionalBytes;
    size_t numOperands;
    size_t operandLength;
    size_t src_operand_length; // Length of source operand
    size_t dest_operand_length; // Length of destination operand
    size_t bit_extension; // Bit extension (0 if not extended, 1 for 0 extension, 2 for sign extension)
    bool rex_w_sensitive; // Indicates if the instruction is sensitive to REX.W prefix ( if 0 source and dest are 64 bit, if 1 sign extension to 64 bit for source)
    bool hasModRM;
    bool hasDisplacement;
    bool hasImmediate;
    const char* description;
};

// Dichiarazione esterna
extern std::unordered_map<uint32_t, InstructionDetails> opcodeMap;



#endif // OPCODE_MAP_HPP