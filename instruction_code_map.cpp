#include <unordered_map>
#include <cstdint>
#include "instruction_code_map.hpp"
#include "instruction.hpp"
#include "decoder.hpp"




std::unordered_map<uint32_t, InstructionType_and_addMode> instructionMap 
{
    
    {0xB0, {typeofInstruction::MOV, AddressingMode::OI}},
    {0xB1, {typeofInstruction::MOV, AddressingMode::OI}},
    {0xB2, {typeofInstruction::MOV, AddressingMode::OI}},
    {0xB3, {typeofInstruction::MOV, AddressingMode::OI}},
    {0xB4, {typeofInstruction::MOV, AddressingMode::OI}},
    {0xB5, {typeofInstruction::MOV, AddressingMode::OI}},
    {0xB6, {typeofInstruction::MOV, AddressingMode::OI}},
    {0xB7, {typeofInstruction::MOV, AddressingMode::OI}},
    {0xB8, {typeofInstruction::MOV, AddressingMode::OI}},
    {0xB9, {typeofInstruction::MOV, AddressingMode::OI}},
    {0xBA, {typeofInstruction::MOV, AddressingMode::OI}},
    {0xBB, {typeofInstruction::MOV, AddressingMode::OI}},
    {0xBC, {typeofInstruction::MOV, AddressingMode::OI}},
    {0xBD, {typeofInstruction::MOV, AddressingMode::OI}},
    {0xBE, {typeofInstruction::MOV, AddressingMode::OI}},
    {0xBF, {typeofInstruction::MOV, AddressingMode::OI}},
    {0x88, {typeofInstruction::MOV, AddressingMode::MR}},
    {0x89, {typeofInstruction::MOV, AddressingMode::MR}},
    {0x8A, {typeofInstruction::MOV, AddressingMode::RM}},
    {0x8B, {typeofInstruction::MOV, AddressingMode::RM}},
    {0xC6, {typeofInstruction::MOV, AddressingMode::MI}},
    {0xC7, {typeofInstruction::MOV, AddressingMode::MI}},
    {0xA0, {typeofInstruction::MOV, AddressingMode::FD}},
    {0xA1, {typeofInstruction::MOV, AddressingMode::FD}},
    {0xA2, {typeofInstruction::MOV, AddressingMode::TD}},
    {0xA3, {typeofInstruction::MOV, AddressingMode::TD}},

};


//definition of the map for the decode functions
std::unordered_map<AddressingMode, DecodeFunc> Addressing_modes
{
    {AddressingMode::OI, &Decoder::decodeInstructionOI},
    {AddressingMode::MI, &Decoder::decodeInstructionMI},
    {AddressingMode::MR, &Decoder::decodeInstructionMR},
    {AddressingMode::RM, &Decoder::decodeInstructionRM},
    {AddressingMode::FD, &Decoder::decodeInstructionFD},
    {AddressingMode::TD, &Decoder::decodeInstructionTD},
    

   
};


std::unordered_map<typeofInstruction, ConstructorFunc> instructionConstructors
{
    {typeofInstruction::MOV, []() -> Instruction* { return new MoveInstruction(); }}
};




////////////////////////////////////////////////////////////////////////////////////////////////////



