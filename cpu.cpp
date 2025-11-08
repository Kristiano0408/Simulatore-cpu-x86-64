#include "cpu.hpp"

//constructor for the CPU
CPU::CPU(Bus& bus): bus(bus), controlUnit(bus), alu(bus), registers(), cacheManager(bus, L1_cache_size, L2_cache_size, L3_cache_size, L1_cache_assoc, L2_cache_assoc, L3_cache_assoc), pipeline(bus) {}

//destructor
CPU::~CPU(){}

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

Pipeline& CPU::getPipeline()
{
    return pipeline;
}

//cpu operations
void CPU::cpuStart()
{
    
}

void CPU::cpuReset()
{
    //reset the CPU
    getRegisters().reset();
    //memory->reset();
}

void CPU::cpuStep()
{
    //we non use this anymorre because we have the pipeline

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

void CPU::execute_operation()
{
    bus.tick(); //advance the bus by one clock cycle (which advances all connected devices and synchronizes them and manages the timing and latencies)
}


void CPU::sendCacheRequest(std::unique_ptr<CacheRequest<anydata>> request)
{
    cacheManager.setRequest(std::move(request));
}