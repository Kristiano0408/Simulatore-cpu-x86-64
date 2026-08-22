#include <cstdint>
#include "instruction_code_map.hpp"
#include "decoder.hpp"


ankerl::unordered_dense::map<uint32_t, InstructionType_and_addMode> instructionMap =
{
    
    {0xB0, {.type=TypeofInstruction::MOV, .mode=AddressingMode::OI, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0xB1, {.type=TypeofInstruction::MOV, .mode=AddressingMode::OI, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0xB2, {.type=TypeofInstruction::MOV, .mode=AddressingMode::OI, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0xB3, {.type=TypeofInstruction::MOV, .mode=AddressingMode::OI, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0xB4, {.type=TypeofInstruction::MOV, .mode=AddressingMode::OI, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0xB5, {.type=TypeofInstruction::MOV, .mode=AddressingMode::OI, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0xB6, {.type=TypeofInstruction::MOV, .mode=AddressingMode::OI, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0xB7, {.type=TypeofInstruction::MOV, .mode=AddressingMode::OI, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0xB8, {.type=TypeofInstruction::MOV, .mode=AddressingMode::OI, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0xB9, {.type=TypeofInstruction::MOV, .mode=AddressingMode::OI, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0xBA, {.type=TypeofInstruction::MOV, .mode=AddressingMode::OI, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0xBB, {.type=TypeofInstruction::MOV, .mode=AddressingMode::OI, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0xBC, {.type=TypeofInstruction::MOV, .mode=AddressingMode::OI, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0xBD, {.type=TypeofInstruction::MOV, .mode=AddressingMode::OI, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0xBE, {.type=TypeofInstruction::MOV, .mode=AddressingMode::OI, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0xBF, {.type=TypeofInstruction::MOV, .mode=AddressingMode::OI, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0x88, {.type=TypeofInstruction::MOV, .mode=AddressingMode::MR, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0x89, {.type=TypeofInstruction::MOV, .mode=AddressingMode::MR, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0x8A, {.type=TypeofInstruction::MOV, .mode=AddressingMode::RM, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0x8B, {.type=TypeofInstruction::MOV, .mode=AddressingMode::RM, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0xC6, {.type=TypeofInstruction::MOV, .mode=AddressingMode::MI, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0xC7, {.type=TypeofInstruction::MOV, .mode=AddressingMode::MI, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0xA0, {.type=TypeofInstruction::MOV, .mode=AddressingMode::FD, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0xA1, {.type=TypeofInstruction::MOV, .mode=AddressingMode::FD, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0xA2, {.type=TypeofInstruction::MOV, .mode=AddressingMode::TD, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},
    {0xA3, {.type=TypeofInstruction::MOV, .mode=AddressingMode::TD, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},

    // Istruzioni ADD
    {0x04, {.type=TypeofInstruction::ADD, .mode=AddressingMode::I, .executionMode=InstructionExecutionMode::ALU}},
    {0x05, {.type=TypeofInstruction::ADD, .mode=AddressingMode::I, .executionMode=InstructionExecutionMode::ALU}},
    {0x8000, {.type=TypeofInstruction::ADD, .mode=AddressingMode::MI, .executionMode=InstructionExecutionMode::ALU}},
    {0x8100, {.type=TypeofInstruction::ADD, .mode=AddressingMode::MI, .executionMode=InstructionExecutionMode::ALU}},
    {0x8300, {.type=TypeofInstruction::ADD, .mode=AddressingMode::MI, .executionMode=InstructionExecutionMode::ALU}},
    {0x00, {.type=TypeofInstruction::ADD, .mode=AddressingMode::MR, .executionMode=InstructionExecutionMode::ALU}},
    {0x01, {.type=TypeofInstruction::ADD, .mode=AddressingMode::MR, .executionMode=InstructionExecutionMode::ALU}},
    {0x02, {.type=TypeofInstruction::ADD, .mode=AddressingMode::RM, .executionMode=InstructionExecutionMode::ALU}},
    {0x03, {.type=TypeofInstruction::ADD, .mode=AddressingMode::RM, .executionMode=InstructionExecutionMode::ALU}},

    // Istruzioni SUB
    {0x2C, {.type=TypeofInstruction::SUB, .mode=AddressingMode::I, .executionMode=InstructionExecutionMode::ALU}},
    {0x2D, {.type=TypeofInstruction::SUB, .mode=AddressingMode::I, .executionMode=InstructionExecutionMode::ALU}},
    {0x8005, {.type=TypeofInstruction::SUB, .mode=AddressingMode::MI, .executionMode=InstructionExecutionMode::ALU}},
    {0x8105, {.type=TypeofInstruction::SUB, .mode=AddressingMode::MI, .executionMode=InstructionExecutionMode::ALU}},
    {0x8305, {.type=TypeofInstruction::SUB, .mode=AddressingMode::MI, .executionMode=InstructionExecutionMode::ALU}},
    {0x28, {.type=TypeofInstruction::SUB, .mode=AddressingMode::MR, .executionMode=InstructionExecutionMode::ALU}},
    {0x29, {.type=TypeofInstruction::SUB, .mode=AddressingMode::MR, .executionMode=InstructionExecutionMode::ALU}},
    {0x2A, {.type=TypeofInstruction::SUB, .mode=AddressingMode::RM, .executionMode=InstructionExecutionMode::ALU}},
    {0x2B, {.type=TypeofInstruction::SUB, .mode=AddressingMode::RM, .executionMode=InstructionExecutionMode::ALU}}


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



