#include "cpu.hpp"
#include "memory.hpp"

//constructor that receives a pointer to the memory
CPU::CPU(Memory* memory) : memory(memory), decoder(), controlUnit()
{
    //nothing to do here

}

//destructor
CPU::~CPU()
{
    //nothing to do here
}

//getters for the registers and ALU
ALU& CPU::getALU()
{
    return controlUnit.getALU();
}

/*CU& CPU::getControlUnit()
{
    return controlUnit;
}*/

RegisterFile& CPU::getRegisters()
{
    return controlUnit.getRegisters();
}

//cpu operations
void CPU::cpuStart()
{
    //start the CPU
    while(true)
    {
        cpuStep();
    }
}

void CPU::cpuReset()
{
    //reset the CPU
    getRegisters().reset();
    //memory->reset();
}

void CPU::cpuStep()
{
    //fetch the instruction
    uint64_t instruction = controlUnit.fetchInstruction();
    //decode the instruction
    Instruction* decodedInstruction = controlUnit.decodeInstruction(instruction);
    //execute the instruction
    controlUnit.executeInstruction(decodedInstruction);
    
    //delete the instruction
    delete decodedInstruction;
    
}