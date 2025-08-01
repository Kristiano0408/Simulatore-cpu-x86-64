#ifndef BUS_HPP
#define BUS_HPP

#include <cstdint>
#include <memory>
#include "helpers.hpp"
#include "clock.hpp"

class CPU;
class Memory;


class Bus {
private:
    std::unique_ptr<CPU> cpu;
    std::unique_ptr<Memory> memory;
    std::unique_ptr<Clock> clock;

public:
    Bus();
    void tick(); // esegue un ciclo di clock
    CPU& getCPU() { return *cpu; }
    Memory& getMemory() { return *memory; }
    Clock& getClock() { return *clock; }
};

#endif // BUS_HPP
