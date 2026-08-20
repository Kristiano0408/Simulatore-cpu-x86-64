#ifndef INSTRUCTION_CODE_MAP_HPP
#define INSTRUCTION_CODE_MAP_HPP

#include <cstdint>
#include <array>
#include "helpers.hpp"
#include "third_party/unordered_dense.h"

class Instruction;
struct InstructionInfo;



//map for the instructions types(for every key(opcode) there is a value(type of instruction, addressing mode. execution mode))
extern ankerl::unordered_dense::map<uint32_t, InstructionType_and_addMode> instructionMap;

// generic function pointer for the decode function (it takes a pointer to an instruction, a reference to InstructionInfo struct and an integer as parameters and returns void)
using DecodeFunc = void (*)(Instruction*, const InstructionInfo&, int);

// map for the decode functions (for every key(addressing mode) there is a value(decode function))
extern const std::array<DecodeFunc, (size_t)AddressingMode::COUNT> addressingModes;

 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////





#endif // INSTRUCTION_CODE_MAP_HPP