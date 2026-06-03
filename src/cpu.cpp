#include "cpu.hpp"

//constructor for the CPU
CPU::CPU(Bus& bus): bus(bus), controlUnit(bus), alu(bus), registers(), cacheManager(bus, L1_cache_size, L2_cache_size, L3_cache_size, L1_cache_assoc, L2_cache_assoc, L3_cache_assoc), pipeline(bus, nullptr), pipelineController(pipeline)
 {
    pipeline.setEventHandler(pipelineController.getEventHandler());
  
 }

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

PipelineController& CPU::getPipelineController()
{
    return pipelineController;
}

//cpu operations
void CPU::cpuStart()
{
    setTicksNeeded(1); //set ticks needed to 1 for starting the CPU
    cpuStep(); //advance the CPU by one clock cycle
    
    
}

void CPU::cpuStep()
{
    debugLog("CPU Step Start");
    tick(); //advance the CPU by one clock cycle
}

void CPU::cpuReset()
{
    //reset the CPU
    getRegisters().reset();
    //memory->reset();
}


void CPU::execute_operation()
{
    bus.tick(); //advance the bus by one clock cycle (which advances all connected devices and synchronizes them and manages the timing and latencies)
}


void CPU::sendCacheRequest(std::unique_ptr<CacheRequest> request)
{
    cacheManager.setRequest(std::move(request));
}