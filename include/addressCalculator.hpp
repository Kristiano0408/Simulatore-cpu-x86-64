#ifndef ADDRESSCALCULATOR_HPP
#define ADDRESSCALCULATOR_HPP

#include <cstdint>
#include <string>


class RegisterFile;
enum class Register : uint8_t;

namespace AddressCalculator
{
    uint64_t indirectAddressing(RegisterFile& registers, Register reg_address);
    uint64_t registerAddressing(RegisterFile& registers, Register reg_value);

    uint64_t BaseAddressing(RegisterFile& registers, Register base);
    uint64_t BaseIndexAddressing(RegisterFile& registers, Register base, Register index);
    uint64_t BaseIndexScaleAddressing(RegisterFile& registers, Register base, Register index, uint8_t scale);
    uint64_t BaseIndexScaleDisplacementAddressing(RegisterFile& registers, Register base, Register index, uint8_t scale, uint64_t displacement);

    uint64_t BaseIndexDIsplacementAddressing(RegisterFile& registers, Register base, Register index, uint64_t displacement);
    uint64_t BaseScaleDisplacementAddressing(RegisterFile& registers, Register base, uint8_t scale, uint64_t displacement);
    uint64_t BaseScaleAddressing(RegisterFile& registers, Register base, uint8_t scale);
    uint64_t BaseDisplacementAddressing(RegisterFile& registers, Register base, uint64_t displacement);

    int ScaleConversion(uint8_t scale);
   
}





#endif // ADRESSING_MODE_HPP


