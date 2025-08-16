#ifndef ADDRESSCALCULATOR_HPP
#define ADDRESSCALCULATOR_HPP

#include <cstdint>
#include <string>


class Bus;

enum class Register;

namespace AddressCalculator
{
    uint64_t indirectAddressing(Bus& bus, Register reg_address);
    uint64_t registerAddressing(Bus& bus, Register reg_value);

    uint64_t BaseAddressing(Bus& bus, Register base);
    uint64_t BaseIndexAddressing(Bus& bus, Register base, Register index);
    uint64_t BaseIndexScaleAddressing(Bus& bus, Register base, Register index, uint8_t scale);
    uint64_t BaseIndexScaleDisplacementAddressing(Bus& bus, Register base, Register index, uint8_t scale, uint64_t displacement);

    uint64_t BaseIndexDIsplacementAddressing(Bus& bus, Register base, Register index, uint64_t displacement);
    uint64_t BaseScaleDisplacementAddressing(Bus& bus, Register base, uint8_t scale, uint64_t displacement);
    uint64_t BaseScaleAddressing(Bus& bus, Register base, uint8_t scale);
    uint64_t BaseDisplacementAddressing(Bus& bus, Register base, uint64_t displacement);

    int ScaleConversion(uint8_t scale);
   
}





#endif // ADRESSING_MODE_HPP


