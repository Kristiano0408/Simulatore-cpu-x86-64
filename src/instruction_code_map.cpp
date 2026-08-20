#include <cstdint>
#include "instruction_code_map.hpp"
#include "instruction.hpp"
#include "decoder.hpp"


ankerl::unordered_dense::map<uint32_t, InstructionType_and_addMode> instructionMap =
{
    
    {0xB0, {TypeofInstruction::MOV, AddressingMode::OI, InstructionExecutionMode::DATA_TRANSFER}},
    {0xB1, {TypeofInstruction::MOV, AddressingMode::OI, InstructionExecutionMode::DATA_TRANSFER}},
    {0xB2, {TypeofInstruction::MOV, AddressingMode::OI, InstructionExecutionMode::DATA_TRANSFER}},
    {0xB3, {TypeofInstruction::MOV, AddressingMode::OI, InstructionExecutionMode::DATA_TRANSFER}},
    {0xB4, {TypeofInstruction::MOV, AddressingMode::OI, InstructionExecutionMode::DATA_TRANSFER}},
    {0xB5, {TypeofInstruction::MOV, AddressingMode::OI, InstructionExecutionMode::DATA_TRANSFER}},
    {0xB6, {TypeofInstruction::MOV, AddressingMode::OI, InstructionExecutionMode::DATA_TRANSFER}},
    {0xB7, {TypeofInstruction::MOV, AddressingMode::OI, InstructionExecutionMode::DATA_TRANSFER}},
    {0xB8, {TypeofInstruction::MOV, AddressingMode::OI, InstructionExecutionMode::DATA_TRANSFER}},
    {0xB9, {TypeofInstruction::MOV, AddressingMode::OI, InstructionExecutionMode::DATA_TRANSFER}},
    {0xBA, {TypeofInstruction::MOV, AddressingMode::OI, InstructionExecutionMode::DATA_TRANSFER}},
    {0xBB, {TypeofInstruction::MOV, AddressingMode::OI, InstructionExecutionMode::DATA_TRANSFER}},
    {0xBC, {TypeofInstruction::MOV, AddressingMode::OI, InstructionExecutionMode::DATA_TRANSFER}},
    {0xBD, {TypeofInstruction::MOV, AddressingMode::OI, InstructionExecutionMode::DATA_TRANSFER}},
    {0xBE, {TypeofInstruction::MOV, AddressingMode::OI, InstructionExecutionMode::DATA_TRANSFER}},
    {0xBF, {TypeofInstruction::MOV, AddressingMode::OI, InstructionExecutionMode::DATA_TRANSFER}},
    {0x88, {TypeofInstruction::MOV, AddressingMode::MR, InstructionExecutionMode::DATA_TRANSFER}},
    {0x89, {TypeofInstruction::MOV, AddressingMode::MR, InstructionExecutionMode::DATA_TRANSFER}},
    {0x8A, {TypeofInstruction::MOV, AddressingMode::RM, InstructionExecutionMode::DATA_TRANSFER}},
    {0x8B, {TypeofInstruction::MOV, AddressingMode::RM, InstructionExecutionMode::DATA_TRANSFER}},
    {0xC6, {TypeofInstruction::MOV, AddressingMode::MI, InstructionExecutionMode::DATA_TRANSFER}},
    {0xC7, {TypeofInstruction::MOV, AddressingMode::MI, InstructionExecutionMode::DATA_TRANSFER}},
    {0xA0, {TypeofInstruction::MOV, AddressingMode::FD, InstructionExecutionMode::DATA_TRANSFER}},
    {0xA1, {TypeofInstruction::MOV, AddressingMode::FD, InstructionExecutionMode::DATA_TRANSFER}},
    {0xA2, {TypeofInstruction::MOV, AddressingMode::TD, InstructionExecutionMode::DATA_TRANSFER}},
    {0xA3, {TypeofInstruction::MOV, AddressingMode::TD, InstructionExecutionMode::DATA_TRANSFER}},

    // Istruzioni ADD
    {0x04, {TypeofInstruction::ADD, AddressingMode::I, InstructionExecutionMode::ALU}},
    {0x05, {TypeofInstruction::ADD, AddressingMode::I, InstructionExecutionMode::ALU}},
    {0x8000, {TypeofInstruction::ADD, AddressingMode::MI, InstructionExecutionMode::ALU}},
    {0x8100, {TypeofInstruction::ADD, AddressingMode::MI, InstructionExecutionMode::ALU}},
    {0x8300, {TypeofInstruction::ADD, AddressingMode::MI, InstructionExecutionMode::ALU}},
    {0x00, {TypeofInstruction::ADD, AddressingMode::MR, InstructionExecutionMode::ALU}},
    {0x01, {TypeofInstruction::ADD, AddressingMode::MR, InstructionExecutionMode::ALU}},
    {0x02, {TypeofInstruction::ADD, AddressingMode::RM, InstructionExecutionMode::ALU}},
    {0x03, {TypeofInstruction::ADD, AddressingMode::RM, InstructionExecutionMode::ALU}},

    // Istruzioni SUB
    {0x2C, {TypeofInstruction::SUB, AddressingMode::I, InstructionExecutionMode::ALU}},
    {0x2D, {TypeofInstruction::SUB, AddressingMode::I, InstructionExecutionMode::ALU}},
    {0x8005, {TypeofInstruction::SUB, AddressingMode::MI, InstructionExecutionMode::ALU}},
    {0x8105, {TypeofInstruction::SUB, AddressingMode::MI, InstructionExecutionMode::ALU}},
    {0x8305, {TypeofInstruction::SUB, AddressingMode::MI, InstructionExecutionMode::ALU}},
    {0x28, {TypeofInstruction::SUB, AddressingMode::MR, InstructionExecutionMode::ALU}},
    {0x29, {TypeofInstruction::SUB, AddressingMode::MR, InstructionExecutionMode::ALU}},
    {0x2A, {TypeofInstruction::SUB, AddressingMode::RM, InstructionExecutionMode::ALU}},
    {0x2B, {TypeofInstruction::SUB, AddressingMode::RM, InstructionExecutionMode::ALU}}


};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const std::array<DecodeFunc,(size_t)AddressingMode::COUNT> addressingModes =
{
    //the order MUST be the same of the enum
    &Decoder::decodeInstructionI,
    &Decoder::decodeInstructionOI,
    &Decoder::decodeInstructionMI,
    &Decoder::decodeInstructionMR,
    &Decoder::decodeInstructionRM,
    &Decoder::decodeInstructionFD,
    &Decoder::decodeInstructionTD,
    
};





////////////////////////////////////////////////////////////////////////////////////////////////////



