#ifndef ADRESSING_MODE_HPP
#define ADRESSING_MODE_HPP

#include <cstdint>
#include <string>
#include "cpu.hpp"

class AddressingMode
{
    public:
        virtual ~AddressingMode() {};
        virtual uint64_t getEffectiveAddress(CPU &cpu) const = 0;
    

};


// Direct Addressing (instruction with direct address)
class DirectAddressing : public AddressingMode
{
    private:
        uint64_t address;

    public:
        DirectAddressing(uint64_t address) : address(address) {};
        uint64_t getEffectiveAddress(CPU &cpu) const override;
};

//Immediate Addressing ( istruction with immediate value)
class ImmediateAddressing : public AddressingMode
{
    private:
        uint64_t value;

    public:
        ImmediateAddressing(uint64_t value) : value(value) {};
        uint64_t getEffectiveAddress(CPU &cpu) const override;
};

//Register Indirect Addressing (instruction and the address is in a register)
class IndirectAddressing : public AddressingMode
{
    private:
        uint64_t address;

    public:
        IndirectAddressing(uint64_t address) : address(address) {};
        uint64_t getEffectiveAddress(CPU &cpu) const override;
};

//Register Addressing (instruction with the value in a register)
class RegisterAddressing : public AddressingMode
{
    private:
        uint64_t reg;

    public:
        RegisterAddressing(uint64_t reg) : reg(reg) {};
        uint64_t getEffectiveAddress(CPU &cpu) const override;
};

//Base Displacement Addressing (instruction with base address and displacement)
class BaseDisplacementAddressing : public AddressingMode {
    private:
        uint8_t baseReg;
        int64_t displacement;
    
    public:
        BaseDisplacementAddressing(uint8_t baseReg, int64_t displacement) 
            : baseReg(baseReg), displacement(displacement) {}
    
        uint64_t getEffectiveAddress(CPU &cpu) const override;
    };

//Indexed Addressing** (Es. `[RBX + RCX]`)
class IndexedAddressing : public AddressingMode {
    private:
        uint8_t baseReg;
        uint8_t indexReg;
    
    public:
        IndexedAddressing(uint8_t baseReg, uint8_t indexReg)
            : baseReg(baseReg), indexReg(indexReg) {}
    
        uint64_t getEffectiveAddress(CPU &cpu) const override;
    };

//Scaled Indexed Addressing** (Es. `[RBX + RCX * 4]`)
class ScaledIndexedAddressing : public AddressingMode {
    private:
        uint8_t baseReg;
        uint8_t indexReg;
        uint8_t scale;
    
    public:
        ScaledIndexedAddressing(uint8_t baseReg, uint8_t indexReg, uint8_t scale)
            : baseReg(baseReg), indexReg(indexReg), scale(scale) {}
    
        uint64_t getEffectiveAddress(CPU &cpu) const override;
    };

//Scaled Indexed Displacement Addressing** (Es. `[RBX + RCX * 4 + 8]`)
class ScaledIndexedDisplacementAddressing : public AddressingMode {
        private:
            uint8_t baseReg;
            uint8_t indexReg;
            uint8_t scale;
            int64_t displacement;
        
        public:
            ScaledIndexedDisplacementAddressing(uint8_t baseReg, uint8_t indexReg, uint8_t scale, int64_t displacement)
                : baseReg(baseReg), indexReg(indexReg), scale(scale), displacement(displacement) {}
        
            uint64_t getEffectiveAddress(CPU &cpu) const override;
        };











#endif // ADRESSING_MODE_HPP


