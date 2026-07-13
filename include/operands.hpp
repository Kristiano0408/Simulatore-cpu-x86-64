// opcode.hpp
#ifndef OPERANDS_HPP
#define OPERANDS_HPP


#include <cstdint>
#include <memory>
#include "helpers.hpp"
#include "cache/cacheManager.hpp"
#include <functional>

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
    uint64_t calculatingAddressR_M(Instruction* i, RegisterFile& registers);
}

class Operand 
{
    public:
        Operand() : size(0), type(OperandType::NONE) {} // Default constructor initializing size to 0
        void setSize(int s);
        int getSize() const;
        OperandType getType() const;
        void setType(OperandType t);

        //virtual OperandResult setValue(uint64_t v, std::function<void()> callback) = 0; // Pure virtual function
        //virtual OperandResult getValue(std::function<void()> callback) = 0; // Pure virtual function

    protected:
        int size; // Size of the operand in bytes
        OperandType type; // Type of the operand
     
    




};

// Derived class for empty operand (no value) for initialization of the unique_ptr
class EmptyOperand : public Operand {
    
    public:
        EmptyOperand() = default;

        //OperandResult setValue([[maybe_unused]] uint64_t v, [[maybe_unused]] std::function<void()> callback) override { return {OperandStatus::OK, 0}; }
        //OperandResult getValue([[maybe_unused]] std::function<void()> callback) override { return {OperandStatus::OK, 0}; }
};

class RegOperand : public Operand 
{
    public:
        RegOperand(uint64_t& reg) : reg(reg) { setType(OperandType::REGISTER); } // Constructor to initialize register reference
        //OperandResult setValue(uint64_t v, [[maybe_unused]] std::function<void()> callback) override;
        //OperandResult getValue([[maybe_unused]] std::function<void()> callback) override;
        uint64_t& getReg() { return reg; } // Getter for the register reference


    private:
        uint64_t& reg; // Reference to the register value
};


class MemOperand : public Operand 
{
    public:
        MemOperand(uint64_t address) : address(address) { setType(OperandType::MEMORY); } // Constructor to initialize memory and address
        //OperandResult setValue(uint64_t v, std::function<void()> callback) override;
        //OperandResult getValue(std::function<void()> callback) override;
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
        ImmediateOperand(uint64_t value) : value(value) { setType(OperandType::IMMEDIATE); } // Constructor to initialize immediate value
        uint64_t getValue() const { return value; } // Getter for immediate value
        void setValue(uint64_t v) { value = v; } // Setter for immediate value
        

    private:
        uint64_t value; // Immediate value
};


#endif
