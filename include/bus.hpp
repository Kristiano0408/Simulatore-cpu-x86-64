#ifndef BUS_HPP
#define BUS_HPP

#include <cstdint>
#include <memory>
#include "helpers.hpp"
#include "clock.hpp"
#include "device.hpp"
#include "cpu.hpp"
#include "memory.hpp"



class Bus {
private:
    CPU cpu;
    Memory memory;
    Clock clock;

public:


    Bus();
    void tick(); // esegue un ciclo di clock
    CPU& getCPU() { return cpu; }
    Memory& getMemory() { return memory; }
    Clock& getClock() { return clock; }

    void resetClock();

    void memoryReset();
};

#endif // BUS_HPP
