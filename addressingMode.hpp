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
        int64_t directAddressing(int64_t address) const;
        int64_t immediateAddressing(int64_t value) const;
        int64_t indirectAddressing(const std::string& reg_address) const;
        int64_t registerAddressing(const std::string& reg_value) const;

        int64_t BaseAddressing(const std::string& base) const;
        int64_t BaseIndexAddressing(const std::string& base, const std::string& index) const;
        int64_t BaseIndexScaleAddressing(const std::string& base, const std::string& index, int64_t scale) const;
        int64_t BaseIndexScaleDisplacementAddressing(const std::string& base, const std::string& index, int64_t scale, int64_t displacement) const;

        int64_t BaseIndexDIsplacementAddressing(const std::string& base, const std::string& index, int64_t displacement) const;
        int64_t BaseScaleDisplacementAddressing(const std::string& base, int64_t scale, int64_t displacement) const;
        int64_t BaseScaleAddressing(const std::string& base, int64_t scale) const;
        int64_t BaseDisplacementAddressing(const std::string& base, int64_t displacement) const;

    private:
        CU* controlUnit;


};




#endif // ADRESSING_MODE_HPP


