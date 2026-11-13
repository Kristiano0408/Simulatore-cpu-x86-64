#include "cpu.hpp"
#include "bus.hpp"

//constructor for the CPU
CPU::CPU(Bus& busRef): bus(busRef), controlUnit(busRef),registers(), cacheManager(busRef, L1_cache_size, L2_cache_size, L3_cache_size, L1_cache_assoc, L2_cache_assoc, L3_cache_assoc), pipeline(*this, nullptr), hazardUnit(pipeline)
{
    DEBUG_LOG(debugLog("CPU created"));
    pipeline.setEventHandler(hazardUnit.getEventHandler());
    hazardUnit.setEraseInstructionQueuedCallbackCPU(&CPU::eraseFlushedCacheResponseByInstructionIDWrapper);
    hazardUnit.setEraseInstructionQueuedCallbackCacheCtx(this);
    hazardUnit.setEraseInstructionQueuedCallbackCache(&CacheManager::eraseFlushedCacheResponseByInstructionIDWrapper);
    hazardUnit.setEraseInstructionQueuedCallbackCacheCtx(&cacheManager);
    hazardUnit.setInflightInstructionInfoCallback(&CU::getInflightInstructionInfoWrapper);
    hazardUnit.setInflightInstructionInfoCallbackCtx(&controlUnit);
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

HazardUnit& CPU::getHazardUnit()
{
    return hazardUnit;
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
    EventLog::getInstance().reset(); // Reset all event logs to prevent queue overflow
}


void CPU::execute_operation()
{
    bus.tick(); //advance the bus by one clock cycle (which advances all connected devices and synchronizes them and manages the timing and latencies)
}


void CPU::sendCacheRequest(CacheRequest&& request, CacheLevelType cacheType)
{
    cacheManager.enqueRequest(std::move(request), cacheType); //send a cache request to the cache manager
}

void CPU::eraseCacheResponseIfFound(uint64_t requestID)
{
    auto it = cacheResponseQueue.find(requestID);
    if (it != cacheResponseQueue.end()) {
        cacheResponseQueue.erase(it);
    }
}


void CPU::eraseFlushedCacheResponseByInstructionIDWrapper(uint64_t IDFromFlush, void* ctx)
{
    CPU* cpuPtr = static_cast<CPU*>(ctx);
    if (cpuPtr) {
        cpuPtr->eraseFlushedCacheResponseByInstructionID(IDFromFlush);
    }
}

void CPU::eraseFlushedCacheResponseByInstructionID(uint64_t IDFromFlush)
{
    for (auto it = cacheResponseQueue.begin(); it != cacheResponseQueue.end(); ) {
        if (it->first > IDFromFlush) {
            it = cacheResponseQueue.erase(it);
        } else {
            ++it;
        }
    }
}


void CPU::findCacheResponse(uint64_t requestID, MaxCPUInstructionLength& response, bool& found)
{
    auto it = cacheResponseQueue.find(requestID);
    if (it != cacheResponseQueue.end()) {
        response = it->second;
        found = true;
    } else {
        found = false;
    }
}