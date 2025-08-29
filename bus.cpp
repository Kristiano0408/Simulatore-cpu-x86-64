
#include "bus.hpp"
#include "cpu.hpp"
#include "memory.hpp"



Bus::Bus() {
    cpu = std::make_unique<CPU>(*this);
    memory = std::make_unique<Memory>(1024*1024, *this);
    clock = std::make_unique<Clock>();
}

void Bus::tick() {
    cpu->cpuStep(); // Execute a CPU step
    clock->tick();
}

