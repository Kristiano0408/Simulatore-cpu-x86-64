#include "addressCalculator.hpp"
#include "cpu.hpp"
#include "registerFile.hpp"
#include "bus.hpp"

namespace  AddressCalculator
{
    uint64_t indirectAddressing(Bus& bus, Register reg_address) 
    {
        return bus.getCPU().getRegisters().getReg(reg_address).raw();
    }

    uint64_t registerAddressing(Bus& bus, Register reg_value) 
    {
        return bus.getCPU().getRegisters().getReg(reg_value).raw();
    }

    uint64_t BaseAddressing(Bus& bus, Register base)
    {
        return bus.getCPU().getRegisters().getReg(base).raw();
    }

    uint64_t BaseScaleAddressing(Bus& bus, Register base, uint8_t scale)
    {
        return bus.getCPU().getRegisters().getReg(base).raw() * ScaleConversion(scale);
    }

    uint64_t BaseIndexAddressing(Bus& bus, Register base, Register index)
    {
        return bus.getCPU().getRegisters().getReg(base).raw() + bus.getCPU().getRegisters().getReg(index).raw();
    }

    uint64_t BaseIndexScaleAddressing(Bus& bus, Register base, Register index, uint8_t scale)
    {
        return bus.getCPU().getRegisters().getReg(base).raw() + bus.getCPU().getRegisters().getReg(index).raw() * ScaleConversion(scale);
    }

    uint64_t BaseIndexScaleDisplacementAddressing(Bus& bus, Register base, Register index, uint8_t scale, uint64_t displacement)
    {
        return bus.getCPU().getRegisters().getReg(base).raw() + bus.getCPU().getRegisters().getReg(index).raw() * ScaleConversion(scale) + displacement;
    }

    uint64_t BaseIndexDDisplacementAddressing(Bus& bus, Register base, Register index, uint64_t displacement)
    {
        return bus.getCPU().getRegisters().getReg(base).raw() + bus.getCPU().getRegisters().getReg(index).raw() + displacement;
    }

    uint64_t BaseScaleDisplacementAddressing(Bus& bus, Register base, uint8_t scale, uint64_t displacement)
    {
        return bus.getCPU().getRegisters().getReg(base).raw() * ScaleConversion(scale) + displacement;
    }

    uint64_t BaseDisplacementAddressing(Bus& bus, Register base, uint64_t displacement)
    {
        return bus.getCPU().getRegisters().getReg(base).raw() + displacement;
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


















