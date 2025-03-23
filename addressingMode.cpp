#include "addressingMode.hpp"
#include "controlUnit.hpp"

//modificare e ritornare tutto unint64_t

AddressingMode::AddressingMode(CU* controlUnit) {
    this->controlUnit = controlUnit;
}

AddressingMode::~AddressingMode() {
    //nothing to do here
}

uint64_t AddressingMode::directAddressing(uint64_t address) const {
    return address;
}

uint64_t AddressingMode::immediateAddressing(uint64_t value) const {
    return value;
}

uint64_t AddressingMode::indirectAddressing(const std::string& reg_address) const {
    return controlUnit->getRegisters().getRegisterValue(reg_address);
}

uint64_t AddressingMode::registerAddressing(const std::string& reg_value) const {
    return controlUnit->getRegisters().getRegisterValue(reg_value);
}


uint64_t AddressingMode::BaseAddressing(const std::string& base) const {
    return controlUnit->getRegisters().getRegisterValue(base);
}

uint64_t AddressingMode::BaseScaleAddressing(const std::string& base, uint8_t scale) const {
    return controlUnit->getRegisters().getRegisterValue(base) * ScaleConversion(scale);
}

uint64_t AddressingMode::BaseIndexAddressing(const std::string& base, const std::string& index) const {
    return controlUnit->getRegisters().getRegisterValue(base) + controlUnit->getRegisters().getRegisterValue(index);
}

uint64_t AddressingMode::BaseIndexScaleAddressing(const std::string& base, const std::string& index, uint8_t scale) const {
    return controlUnit->getRegisters().getRegisterValue(base) + controlUnit->getRegisters().getRegisterValue(index) * ScaleConversion(scale);
}

uint64_t AddressingMode::BaseIndexScaleDisplacementAddressing(const std::string& base, const std::string& index, uint8_t scale, uint64_t displacement) const {
    return controlUnit->getRegisters().getRegisterValue(base) + controlUnit->getRegisters().getRegisterValue(index) * ScaleConversion(scale) + displacement;
}

uint64_t AddressingMode::BaseIndexDIsplacementAddressing(const std::string& base, const std::string& index, uint64_t displacement) const {
    return controlUnit->getRegisters().getRegisterValue(base) + controlUnit->getRegisters().getRegisterValue(index) + displacement;
}

uint64_t AddressingMode::BaseScaleDisplacementAddressing(const std::string& base, uint8_t scale, uint64_t displacement) const {
    return controlUnit->getRegisters().getRegisterValue(base) * ScaleConversion(scale) + displacement;
}

uint64_t AddressingMode::BaseDisplacementAddressing(const std::string& base, uint64_t displacement) const {
    return controlUnit->getRegisters().getRegisterValue(base) + displacement;
}

int AddressingMode::ScaleConversion(uint8_t scale) const {
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
