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

    // Istruzioni ADD
    {0x04, {typeofInstruction::ADD, AddressingMode::I}},
    {0x05, {typeofInstruction::ADD, AddressingMode::I}},
    {0x8000, {typeofInstruction::ADD, AddressingMode::MI}},
    {0x8100, {typeofInstruction::ADD, AddressingMode::MI}},
    {0x8300, {typeofInstruction::ADD, AddressingMode::MI}},
    {0x00, {typeofInstruction::ADD, AddressingMode::MR}},
    {0x01, {typeofInstruction::ADD, AddressingMode::MR}},
    {0x02, {typeofInstruction::ADD, AddressingMode::RM}},
    {0x03, {typeofInstruction::ADD, AddressingMode::RM}},

    // Istruzioni SUB
    {0x2C, {typeofInstruction::SUB, AddressingMode::I}},
    {0x2D, {typeofInstruction::SUB, AddressingMode::I}},
    {0x8005, {typeofInstruction::SUB, AddressingMode::MI}},
    {0x8105, {typeofInstruction::SUB, AddressingMode::MI}},
    {0x8305, {typeofInstruction::SUB, AddressingMode::MI}},
    {0x28, {typeofInstruction::SUB, AddressingMode::MR}},
    {0x29, {typeofInstruction::SUB, AddressingMode::MR}},
    {0x2A, {typeofInstruction::SUB, AddressingMode::RM}},
    {0x2B, {typeofInstruction::SUB, AddressingMode::RM}},


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
    {AddressingMode::I, &Decoder::decodeInstructionI},
    

   
};



std::unordered_map<typeofInstruction, std::function<std::unique_ptr<Instruction>()>> instructionConstructors 
{
    {typeofInstruction::MOV, [](){ return std::make_unique<MoveInstruction>(); }},
    {typeofInstruction::ADD, [](){ return std::make_unique<AddInstruction>(); }},
    {typeofInstruction::SUB, [](){ return std::make_unique<SubInstruction>(); }}
};



////////////////////////////////////////////////////////////////////////////////////////////////////



