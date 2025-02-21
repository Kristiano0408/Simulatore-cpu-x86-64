#include "addressingMode.hpp"
#include "cpu.hpp"


//RICORDARSI controllo overflow

int64_t DirectAddressing::getEffectiveAddress(CPU &cpu) const
{
    return address;
}

int64_t ImmediateAddressing::getEffectiveAddress(CPU &cpu) const
{
    return value;
}

int64_t IndirectAddressing::getEffectiveAddress(CPU &cpu) const
{
    return cpu.getRegisters().getRegister(reg_address);
}

int64_t RegisterAddressing::getEffectiveAddress(CPU &cpu) const
{
    return cpu.getRegisters().getRegister(reg_value);
}

int64_t BaseDisplacementAddressing::getEffectiveAddress(CPU &cpu) const
{
    return cpu.getRegisters().getRegister(baseReg) + displacement;
}

int64_t IndexedAddressing::getEffectiveAddress(CPU &cpu) const
{
    return cpu.getRegisters().getRegister(baseReg) + cpu.getRegisters().getRegister(indexReg);
}

int64_t ScaledIndexedAddressing::getEffectiveAddress(CPU &cpu) const
{
    return cpu.getRegisters().getRegister(baseReg) + cpu.getRegisters().getRegister(indexReg) * scale;
}

int64_t ScaledIndexedDisplacementAddressing::getEffectiveAddress(CPU &cpu) const
{
    return cpu.getRegisters().getRegister(baseReg) + cpu.getRegisters().getRegister(indexReg) * scale + displacement;
}

