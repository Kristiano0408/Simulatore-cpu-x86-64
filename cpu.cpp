#include "cpu.hpp"
#include "memory.hpp"
#include "registerFile.hpp"

CPU::CPU(Memory* memory) : memory(memory)
{
    //initialize the registers
    registers = RegisterFile();
    //initialize the ALU
    alu = ALU();
    //initialize the decoder
    decoder = Decoder();
    //initialize the control unit
    controlUnit = CU();

}

CPU::~CPU()
{
    //nothing to do here
}

RegisterFile& CPU::getRegisters()
{
    return registers;
}

ALU& CPU::getALU()
{
    return alu;
}


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
    registers.reset();
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