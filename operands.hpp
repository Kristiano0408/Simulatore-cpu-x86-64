// opcode.hpp
#ifndef OPERANDS_HPP
#define OPERANDS_HPP


#include <cstdint>
#include "helpers.hpp"


class CU;  // Forward declaration of CU
class Memory;  // Forward declaration of Memory
class Instruction;  // Forward declaration OF Instruction

namespace operandFetch {
    void fetchRM(Instruction* i, CU* controlUnit, Memory* ram);
    void fetchMR(Instruction* i, CU* controlUnit, Memory* ram);
    void fetchFD(Instruction* i, CU* controlUnit, Memory* ram);
    void fetchTD(Instruction* i, CU* controlUnit, Memory* ram);
    void fetchOI(Instruction* i, CU* controlUnit, Memory* ram, uint32_t opcode);
    void fetchMI(Instruction* i, CU* controlUnit, Memory* ram);
    uint64_t calculatingAddressR_M(Instruction* i, CU* controlUnit, Memory* ram);
}

class Operand 
{
    public:
        Operand() : size(0) {} // Default constructor initializing size to 0
        void setSize(int s);
        int getSize() const;
        virtual void setValue(uint64_t v) = 0; // Pure virtual function
        virtual uint64_t getValue() = 0; // Pure virtual function




    protected:
        int size; // Size of the operand in bytes
     
    




};

class RegOperand : public Operand 
{
    public:
        RegOperand(uint64_t& reg) : reg(reg) {} // Constructor to initialize register reference
        void setValue(uint64_t v) override ;
        uint64_t getValue() override ;

    private:
        uint64_t& reg; // Reference to the register value
};


class MemOperand : public Operand 
{
    public:
        MemOperand(Memory* mem, uint64_t address) : mem(mem), address(address) {} // Constructor to initialize memory and address
        void setValue(uint64_t v) override ;
        uint64_t getValue() override ;

    private:
        Memory* mem; // Pointer to the memory object
        uint64_t address; // Address in memory
};

class ImmediateOperand : public Operand 
{
    public:
        ImmediateOperand(uint64_t value) : value(value) {} // Constructor to initialize immediate value
        void setValue(uint64_t v) override ;
        uint64_t getValue() override ;

    private:
        uint64_t value; // Immediate value
};


#endif
