#include "../include/instruction.hpp"
#include <cstdint>
#include "controlUnit.hpp"
#include "bus.hpp"
#include "registerFile.hpp"
#include "cpu.hpp"

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

uint64_t Instruction::castingValue(uint64_t value, uint8_t nbit) 
{
    switch (nbit)
    {
        case 8:
            return castTo<uint8_t>(value);
        case 16:
            return castTo<uint16_t>(value);
        case 32:
            return castTo<uint32_t>(value);
        case 64:
            return castTo<uint64_t>(value);
        default:
            std::cerr << "Invalid number of bits" << std::endl;
            return 0; // or throw an exception 

    }
}


//getters and setters for the operands
void Instruction::setSourceOperand(std::unique_ptr<Operand> sourceOperand) {
    this->sourceOperand = std::move(sourceOperand); // move the unique_ptr to the member variable

}

Operand* Instruction::getSourceOperand() {
    return sourceOperand.get(); // return the raw pointer of the unique_ptr
}

void Instruction::setDestinationOperand(std::unique_ptr<Operand> destinationOperand) 
{
    this->destinationOperand = std::move(destinationOperand); // move the unique_ptr to the member variable
}

Operand* Instruction::getDestinationOperand() {
    return destinationOperand.get(); // return the raw pointer of the unique_ptr
}


//calculate the number of bits of the value/operand
uint8_t Instruction::calculating_number_of_bits() 
{
    uint32_t opcode = core.opcode;

    if (core.rexprefix & 0x08)
    {
        return 64;
    }

    for (int i = 0; i < core.numPrefixes; i++)
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

    if(opcode_8bit.count(opcode))
    {
        return 8;
    }
   

    return 32;
}

//get the addressing mode

uint64_t Instruction::mask(uint8_t nbit) 
{
    switch (nbit)
    {
        case 8:
            return 0xFF;
        case 16:
            return 0xFFFF;
        case 32:
            return 0xFFFFFFFF;
        case 64:
            return 0xFFFFFFFFFFFFFFFF;
        default:
            std::cerr << "Invalid number of bits" << std::endl;
            return 0; // or throw an exception 

    }
}


///////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////



