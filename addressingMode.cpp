#include "addressingMode.hpp"



//RICORDARSI controllo overflow

AddressingMode::AddressingMode(CU* controlUnit) {
    this->controlUnit = controlUnit;
}

AddressingMode::~AddressingMode() {
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

/* in theory it doesn't make sense to have this function cause the value will be taken during execution, in decoding is stored the name of the register
    Maybe it could be useful for other instructions(not move, cause the execution will take the value from the register)
int64_t AddressingMode::registerAddressing(const std::string& reg_value) const {
    return controlUnit->getRegisters().getRegisterValue(reg_value);
}*/

int64_t AddressingMode::baseDisplacementAddressing(const std::string& baseReg, int64_t displacement) const {
    return controlUnit->getRegisters().getRegisterValue(baseReg) + displacement;
}

int64_t AddressingMode::indexedAddressing(const std::string& baseReg, const std::string& indexReg) const {
    return controlUnit->getRegisters().getRegisterValue(baseReg) + controlUnit->getRegisters().getRegisterValue(indexReg);
}

int64_t AddressingMode::scaledIndexedAddressing(const std::string& baseReg, const std::string& indexReg, int8_t scale) const {
    return controlUnit->getRegisters().getRegisterValue(baseReg) + controlUnit->getRegisters().getRegisterValue(indexReg) * scale;
}

int64_t AddressingMode::scaledIndexedDisplacementAddressing(const std::string& baseReg, const std::string& indexReg, int8_t scale, int64_t displacement) const {
    return controlUnit->getRegisters().getRegisterValue(baseReg) + controlUnit->getRegisters().getRegisterValue(indexReg) * scale + displacement;
}