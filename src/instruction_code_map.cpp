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
    {0x2B, {.type=TypeofInstruction::SUB, .mode=AddressingMode::RM, .executionMode=InstructionExecutionMode::ALU}},

    // Istruzioni CMP
    {0x3C, {.type=TypeofInstruction::CMP, .mode=AddressingMode::I,  .executionMode=InstructionExecutionMode::ALU}},
    {0x3D, {.type=TypeofInstruction::CMP, .mode=AddressingMode::I,  .executionMode=InstructionExecutionMode::ALU}},
    {0x8007, {.type=TypeofInstruction::CMP, .mode=AddressingMode::MI, .executionMode=InstructionExecutionMode::ALU}},
    {0x8107, {.type=TypeofInstruction::CMP, .mode=AddressingMode::MI, .executionMode=InstructionExecutionMode::ALU}},
    {0x8307, {.type=TypeofInstruction::CMP, .mode=AddressingMode::MI, .executionMode=InstructionExecutionMode::ALU}},
    {0x38, {.type=TypeofInstruction::CMP, .mode=AddressingMode::MR, .executionMode=InstructionExecutionMode::ALU}},
    {0x39, {.type=TypeofInstruction::CMP, .mode=AddressingMode::MR, .executionMode=InstructionExecutionMode::ALU}},
    {0x3A, {.type=TypeofInstruction::CMP, .mode=AddressingMode::RM, .executionMode=InstructionExecutionMode::ALU}},
    {0x3B, {.type=TypeofInstruction::CMP, .mode=AddressingMode::RM, .executionMode=InstructionExecutionMode::ALU}},

    // Istruzioni ADC
    {0x14, {.type=TypeofInstruction::ADC, .mode=AddressingMode::I,  .executionMode=InstructionExecutionMode::ALU}},
    {0x15, {.type=TypeofInstruction::ADC, .mode=AddressingMode::I,  .executionMode=InstructionExecutionMode::ALU}},
    {0x8002, {.type=TypeofInstruction::ADC, .mode=AddressingMode::MI, .executionMode=InstructionExecutionMode::ALU}},
    {0x8102, {.type=TypeofInstruction::ADC, .mode=AddressingMode::MI, .executionMode=InstructionExecutionMode::ALU}},
    {0x8302, {.type=TypeofInstruction::ADC, .mode=AddressingMode::MI, .executionMode=InstructionExecutionMode::ALU}},
    {0x10, {.type=TypeofInstruction::ADC, .mode=AddressingMode::MR, .executionMode=InstructionExecutionMode::ALU}},
    {0x11, {.type=TypeofInstruction::ADC, .mode=AddressingMode::MR, .executionMode=InstructionExecutionMode::ALU}},
    {0x12, {.type=TypeofInstruction::ADC, .mode=AddressingMode::RM, .executionMode=InstructionExecutionMode::ALU}},
    {0x13, {.type=TypeofInstruction::ADC, .mode=AddressingMode::RM, .executionMode=InstructionExecutionMode::ALU}},

    // Istruzioni SBB
    {0x1C, {.type=TypeofInstruction::SBB, .mode=AddressingMode::I,  .executionMode=InstructionExecutionMode::ALU}},
    {0x1D, {.type=TypeofInstruction::SBB, .mode=AddressingMode::I,  .executionMode=InstructionExecutionMode::ALU}},
    {0x8003, {.type=TypeofInstruction::SBB, .mode=AddressingMode::MI, .executionMode=InstructionExecutionMode::ALU}},
    {0x8103, {.type=TypeofInstruction::SBB, .mode=AddressingMode::MI, .executionMode=InstructionExecutionMode::ALU}},
    {0x8303, {.type=TypeofInstruction::SBB, .mode=AddressingMode::MI, .executionMode=InstructionExecutionMode::ALU}},
    {0x18, {.type=TypeofInstruction::SBB, .mode=AddressingMode::MR, .executionMode=InstructionExecutionMode::ALU}},
    {0x19, {.type=TypeofInstruction::SBB, .mode=AddressingMode::MR, .executionMode=InstructionExecutionMode::ALU}},
    {0x1A, {.type=TypeofInstruction::SBB, .mode=AddressingMode::RM, .executionMode=InstructionExecutionMode::ALU}},
    {0x1B, {.type=TypeofInstruction::SBB, .mode=AddressingMode::RM, .executionMode=InstructionExecutionMode::ALU}},

    // Istruzioni INC / DEC
    {0xFE00, {.type=TypeofInstruction::INC, .mode=AddressingMode::M,  .executionMode=InstructionExecutionMode::ALU}},
    {0xFF00, {.type=TypeofInstruction::INC, .mode=AddressingMode::M,  .executionMode=InstructionExecutionMode::ALU}},
    {0xFE01, {.type=TypeofInstruction::DEC, .mode=AddressingMode::M,  .executionMode=InstructionExecutionMode::ALU}},
    {0xFF01, {.type=TypeofInstruction::DEC, .mode=AddressingMode::M,  .executionMode=InstructionExecutionMode::ALU}},

    // Istruzioni NEG (Gruppo 3, reg = 011b)
    {0xF603, {.type=TypeofInstruction::NEG, .mode=AddressingMode::M,  .executionMode=InstructionExecutionMode::ALU}},
    {0xF703, {.type=TypeofInstruction::NEG, .mode=AddressingMode::M,  .executionMode=InstructionExecutionMode::ALU}},

    // Istruzioni LEA
    {0x8D, {.type=TypeofInstruction::LEA, .mode=AddressingMode::RM, .executionMode=InstructionExecutionMode::DATA_TRANSFER}},

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
    &Decoder::decodeInstructionM,
};





////////////////////////////////////////////////////////////////////////////////////////////////////



