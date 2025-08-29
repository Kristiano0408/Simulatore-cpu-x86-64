// opcode.hpp
#ifndef OPERANDS_HPP
#define OPERANDS_HPP


#include <cstdint>
#include <memory>
#include "helpers.hpp"
#include "cacheManager.hpp"


class Bus;
class Memory;
class Instruction;  // Forward declaration OF Instruction

namespace operandFetch {
    void fetchRM(Instruction* i, Bus& bus);
    void fetchMR(Instruction* i, Bus& bus);
    void fetchFD(Instruction* i, Bus& bus);
    void fetchTD(Instruction* i, Bus& bus);
    void fetchOI(Instruction* i, Bus& bus, uint32_t opcode);
    void fetchMI(Instruction* i, Bus& bus);
    uint64_t calculatingAddressR_M(Instruction* i, Bus& bus);
}

class Operand 
{
    public:
        Operand() : size(0) {} // Default constructor initializing size to 0
        void setSize(int s);
        int getSize() const;
        virtual Result<void> setValue(uint64_t v) = 0; // Pure virtual function
        virtual Result<uint64_t> getValue() = 0; // Pure virtual function




    protected:
        int size; // Size of the operand in bytes
     
    




};

// Derived class for empty operand (no value) for initialization of the unique_ptr
class EmptyOperand : public Operand {
    
    public:
        EmptyOperand() = default;

        Result<void> setValue([[maybe_unused]] uint64_t v) override { return {}; }
        Result<uint64_t> getValue() override { return {}; }
};

class RegOperand : public Operand 
{
    public:
        RegOperand(uint64_t& reg) : reg(reg) {} // Constructor to initialize register reference
        Result<void> setValue(uint64_t v) override;
        Result<uint64_t> getValue() override;

    private:
        uint64_t& reg; // Reference to the register value
};


class MemOperand : public Operand 
{
    public:
        MemOperand(CacheManager& cache, uint64_t address) : cache(cache), address(address) {} // Constructor to initialize memory and address
        Result<void> setValue(uint64_t v) override;
        Result<uint64_t> getValue() override;

    private:
        CacheManager& cache; // Reference to the cache manager
        uint64_t address; // Address in memory
};

class ImmediateOperand : public Operand 
{
    public:
        ImmediateOperand(uint64_t value) : value(value) {} // Constructor to initialize immediate value
        Result<void> setValue(uint64_t v) override;
        Result<uint64_t> getValue() override;

    private:
        uint64_t value; // Immediate value
};


#endif
