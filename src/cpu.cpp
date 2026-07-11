#include "cpu.hpp"
#include "bus.hpp"

//constructor for the CPU
CPU::CPU(Bus& bus): bus(bus), controlUnit(bus), alu(), registers(), cacheManager(bus, L1_cache_size, L2_cache_size, L3_cache_size, L1_cache_assoc, L2_cache_assoc, L3_cache_assoc), pipeline(*this, nullptr), pipelineController(pipeline)
{
    DEBUG_LOG(debugLog("CPU created"));
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
    DEBUG_LOG(debugLog("CPU Step Start"));
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


void CPU::sendCacheRequest(CacheRequest&& request)
{
    cacheManager.enqueRequest(std::move(request));
}

void CPU::eraseCacheResponseIfFound(int requestID)
{
    auto it = cacheResponseQueue.find(requestID);
    if (it != cacheResponseQueue.end()) {
        cacheResponseQueue.erase(it);
    }
}

void CPU::findCacheResponse(int requestID, MaxCPUInstructionLength& response, bool& found)
{
    auto it = cacheResponseQueue.find(requestID);
    if (it != cacheResponseQueue.end()) {
        response = it->second;
        found = true;
    } else {
        found = false;
    }
}