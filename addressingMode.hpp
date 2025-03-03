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
        int64_t baseDisplacementAddressing(const std::string& baseReg, int64_t displacement) const;
        int64_t indexedAddressing(const std::string& baseReg, const std::string& indexReg) const;
        int64_t scaledIndexedAddressing(const std::string& baseReg, const std::string& indexReg, int8_t scale) const;
        int64_t scaledIndexedDisplacementAddressing(const std::string& baseReg, const std::string& indexReg, int8_t scale, int64_t displacement) const;


    private:
        CU* controlUnit;
};




#endif // ADRESSING_MODE_HPP


