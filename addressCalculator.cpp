#include "addressCalculator.hpp"
#include "controlUnit.hpp"

namespace  AddressCalculator
{
    uint64_t indirectAddressing(CU* controlUnit, const std::string& reg_address) 
    {
        return controlUnit->getRegisters().getRegisterValue(reg_address);
    }

    uint64_t registerAddressing(CU* controlUnit, const std::string& reg_value)  
    {
        return controlUnit->getRegisters().getRegisterValue(reg_value);
    }

    uint64_t BaseAddressing(CU* controlUnit, const std::string& base) 
    {
        return controlUnit->getRegisters().getRegisterValue(base);
    }
    
    uint64_t BaseScaleAddressing(CU* controlUnit, const std::string& base, uint8_t scale)
     {
        return controlUnit->getRegisters().getRegisterValue(base) * ScaleConversion(scale);
    }

    uint64_t BaseIndexAddressing(CU* controlUnit, const std::string& base, const std::string& index)
    {
        return controlUnit->getRegisters().getRegisterValue(base) + controlUnit->getRegisters().getRegisterValue(index);
    }

    uint64_t BaseIndexScaleAddressing(CU* controlUnit,const std::string& base, const std::string& index, uint8_t scale)
    {
        return controlUnit->getRegisters().getRegisterValue(base) + controlUnit->getRegisters().getRegisterValue(index) * ScaleConversion(scale);
    }

    uint64_t BaseIndexScaleDisplacementAddressing(CU* controlUnit, const std::string& base, const std::string& index, uint8_t scale, uint64_t displacement)
    {
        return controlUnit->getRegisters().getRegisterValue(base) + controlUnit->getRegisters().getRegisterValue(index) * ScaleConversion(scale) + displacement;
    }

    uint64_t BaseIndexDIsplacementAddressing(CU* controlUnit, const std::string& base, const std::string& index, uint64_t displacement)
    {
        return controlUnit->getRegisters().getRegisterValue(base) + controlUnit->getRegisters().getRegisterValue(index) + displacement;
    }

    uint64_t BaseScaleDisplacementAddressing(CU* controlUnit, const std::string& base, uint8_t scale, uint64_t displacement)
    {
        return controlUnit->getRegisters().getRegisterValue(base) * ScaleConversion(scale) + displacement;
    }

    uint64_t BaseDisplacementAddressing(CU* controlUnit, const std::string& base, uint64_t displacement)
    {
        return controlUnit->getRegisters().getRegisterValue(base) + displacement;
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


















