#ifndef ADDRESSCALCULATOR_HPP
#define ADDRESSCALCULATOR_HPP

#include <cstdint>
#include <string>


class CU;

enum class Register;

namespace AddressCalculator
{
    uint64_t indirectAddressing(CU* controlUnit, Register reg_address);
    uint64_t registerAddressing(CU* controlUnit, Register reg_value);

    uint64_t BaseAddressing(CU* controlUnit, Register base);
    uint64_t BaseIndexAddressing(CU* controlUnit, Register base, Register index);
    uint64_t BaseIndexScaleAddressing(CU* controlUnit, Register base, Register index, uint8_t scale);
    uint64_t BaseIndexScaleDisplacementAddressing(CU* controlUnit,Register base, Register index, uint8_t scale, uint64_t displacement);

    uint64_t BaseIndexDIsplacementAddressing(CU* controlUnit, Register base, Register index, uint64_t displacement);
    uint64_t BaseScaleDisplacementAddressing(CU* controlUnit, Register base, uint8_t scale, uint64_t displacement);
    uint64_t BaseScaleAddressing(CU* controlUnit, Register base, uint8_t scale);
    uint64_t BaseDisplacementAddressing(CU* controlUnit,Register base, uint64_t displacement);

    int ScaleConversion(uint8_t scale);
   
}





#endif // ADRESSING_MODE_HPP


