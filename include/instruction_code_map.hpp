#ifndef INSTRUCTION_CODE_MAP_HPP
#define INSTRUCTION_CODE_MAP_HPP

#include <cstdint>
#include <unordered_map>
#include <functional>
#include <memory>
#include "helpers.hpp"




class Instruction;
struct InstructionInfo;





//map for the instructions types(for every key(opcode) there is a value(type of instruction and addressing mode))
extern std::unordered_map<uint32_t, InstructionType_and_addMode> instructionMap;

//generic function pointer for the constructor of the instruction(it returns a unique pointer to an instruction)
using  ConstructorFunc = std::function<std::unique_ptr<Instruction>()>;

// map for the constructors of the instructions(for every key(type of instruction) there is a value(constructor function))
extern std::unordered_map<TypeofInstruction, ConstructorFunc> instructionConstructors;


// generic function pointer for the decode function (it takes a pointer to an instruction, a reference to InstructionInfo struct and an integer as parameters and returns void)
//using DecodeFunc = void (*)(Instruction*, const InstructionInfo&, int);
using DecodeFunc = std::function<void(Instruction*, const InstructionInfo&, int)>;
// map for the decode functions (for every key(addressing mode) there is a value(decode function))
extern std::unordered_map<AddressingMode, DecodeFunc> Addressing_modes;

 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////





#endif // INSTRUCTION_CODE_MAP_HPP