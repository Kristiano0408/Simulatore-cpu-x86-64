
#include "bus.hpp"
#include "cpu.hpp"
#include "memory.hpp"
#include "clock.hpp"



Bus::Bus() {
    cpu = std::make_unique<CPU>(*this);
    memory = std::make_unique<Memory>(1024*1024, *this);
    clock = std::make_unique<Clock>();
}

void Bus::tick() {
    debugLog("Bus Tick Start");
    cpu->getPipeline().tick();
    cpu->getCacheManager().tick();
    clock->tick();
}

void Bus::resetClock() {
    clock->reset();
}

void Bus::memoryReset() {
    memory->clear();
}