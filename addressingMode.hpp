#ifndef ADDRESSING_MODE_HPP
#define ADDRESSING_MODE_HPP

#include <cstdint>
#include <string>


class CU;



class AddressingMode
{
    public:
        AddressingMode(CU* controlUnit);
        ~AddressingMode();
        uint64_t directAddressing(uint64_t address) const;
        uint64_t immediateAddressing(uint64_t value) const;
        uint64_t indirectAddressing(const std::string& reg_address) const;
        uint64_t registerAddressing(const std::string& reg_value) const;

        uint64_t BaseAddressing(const std::string& base) const;
        uint64_t BaseIndexAddressing(const std::string& base, const std::string& index) const;
        uint64_t BaseIndexScaleAddressing(const std::string& base, const std::string& index, uint8_t scale) const;
        uint64_t BaseIndexScaleDisplacementAddressing(const std::string& base, const std::string& index, uint8_t scale, uint64_t displacement) const;

        uint64_t BaseIndexDIsplacementAddressing(const std::string& base, const std::string& index, uint64_t displacement) const;
        uint64_t BaseScaleDisplacementAddressing(const std::string& base, uint8_t scale, uint64_t displacement) const;
        uint64_t BaseScaleAddressing(const std::string& base, uint8_t scale) const;
        uint64_t BaseDisplacementAddressing(const std::string& base, uint64_t displacement) const;

        int ScaleConversion(uint8_t scale) const;



    private:
        CU* controlUnit;


};




#endif // ADRESSING_MODE_HPP


