#ifndef INSTRUCTION_CODE_MAP_HPP
#define INSTRUCTION_CODE_MAP_HPP

#include <cstdint>
#include <unordered_map>


//farward declaration of the classes
class Instruction;
struct InstructionInfo;



enum class typeofInstruction
{
    MOV, //move instruction
};

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

// generic function pointer for the decode function
using DecodeFunc = void (*)(Instruction*, const InstructionInfo&, int);

// Mappa per le modalità di decodifica delle istruzioni MOV
extern std::unordered_map<AddressingMode, DecodeFunc> Addressing_modes;

 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////


enum class MOVType
{
    MOV_MR,  //move register to R/M
    MOV_RM,  //move R/M to register
    MOV_MI,  //move immediate to memory/register
    MOV_OI,  //move immediate to reg
    MOV_FD,  //move from offset to Rax
    MOV_TD,  //move from Rax to offset

};




// Mappa per le istruzioni MOV
// La chiave è l'opcode e il valore è il tipo di istruzione MOV
extern std::unordered_map<uint32_t, MOVType> move_instruction;




#endif // INSTRUCTION_CODE_MAP_HPP