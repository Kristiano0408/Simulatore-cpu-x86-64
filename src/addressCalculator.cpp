#include "addressCalculator.hpp"
#include "cpu.hpp"
#include "registerFile.hpp"

namespace  AddressCalculator
{
    uint64_t indirectAddressing(RegisterFile& registers, Register reg_address) 
    {
        return registers.getReg(reg_address).raw();
    }

    uint64_t registerAddressing(RegisterFile& registers, Register reg_value) 
    {
        return registers.getReg(reg_value).raw();
    }

    uint64_t BaseAddressing(RegisterFile& registers, Register base)
    {
        return registers.getReg(base).raw();
    }

    uint64_t BaseScaleAddressing(RegisterFile& registers, Register base, uint8_t scale)
    {
        return registers.getReg(base).raw() * ScaleConversion(scale);
    }

    uint64_t BaseIndexAddressing(RegisterFile& registers, Register base, Register index)
    {
        return registers.getReg(base).raw() + registers.getReg(index).raw();
    }

    uint64_t BaseIndexScaleAddressing(RegisterFile& registers, Register base, Register index, uint8_t scale)
    {
        return registers.getReg(base).raw() + registers.getReg(index).raw() * ScaleConversion(scale);
    }

    uint64_t BaseIndexScaleDisplacementAddressing(RegisterFile& registers, Register base, Register index, uint8_t scale, uint64_t displacement)
    {
        return registers.getReg(base).raw() + registers.getReg(index).raw() * ScaleConversion(scale) + displacement;
    }

    uint64_t BaseIndexDDisplacementAddressing(RegisterFile& registers, Register base, Register index, uint64_t displacement)
    {
        return registers.getReg(base).raw() + registers.getReg(index).raw() + displacement;
    }

    uint64_t BaseScaleDisplacementAddressing(RegisterFile& registers, Register base, uint8_t scale, uint64_t displacement)
    {
        return registers.getReg(base).raw() * ScaleConversion(scale) + displacement;
    }

    uint64_t BaseDisplacementAddressing(RegisterFile& registers, Register base, uint64_t displacement)
    {
        return registers.getReg(base).raw() + displacement;
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


















