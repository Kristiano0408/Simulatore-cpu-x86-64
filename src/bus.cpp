
#include "bus.hpp"

#include <cstddef>
#include "clock.hpp"



Bus::Bus() :cpu(*this), memory(static_cast<size_t>(1024*1024), *this)
{
   DEBUG_LOG(debugLog("Bus created"));
}

void Bus::tick() {

    #ifdef GUI_ENABLED
    if(cpu.getPipelineController().isPipelineStalledForGUI()) 
    {
        cpu.getPipeline().tick(); // Still tick the pipeline to allow GUI updates, but don't advance the clock
        cpu.getCacheManager().tick(); // Memory/cache requests must keep progressing while the GUI is stalled
        return;
    }
    #endif
    DEBUG_LOG(debugLog("Bus Tick Start"));
    cpu.getPipeline().tick();
    DEBUG_LOG(debugLog("Pipeline ticked"));
    cpu.getCacheManager().tick();
    DEBUG_LOG(debugLog("CacheManager ticked"));
    clock.tick();
}

void Bus::resetClock() {
    clock.reset();
}

void Bus::memoryReset() {
    memory.clear();
}