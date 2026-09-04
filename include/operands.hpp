// opcode.hpp
#ifndef OPERANDS_HPP
#define OPERANDS_HPP


#include "helpers/operandTypes.hpp"
#include <cstdint>



class RegisterFile;
class Memory;
class Instruction;  // Forward declaration OF Instruction

namespace operandFetch {
    void fetchRM(Instruction* i, RegisterFile& registers);
    void fetchMR(Instruction* i, RegisterFile& registers);
    void fetchFD(Instruction* i, RegisterFile& registers);
    void fetchTD(Instruction* i, RegisterFile& registers);
    void fetchOI(Instruction* i, RegisterFile& registers, uint32_t opcode);
    void fetchMI(Instruction* i, RegisterFile& registers);
    void fetchI(Instruction* i, RegisterFile& registers);
    void fetchLEA(Instruction* i, RegisterFile& registers);
    uint64_t calculatingAddressRM(Instruction* i, RegisterFile& registers);
}

class Operand 
{
    public:
        Operand() : size(0), type(OperandType::NONE) {} // Default constructor initializing size to 0

        void setSize(uint8_t s);
        uint8_t getSize() const { return size; }
        OperandType getType() const { return type; }
        void setType(OperandType t) { this->type = t; }

    protected:
        uint8_t size; // Size of the operand in bit
        OperandType type; // Type of the operand
     
    




};

// Derived class for empty operand (no value) for initialization of the unique_ptr
class EmptyOperand : public Operand {
    
    public:
        EmptyOperand() = default;

};

class RegOperand : public Operand 
{
    public:
        RegOperand(uint64_t& registerValue) : reg(registerValue) { setType(OperandType::REGISTER); } // Constructor to initialize register reference

        uint64_t& getReg() { return reg; } // Getter for the register reference


    private:
        uint64_t& reg; // Reference to the register value
};


class MemOperand : public Operand 
{
    public:
        MemOperand(uint64_t addressValue) : address(addressValue) { setType(OperandType::MEMORY); } // Constructor to initialize memory and address
        
        uint64_t getAddress() const { return address; } // Getter for the memory address
        void setAddress(uint64_t addr) { address = addr; } // Setter for the memory address

        void setReadRequestSent(bool sent) { readRequestSent = sent; } // Setter for read request sent flag
        bool isReadRequestSent() const { return readRequestSent; } // Getter for read request sent flag
        void setWriteRequestSent(bool sent) { writeRequestSent = sent; } // Setter for write request sent flag
        bool isWriteRequestSent() const { return writeRequestSent; }

    private:
        uint64_t address; // Address in memory
        bool readRequestSent = false;
        bool writeRequestSent = false;
};

class ImmediateOperand : public Operand 
{
    public:
        ImmediateOperand(uint64_t immediateValue) : value(immediateValue) { setType(OperandType::IMMEDIATE); } // Constructor to initialize immediate value
        uint64_t getValue() const { return value; } // Getter for immediate value
        void setValue(uint64_t v) { value = v; } // Setter for immediate value
        

    private:
        uint64_t value; // Immediate value
};


#endif
