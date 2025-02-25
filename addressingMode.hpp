#ifndef ADDRESSING_MODE_HPP
#define ADDRESSING_MODE_HPP

#include <cstdint>
#include <string>
#include "cpu.hpp"



class AddressingMode
{
    public:
        virtual ~AddressingMode() {};
        virtual int64_t getEffectiveAddress(CU &cu) const = 0;
    

};


// Direct Addressing (instruction with direct address)
class DirectAddressing : public AddressingMode
{
    private:
        int64_t address;

    public:
        DirectAddressing(int64_t address) : address(address) {};
        int64_t getEffectiveAddress(CU &cu) const override;
};

//Immediate Addressing ( istruction with immediate value)
class ImmediateAddressing : public AddressingMode
{
    private:
        int64_t value;

    public:
        ImmediateAddressing(int64_t value) : value(value) {};
        int64_t getEffectiveAddress(CU &cu) const override;
};

//Register Indirect Addressing (instruction and the address is in a register)
class IndirectAddressing : public AddressingMode
{
    private:
        const std::string& reg_address;

    public:
        IndirectAddressing(const std::string& reg_address) : reg_address(reg_address) {};
        int64_t getEffectiveAddress(CU &cu) const override;
};

//Register Addressing (instruction with the value in a register)
class RegisterAddressing : public AddressingMode
{
    private:
        const std::string& reg_value;

    public:
        RegisterAddressing(const std::string& reg_value) : reg_value(reg_value) {};
        int64_t getEffectiveAddress(CU &cu) const override;
};

//Base Displacement Addressing (instruction with base address and displacement)
class BaseDisplacementAddressing : public AddressingMode {
    private:
        const std::string& baseReg;
        int64_t displacement;
    
    public:
        BaseDisplacementAddressing(const std::string& baseReg, int64_t displacement) 
            : baseReg(baseReg), displacement(displacement) {}
    
        int64_t getEffectiveAddress(CU &cu) const override;
    };

//Indexed Addressing** (Es. `[RBX + RCX]`)
class IndexedAddressing : public AddressingMode {
    private:
        const std::string& baseReg;
        const std::string& indexReg;
    
    public:
        IndexedAddressing(const std::string& baseReg, const std::string& indexReg)
            : baseReg(baseReg), indexReg(indexReg) {}
    
        int64_t getEffectiveAddress(CU &cu) const override;
    };

//Scaled Indexed Addressing** (Es. `[RBX + RCX * 4]`)
class ScaledIndexedAddressing : public AddressingMode {
    private:
        const std::string& baseReg;
        const std::string& indexReg;
        int8_t scale;
    
    public:
        ScaledIndexedAddressing(const std::string& baseReg, const std::string& indexReg, int8_t scale)
            : baseReg(baseReg), indexReg(indexReg), scale(scale) {}
        int64_t getEffectiveAddress(CU &cu) const override;
    };

//Scaled Indexed Displacement Addressing** (Es. `[RBX + RCX * 4 + 8]`)
class ScaledIndexedDisplacementAddressing : public AddressingMode {
        private:
            const std::string& baseReg;
            const std::string& indexReg;
            int8_t scale;
            int64_t displacement;
        
        public:

            ScaledIndexedDisplacementAddressing(const std::string& baseReg, const std::string& indexReg, int8_t scale, int64_t displacement)
                : baseReg(baseReg), indexReg(indexReg), scale(scale), displacement(displacement) {}
        
            int64_t getEffectiveAddress(CU &cu) const override;
        };











#endif // ADRESSING_MODE_HPP


