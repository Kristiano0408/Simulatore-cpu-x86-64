#ifndef ADDRESSCALCULATOR_HPP
#define ADDRESSCALCULATOR_HPP

#include <cstdint>
#include <string>


class CU;

namespace AddressCalculator
{
    uint64_t indirectAddressing(CU* controlUnit, const std::string& reg_address);
    uint64_t registerAddressing(CU* controlUnit, const std::string& reg_value);

    uint64_t BaseAddressing(CU* controlUnit, const std::string& base);
    uint64_t BaseIndexAddressing(CU* controlUnit, const std::string& base, const std::string& index);
    uint64_t BaseIndexScaleAddressing(CU* controlUnit, const std::string& base, const std::string& index, uint8_t scale);
    uint64_t BaseIndexScaleDisplacementAddressing(CU* controlUnit,const std::string& base, const std::string& index, uint8_t scale, uint64_t displacement);

    uint64_t BaseIndexDIsplacementAddressing(CU* controlUnit, const std::string& base, const std::string& index, uint64_t displacement);
    uint64_t BaseScaleDisplacementAddressing(CU* controlUnit, const std::string& base, uint8_t scale, uint64_t displacement);
    uint64_t BaseScaleAddressing(CU* controlUnit, const std::string& base, uint8_t scale);
    uint64_t BaseDisplacementAddressing(CU* controlUnit,const std::string& base, uint64_t displacement);

    int ScaleConversion(uint8_t scale);
   
}





#endif // ADRESSING_MODE_HPP


