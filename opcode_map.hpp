#ifndef OPCODE_MAP_HPP
#define OPCODE_MAP_HPP

#include <unordered_map>
#include <cstdint>

// Struttura per memorizzare i dettagli dell'istruzione
struct InstructionDetails {
    size_t totalLength;
    size_t opcodeLength;
    size_t additionalBytes;
    size_t numOperands;
    size_t operandLength;
    bool hasModRM;
    bool hasDisplacement;
    bool hasImmediate;
    const char* description;
};

// Dichiarazione esterna
extern std::unordered_map<uint32_t, InstructionDetails> opcodeMap;



#endif // OPCODE_MAP_HPP