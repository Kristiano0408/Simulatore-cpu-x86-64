#include "instruction.hpp"
#include <cstdint>

Instruction::~Instruction() {
    //nothing to do here
}

void Instruction::execute(CU* cu, Memory* memory) {
    // Definizione vuota o base
}

void Instruction::setOpcode(uint32_t opcode) {
    this->opcode = opcode;
}

uint32_t Instruction::getOpcode() {
    return opcode;
}

void Instruction::setPrefix(uint8_t prefix[4]) {
    for (int i = 0; i < 4; i++) {
        this->prefix[i] = prefix[i];
    }
}

uint8_t* Instruction::getPrefix() {
    return prefix;
}

void Instruction::setNumPrefixes(int numPrefixes) {
    this->numPrefixes = numPrefixes;
}

int Instruction::getNumPrefixes() {
    return numPrefixes;
}

void Instruction::setRex(bool rex) {
    this->rex = rex;
}

bool Instruction::getRex() {
    return rex;
}

void Instruction::setRexprefix(int16_t rexprefix) {
    this->rexprefix = rexprefix;
}

int16_t Instruction::getRexprefix() {
    return rexprefix;
}


//Move instruction
void MoveInstruction::execute(CU* controlUnit, Memory* ram) {
    //execute the instruction
}

MoveInstruction::MoveInstruction() {
    value = 0;
    S_address = 0;
    D_register = nullptr;
    S_register = nullptr;
    
}

void MoveInstruction::setS_address(int64_t address) {
    S_address = address;
}

void MoveInstruction::setD_register(char* registerName) {
    D_register = registerName;
}

void MoveInstruction::setS_register(char* registerName) {
    S_register = registerName;
}

void MoveInstruction::setValue(int64_t value) {
    this->value = value;
}

int64_t MoveInstruction::getValue() const {
    return value;
}