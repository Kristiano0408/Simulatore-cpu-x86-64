#include "addressingMode.hpp"
#include "controlUnit.hpp"


class CU;

//RICORDARSI controllo overflow

int64_t DirectAddressing::getEffectiveAddress(CU &cu) const
{
    return address;
}

int64_t ImmediateAddressing::getEffectiveAddress(CU &cu) const
{
    return value;
}

int64_t IndirectAddressing::getEffectiveAddress(CU &cu) const
{
    return cu.getRegisters().getRegisterValue(reg_address);
}

int64_t RegisterAddressing::getEffectiveAddress(CU &cu) const
{
    return cu.getRegisters().getRegisterValue(reg_value);
}

int64_t BaseDisplacementAddressing::getEffectiveAddress(CU &cu) const
{
    return cu.getRegisters().getRegisterValue(baseReg) + displacement;
}


int64_t IndexedAddressing::getEffectiveAddress(CU &cu) const
{
    return cu.getRegisters().getRegisterValue(baseReg) + cu.getRegisters().getRegisterValue(indexReg);
}


int64_t ScaledIndexedAddressing::getEffectiveAddress(CU &cu) const
{
    return cu.getRegisters().getRegisterValue(baseReg) + cu.getRegisters().getRegisterValue(indexReg) * scale;
}

int64_t ScaledIndexedDisplacementAddressing::getEffectiveAddress(CU &cu) const
{
    return cu.getRegisters().getRegisterValue(baseReg) + cu.getRegisters().getRegisterValue(indexReg) * scale + displacement;
}

