#include "../include/instruction.hpp"
#include <cstdint>
#include "controlUnit.hpp"
#include <unordered_set>

Instruction::Instruction()
{
    core = InstructionCore();
    flags = InstructionFlags();
    sourceOperand = std::make_unique<EmptyOperand>();
    destinationOperand = std::make_unique<EmptyOperand>();

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


//calculate the number of bits of the value/operand
uint8_t Instruction::calculatingNumberOfBits() 
{
    uint32_t opcode = core.opcode;

    if ((core.rexprefix & 0x08) != 0)
    {
        return 64;
    }

    for (size_t i = 0; i < core.numPrefixes; i++)
    {
        if (core.prefix[i] == 0x66)
        {
            DEBUG_LOG(debugLog("66 prefix"));
            return 16;
        }
    }

    static const std::unordered_set<uint32_t> opcode_8bit = {0xA0, 0xA2, 0xC6, 0x88, 0x8A, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, //mov
                                                            0x04, 0x8000,0x8300, 0x00, 0x02, //add
                                                            0x8005, 0x2C, 0x28, 0x2A, //sub
                                                            };

    if(opcode_8bit.contains(opcode))
    {
        return 8;
    }
   

    return 32;
}


