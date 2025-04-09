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
};

//enum for the addressing mode of the instruction
enum class AddressingMode
{
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


//map for the instructions types 
extern std::unordered_map<uint32_t, InstructionType_and_addMode> instructionMap;

//generic function pointer for the constructor of the instruction
using  ConstructorFunc = std::function<std::unique_ptr<Instruction>()>;

// map for the constructors of the instructions
extern std::unordered_map<typeofInstruction, ConstructorFunc> instructionConstructors;


// generic function pointer for the decode function
using DecodeFunc = void (*)(Instruction*, const InstructionInfo&, int);

// map for the decode functions
extern std::unordered_map<AddressingMode, DecodeFunc> Addressing_modes;

 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////





#endif // INSTRUCTION_CODE_MAP_HPP