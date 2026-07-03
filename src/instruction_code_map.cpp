#include <unordered_map>
#include <cstdint>
#include "instruction_code_map.hpp"
#include "instruction_wrapper.hpp"
#include "decoder.hpp"




std::unordered_map<uint32_t, InstructionType_and_addMode> instructionMap 
{
    
    {0xB0, {TypeofInstruction::MOV, AddressingMode::OI}},
    {0xB1, {TypeofInstruction::MOV, AddressingMode::OI}},
    {0xB2, {TypeofInstruction::MOV, AddressingMode::OI}},
    {0xB3, {TypeofInstruction::MOV, AddressingMode::OI}},
    {0xB4, {TypeofInstruction::MOV, AddressingMode::OI}},
    {0xB5, {TypeofInstruction::MOV, AddressingMode::OI}},
    {0xB6, {TypeofInstruction::MOV, AddressingMode::OI}},
    {0xB7, {TypeofInstruction::MOV, AddressingMode::OI}},
    {0xB8, {TypeofInstruction::MOV, AddressingMode::OI}},
    {0xB9, {TypeofInstruction::MOV, AddressingMode::OI}},
    {0xBA, {TypeofInstruction::MOV, AddressingMode::OI}},
    {0xBB, {TypeofInstruction::MOV, AddressingMode::OI}},
    {0xBC, {TypeofInstruction::MOV, AddressingMode::OI}},
    {0xBD, {TypeofInstruction::MOV, AddressingMode::OI}},
    {0xBE, {TypeofInstruction::MOV, AddressingMode::OI}},
    {0xBF, {TypeofInstruction::MOV, AddressingMode::OI}},
    {0x88, {TypeofInstruction::MOV, AddressingMode::MR}},
    {0x89, {TypeofInstruction::MOV, AddressingMode::MR}},
    {0x8A, {TypeofInstruction::MOV, AddressingMode::RM}},
    {0x8B, {TypeofInstruction::MOV, AddressingMode::RM}},
    {0xC6, {TypeofInstruction::MOV, AddressingMode::MI}},
    {0xC7, {TypeofInstruction::MOV, AddressingMode::MI}},
    {0xA0, {TypeofInstruction::MOV, AddressingMode::FD}},
    {0xA1, {TypeofInstruction::MOV, AddressingMode::FD}},
    {0xA2, {TypeofInstruction::MOV, AddressingMode::TD}},
    {0xA3, {TypeofInstruction::MOV, AddressingMode::TD}},

    // Istruzioni ADD
    {0x04, {TypeofInstruction::ADD, AddressingMode::I}},
    {0x05, {TypeofInstruction::ADD, AddressingMode::I}},
    {0x8000, {TypeofInstruction::ADD, AddressingMode::MI}},
    {0x8100, {TypeofInstruction::ADD, AddressingMode::MI}},
    {0x8300, {TypeofInstruction::ADD, AddressingMode::MI}},
    {0x00, {TypeofInstruction::ADD, AddressingMode::MR}},
    {0x01, {TypeofInstruction::ADD, AddressingMode::MR}},
    {0x02, {TypeofInstruction::ADD, AddressingMode::RM}},
    {0x03, {TypeofInstruction::ADD, AddressingMode::RM}},

    // Istruzioni SUB
    {0x2C, {TypeofInstruction::SUB, AddressingMode::I}},
    {0x2D, {TypeofInstruction::SUB, AddressingMode::I}},
    {0x8005, {TypeofInstruction::SUB, AddressingMode::MI}},
    {0x8105, {TypeofInstruction::SUB, AddressingMode::MI}},
    {0x8305, {TypeofInstruction::SUB, AddressingMode::MI}},
    {0x28, {TypeofInstruction::SUB, AddressingMode::MR}},
    {0x29, {TypeofInstruction::SUB, AddressingMode::MR}},
    {0x2A, {TypeofInstruction::SUB, AddressingMode::RM}},
    {0x2B, {TypeofInstruction::SUB, AddressingMode::RM}},


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



std::unordered_map<TypeofInstruction, std::function<std::unique_ptr<Instruction>()>> instructionConstructors 
{
    {TypeofInstruction::MOV, [](){ return std::make_unique<MoveInstruction>(); }},
    {TypeofInstruction::ADD, [](){ return std::make_unique<AddInstruction>(); }},
    {TypeofInstruction::SUB, [](){ return std::make_unique<SubInstruction>(); }}
};



////////////////////////////////////////////////////////////////////////////////////////////////////



