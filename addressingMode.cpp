#include "addressingMode.hpp"
#include "controlUnit.hpp"

//modificare e ritornare tutto unint64_t

AddressingMode::AddressingMode(CU* controlUnit) {
    this->controlUnit = controlUnit;
}

AddressingMode::~AddressingMode() {
    //nothing to do here
}

int64_t AddressingMode::directAddressing(int64_t address) const {
    return address;
}

int64_t AddressingMode::immediateAddressing(int64_t value) const {
    return value;
}

int64_t AddressingMode::indirectAddressing(const std::string& reg_address) const {
    return controlUnit->getRegisters().getRegisterValue(reg_address);
}

int64_t AddressingMode::registerAddressing(const std::string& reg_value) const {
    return controlUnit->getRegisters().getRegisterValue(reg_value);
}


int64_t AddressingMode::BaseAddressing(const std::string& base) const {
    return controlUnit->getRegisters().getRegisterValue(base);
}

int64_t AddressingMode::BaseScaleAddressing(const std::string& base, int64_t scale) const {
    return controlUnit->getRegisters().getRegisterValue(base) * scale;
}

int64_t AddressingMode::BaseIndexAddressing(const std::string& base, const std::string& index) const {
    return controlUnit->getRegisters().getRegisterValue(base) + controlUnit->getRegisters().getRegisterValue(index);
}

int64_t AddressingMode::BaseIndexScaleAddressing(const std::string& base, const std::string& index, int64_t scale) const {
    return controlUnit->getRegisters().getRegisterValue(base) + controlUnit->getRegisters().getRegisterValue(index) * scale;
}

int64_t AddressingMode::BaseIndexScaleDisplacementAddressing(const std::string& base, const std::string& index, int64_t scale, int64_t displacement) const {
    return controlUnit->getRegisters().getRegisterValue(base) + controlUnit->getRegisters().getRegisterValue(index) * scale + displacement;
}

int64_t AddressingMode::BaseIndexDIsplacementAddressing(const std::string& base, const std::string& index, int64_t displacement) const {
    return controlUnit->getRegisters().getRegisterValue(base) + controlUnit->getRegisters().getRegisterValue(index) + displacement;
}

int64_t AddressingMode::BaseScaleDisplacementAddressing(const std::string& base, int64_t scale, int64_t displacement) const {
    return controlUnit->getRegisters().getRegisterValue(base) * scale + displacement;
}

int64_t AddressingMode::BaseDisplacementAddressing(const std::string& base, int64_t displacement) const {
    return controlUnit->getRegisters().getRegisterValue(base) + displacement;
}
