#include "../include/instruction.hpp"
#include <cstdint>
#include "controlUnit.hpp"
#include "../include/helpers/maskAndConst.hpp"
#include <unordered_set>

Instruction::Instruction()
{
    core = InstructionCore();
    flags = InstructionFlags();
    sourceOperand = std::make_unique<EmptyOperand>();
    destinationOperand = std::make_unique<EmptyOperand>();
    auxOperand = std::make_unique<EmptyOperand>();
}
bool Instruction::isEmpty() const 
{
    return (core.opcode == 0 || core.InstructionId == 0);
}

//getters and setters for the operands
void Instruction::setSourceOperand(std::unique_ptr<Operand> sOperand) {
    this->sourceOperand = std::move(sOperand); // move the unique_ptr to the member variable

}

Operand* Instruction::getSourceOperand() {
    return sourceOperand.get(); // return the raw pointer of the unique_ptr
}

void Instruction::setDestinationOperand(std::unique_ptr<Operand> dOperand) 
{
    this->destinationOperand = std::move(dOperand); // move the unique_ptr to the member variable
}

Operand* Instruction::getDestinationOperand() {
    return destinationOperand.get(); // return the raw pointer of the unique_ptr
}

void Instruction::setAuxOperand(std::unique_ptr<Operand> aOperand) {
    this->auxOperand = std::move(aOperand); // move the unique_ptr to the member variable
}

Operand* Instruction::getAuxOperand() {
    return auxOperand.get(); // return the raw pointer of the unique_ptr
}


//calculate the number of bits of the value/operand
void Instruction::calculatingNumberOfBits() 
{   
    InstructionCore& core = this->getCore();
    uint32_t opcode = core.opcode;

    // NOTE: operand widths are resolved here, NOT from the opcode map: the map entries are
    // defined for the 32-bit form, so REX.W / 0x66 must override the copied values.
    if (RexMask::isREX_W(static_cast<std::byte>(core.rexprefix)))
    {
        core.destNbit = 64;
    }
    else
    {
        for (size_t i = 0; i < core.numPrefixes; i++)
        {
            if (core.prefix[i] == 0x66)
            {
                DEBUG_LOG(debugLog("66 prefix"));
                core.destNbit = 16;
                break;
            }
        }
    }

    static const std::unordered_set<uint32_t> opcode_8bit = {0xA0, 0xA2, 0xC6, 0x88, 0x8A, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, //mov
                                                            0x04, 0x8000,0x8300, 0x00, 0x02, //add
                                                            0x8005, 0x2C, 0x28, 0x2A, //sub
                                                            0xFE00, 0xFE01, //inc/dec r/m8
                                                            0x14, 0x8002, 0x10, 0x12, //adc
                                                            0x1C, 0x8003, 0x18, 0x1A, //sbb
                                                            0xF603, //neg r/m8
                                                            0x3C, 0x8007, 0x38, 0x3A, //cmp
                                                            0xF604, 0xF605, //MUL/IMUL r/m8
                                                            0xF606, 0xF607, //DIV/IDIV r/m8
                                                        }; 

    if(opcode_8bit.contains(opcode))
    {
        if (core.destNbit == 0) core.destNbit = 8;
    }
    else if (core.destNbit == 0)
    {
        core.destNbit = 32; //default value
    }

    // Resolve the source operand width from the opcode (the opcode map entries are defined
    // for the 32-bit form, so the width must follow the resolved destination width):
    //   - r/m-opcode forms (MR/RM/M, group F6/F7/FE/FF/80-83 with reg operand): src == dest.
    //   - accumulator + r/m (M_ACC, MUL/IMUL/DIV/IDIV 1-op): src == dest.
    //   - accumulator + reg (OI/MO, e.g. ADD RAX,RBX / CMP RAX,RBX): src == dest.
    //   - accumulator + sign-extended immediate (04/05/0C/14/1C/18-1D/3C/3D...): src == dest
    //     (the immediate is sign-extended to the operand width before the ALU).
    //   - r/m + imm16/32 (MI, 81/83 group): src stays the immediate width (8/16/32),
    //     sign-extended to dest width by the ALU.

    const bool isExtension = (core.type == TypeofInstruction::MOVSX || core.type == TypeofInstruction::MOVSXD || core.type == TypeofInstruction::MOVZX);
    
    const bool srcFollowsDest = (!isExtension) &&
        (core.addressingMode == AddressingMode::I   ||
         core.addressingMode == AddressingMode::MR ||
         core.addressingMode == AddressingMode::RM ||
         core.addressingMode == AddressingMode::M  ||
         core.addressingMode == AddressingMode::M_ACC ||
         core.addressingMode == AddressingMode::MI);

    if (srcFollowsDest)
    {
        // For the MI (81/83) group the immediate width is 8 (imm8) or 16/32 (imm16/32,
        // or 16 with the 0x66 prefix); it never exceeds 32 bits.
        if(core.addressingMode == AddressingMode::MI && core.immediateNbit != 0)
        {
            core.srcNbit = core.immediateNbit;
        }
        else
            core.srcNbit = core.destNbit;
    }

    // auxNbit / immediateNbit are not stored in the opcode map: derive them here.
    // - IMUL RMI (3-op): the immediate is imm8 (0x6B) or imm16/imm32 (0x69, or imm16 with 0x66).
    // - M_ACC (MUL/IMUL/DIV/IDIV 1-op): the aux operand is RDX, i.e. the high half of the
    //   accumulator pair, so it has the same width as the destination (RAX).
    if (core.type == TypeofInstruction::IMUL && core.addressingMode == AddressingMode::RMI)
    {
        core.immediateNbit = (core.opcode == 0x6B) ? 8 : core.destNbit;
        core.auxNbit = 0;
    }
    else if (core.addressingMode == AddressingMode::M_ACC)
    {
        core.auxNbit = core.destNbit;
        core.immediateNbit = 0;
    }
    else
    {
        core.auxNbit = 0;
        core.immediateNbit = 0;
    }

    return;
}


