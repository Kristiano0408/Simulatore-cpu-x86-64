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
    void fetchI(Instruction* i, Bus& bus);
    uint64_t calculatingAddressR_M(Instruction* i, Bus& bus);
}

class Operand 
{
    public:
        Operand() : size(0) {} // Default constructor initializing size to 0
        void setSize(int s);
        int getSize() const;
        virtual Result<void> setValue(anydata v) = 0; // Pure virtual function
        virtual Result<anydata> getValue() = 0; // Pure virtual function




    protected:
        int size; // Size of the operand in bytes
     
    




};

// Derived class for empty operand (no value) for initialization of the unique_ptr
class EmptyOperand : public Operand {
    
    public:
        EmptyOperand() = default;

        Result<void> setValue([[maybe_unused]] anydata v) override { return {}; }
        Result<anydata> getValue() override { return {}; }
};

class RegOperand : public Operand 
{
    public:
        RegOperand(uint64_t& reg) : reg(reg) {} // Constructor to initialize register reference
        Result<void> setValue(anydata v) override;
        Result<anydata> getValue() override;

    private:
        uint64_t& reg; // Reference to the register value
};


class MemOperand : public Operand 
{
    public:
        MemOperand(Bus& bus, uint64_t address, uint64_t instructionID) : bus(bus), address(address), instructionID(instructionID) {} // Constructor to initialize memory and address
        Result<void> setValue(anydata v) override;
        Result<anydata> getValue() override;

    private:
        Bus& bus; // Reference to the bus
        uint64_t address; // Address in memory
        bool requestSent = false; // Flag to indicate if a request has been sent to the cache
        bool readRequestSent = false; // Flag to indicate if a read request has been sent to the cache
        uint64_t instructionID;
};

class ImmediateOperand : public Operand 
{
    public:
        ImmediateOperand(anydata value) : value(value) {} // Constructor to initialize immediate value
        Result<void> setValue(anydata v) override;
        Result<anydata> getValue() override;

    private:
        anydata value; // Immediate value
};


#endif
