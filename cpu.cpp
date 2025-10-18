#include "cpu.hpp"

//constructor for the CPU
CPU::CPU(Bus& bus): bus(bus), controlUnit(bus), alu(bus), registers(), cacheManager(bus, L1_cache_size, L2_cache_size, L3_cache_size, L1_cache_assoc, L2_cache_assoc, L3_cache_assoc) {}

//destructor
CPU::~CPU()
{
    // Clean up the decoded instruction if it exists
    if (decodedInstruction) {
        delete decodedInstruction;
        decodedInstruction = nullptr;
    }

}

//getters for the registers and ALU
ALU& CPU::getALU()
{
    return alu;
}

CU& CPU::getControlUnit()
{
    return controlUnit;
}

RegisterFile& CPU::getRegisters()
{
    return registers;
}

CacheManager& CPU::getCacheManager()
{
    return cacheManager;
}

//cpu operations
void CPU::cpuStart()
{
    //start the CPU
    while(true)
    {
        bus.tick(); //tick the bus, this will call cpuStep() and clock.tick()
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

    /*switch (state) {
        case CPUState::FETCH:
            // Fetch the instruction
            current_instruction = controlUnit.fetchInstruction();
            state = CPUState::DECODE;
            break;
        
        case CPUState::DECODE:
            // Decode the instruction
            decodedInstruction = controlUnit.decodeInstruction(current_instruction);
            state = CPUState::OPERAND_FETCH;
            break;

        case CPUState::OPERAND_FETCH:
            // Fetch operands for the instruction
            controlUnit.OperandFetch(decodedInstruction);
            state = CPUState::EXECUTE;
            break;

        case CPUState::EXECUTE:
            // Execute the instruction
            controlUnit.executeInstruction(decodedInstruction);
            state = CPUState::FETCH; // Go back to fetch state
            delete decodedInstruction; // Clean up the decoded instruction
            decodedInstruction = nullptr; // Reset the pointer
            break;
        

        default:
            throw std::runtime_error("Invalid CPU state");
    }

    /*
    //fetch the instruction
    InstructionInfo  instruction = controlUnit.fetchInstruction();
    //decode the instruction
    Instruction* decodedInstruction = controlUnit.decodeInstruction(instruction);

    //operand fetch
    controlUnit.OperandFetch(decodedInstruction);

    //execute the instruction
    controlUnit.executeInstruction(decodedInstruction);
    
    //delete the instruction
    delete decodedInstruction;*/
    
}