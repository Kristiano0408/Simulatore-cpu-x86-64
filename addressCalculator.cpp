#include "addressCalculator.hpp"
#include "controlUnit.hpp"
#include "registerFile.hpp"

namespace  AddressCalculator
{
    uint64_t indirectAddressing(CU* controlUnit, Register reg_address) 
    {
        return controlUnit->getRegisters().getReg(reg_address).raw();
    }

    uint64_t registerAddressing(CU* controlUnit, Register reg_value) 
    {
        return controlUnit->getRegisters().getReg(reg_value).raw();
    }

    uint64_t BaseAddressing(CU* controlUnit, Register base)
    {
        return controlUnit->getRegisters().getReg(base).raw();
    }
    
    uint64_t BaseScaleAddressing(CU* controlUnit, Register base, uint8_t scale)
     {
        return controlUnit->getRegisters().getReg(base).raw() * ScaleConversion(scale);
    }

    uint64_t BaseIndexAddressing(CU* controlUnit, Register base, Register index)
    {
        return controlUnit->getRegisters().getReg(base).raw() + controlUnit->getRegisters().getReg(index).raw();
    }

    uint64_t BaseIndexScaleAddressing(CU* controlUnit,Register base, Register index, uint8_t scale)
    {
        return controlUnit->getRegisters().getReg(base).raw() + controlUnit->getRegisters().getReg(index).raw() * ScaleConversion(scale);
    }

    uint64_t BaseIndexScaleDisplacementAddressing(CU* controlUnit, Register base, Register index, uint8_t scale, uint64_t displacement)
    {
        return controlUnit->getRegisters().getReg(base).raw() + controlUnit->getRegisters().getReg(index).raw() * ScaleConversion(scale) + displacement;
    }

    uint64_t BaseIndexDIsplacementAddressing(CU* controlUnit, Register base, Register index, uint64_t displacement)
    {
        return controlUnit->getRegisters().getReg(base).raw() + controlUnit->getRegisters().getReg(index).raw() + displacement;
    }

    uint64_t BaseScaleDisplacementAddressing(CU* controlUnit, Register base, uint8_t scale, uint64_t displacement)
    {
        return controlUnit->getRegisters().getReg(base).raw() * ScaleConversion(scale) + displacement;
    }

    uint64_t BaseDisplacementAddressing(CU* controlUnit, Register base, uint64_t displacement)
    {
        return controlUnit->getRegisters().getReg(base).raw() + displacement;
    }

    int ScaleConversion(uint8_t scale)
    {
        switch (scale) {
            case 0b00:
                return 1;
            case 0b01:
                return 2;
            case 0b10:
                return 4;
            case 0b11:
                return 8;
            default:
                return 0;
        }
    }



    
} // namespace  AddressCalculator


















