#ifndef INSTRUCTION_CODE_MAP_HPP
#define INSTRUCTION_CODE_MAP_HPP

#include <cstdint>
#include <unordered_map>
#include <functional>
#include <memory>


//farward declaration of the classes
class Instruction;
struct InstructionInfo;


//enum for the type of instruction
enum class typeofInstruction
{
    MOV, //move instruction
    ADD, //add instruction
    SUB, //sub instruction
};

//enum for the addressing mode of the instruction
enum class AddressingMode
{
    I,  //move immediate to accumulator
    OI, //move immediate to register
    MI, //move immediate to memory/register
    MR, //move register to R/M
    RM, //move R/M to register
    FD, //move from offset to Rax
    TD, //move from Rax to offset

};

//struct for the instruction type and relative addressing mode
struct InstructionType_and_addMode
{
    typeofInstruction type;
    AddressingMode mode;
};


//map for the instructions types(for every key(opcode) there is a value(type of instruction and addressing mode))
extern std::unordered_map<uint32_t, InstructionType_and_addMode> instructionMap;

//generic function pointer for the constructor of the instruction(it returns a unique pointer to an instruction)
using  ConstructorFunc = std::function<std::unique_ptr<Instruction>()>;

// map for the constructors of the instructions(for every key(type of instruction) there is a value(constructor function))
extern std::unordered_map<typeofInstruction, ConstructorFunc> instructionConstructors;


// generic function pointer for the decode function (it takes a pointer to an instruction, a reference to InstructionInfo struct and an integer as parameters and returns void)
using DecodeFunc = void (*)(Instruction*, const InstructionInfo&, int);

// map for the decode functions (for every key(addressing mode) there is a value(decode function))
extern std::unordered_map<AddressingMode, DecodeFunc> Addressing_modes;

 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////





#endif // INSTRUCTION_CODE_MAP_HPP