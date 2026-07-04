
#include "bus.hpp"
#include "clock.hpp"



Bus::Bus() :cpu(*this), memory(1024*1024, *this)
{
   
}

void Bus::tick() {

    #ifdef GUI_ENABLED
    if(cpu.getPipelineController().isPipelineStalledForGUI()) 
    {
        cpu.getPipeline().tick(); // Still tick the pipeline to allow GUI updates, but don't advance the clock
        return;
    }
    #endif
    debugLog("Bus Tick Start");
    cpu.getPipeline().tick();
    debugLog("Pipeline ticked");
    cpu.getCacheManager().tick();
    debugLog("CacheManager ticked");
    clock.tick();
}

void Bus::resetClock() {
    clock.reset();
}

void Bus::memoryReset() {
    memory.clear();
}