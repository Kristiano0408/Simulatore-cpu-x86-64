#include "../include/instruction.hpp"
#include <cstdint>
#include "controlUnit.hpp"
#include "bus.hpp"
#include "registerFile.hpp"
#include "cpu.hpp"

Instruction::Instruction()
{
    rm = {0, 0, 0, 0};
    sib = {0, 0, 0, 0};
    hasDisplacement = false;
    hasImmediate = false;
    hasModRM = false;
    hasSIB = false;
    regToReg = false;
    regToMem = false;
    memToReg = false;
    numPrefixes = 0;
    rex = false;
    rexprefix = 0;
    nbit = 0;
    opcode = 0;
    value = 0;
    displacement = 0;
    SIBdisplacement = 0;
    sourceOperand = std::make_unique<EmptyOperand>();
    destinationOperand = std::make_unique<EmptyOperand>();

}

Instruction::~Instruction() {
}

void Instruction::setOpcode(uint32_t opcode) 
{
    this->opcode = opcode;
}

uint32_t Instruction::getOpcode() 
{
    return opcode;
}

void Instruction::setPrefix(uint8_t prefix[4]) 
{
    for (int i = 0; i < 4; i++) {
        this->prefix[i] = prefix[i];
    }
}

uint8_t* Instruction::getPrefix() 
{
    return prefix;
}

void Instruction::setNumPrefixes(int numPrefixes) 
{
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

void Instruction::setRexprefix(int8_t rexprefix) {
    this->rexprefix = rexprefix;
}

int8_t Instruction::getRexprefix() {
    return rexprefix;
}


void Instruction::setNbit(int nbit) {
    this->nbit = nbit;
}

int Instruction::getNbit() {
    return nbit;
}


void Instruction::setHasImmediate(bool hasImmediate) {
    this->hasImmediate = hasImmediate;
}

bool Instruction::getHasImmediate() {
    return hasImmediate;
}


void Instruction::setHasDisplacement(bool hasDisplacement) {
    this->hasDisplacement = hasDisplacement;
}

bool Instruction::getHasDisplacement() {
    return hasDisplacement;
}


void Instruction::setHasModRM(bool hasModRM) {
    this->hasModRM = hasModRM;
}

bool Instruction::getHasModRM() {
    return hasModRM;
}


void Instruction::setHasSIB(bool hasSIB) {
    this->hasSIB = hasSIB;
}

bool Instruction::getHasSIB() {
    return hasSIB;
}


void Instruction::setRM(r_m rm) {
    this->rm = rm;
}

r_m Instruction::getRM() {
    return rm;
}


void Instruction::setSIB(SIB sib) {
    this->sib = sib;
}

SIB Instruction::getSIB() {
    return sib;
}


void Instruction::setValue(uint64_t value) {
    this->value = value;
}

uint64_t Instruction::getValue() {
    return value;
}


void Instruction::setDisplacement(uint64_t displacement) {
    this->displacement = displacement;
}

uint64_t Instruction::getDisplacement() {
    return displacement;
}


void Instruction::setSIBdisplacement(uint32_t SIBdisplacement) {
    this->SIBdisplacement = SIBdisplacement;
}

uint32_t Instruction::getSIBdisplacement() {
    return SIBdisplacement;
}


void Instruction::setRegToReg(bool regToReg) {
    this->regToReg = regToReg;
}

bool Instruction::getRegToReg() {
    return regToReg;
}


void Instruction::setRegToMem(bool regToMem) {
    this->regToMem = regToMem;
}

bool Instruction::getRegToMem() {
    return regToMem;
}


void Instruction::setMemToReg(bool memToReg) {
    this->memToReg = memToReg;
}

bool Instruction::getMemToReg() {
    return memToReg;
}

uint64_t Instruction::castingValue(uint64_t value, int nbit) 
{
    switch (nbit)
    {
        case 8:
            return castTo<uint8_t>(value);
        case 16:
            return castTo<uint16_t>(value);
        case 32:
            return castTo<uint32_t>(value);
        case 64:
            return castTo<uint64_t>(value);
        default:
            std::cerr << "Invalid number of bits" << std::endl;
            return 0; // or throw an exception 

    }
}


//getters and setters for the operands
void Instruction::setSourceOperand(std::unique_ptr<Operand> sourceOperand) {
    this->sourceOperand = std::move(sourceOperand); // move the unique_ptr to the member variable

}

Operand* Instruction::getSourceOperand() {
    return sourceOperand.get(); // return the raw pointer of the unique_ptr
}

void Instruction::setDestinationOperand(std::unique_ptr<Operand> destinationOperand) 
{
    this->destinationOperand = std::move(destinationOperand); // move the unique_ptr to the member variable
}

Operand* Instruction::getDestinationOperand() {
    return destinationOperand.get(); // return the raw pointer of the unique_ptr
}

void Instruction::setAddressingMode(AddressingMode addressingMode) {
    this->addressingMode = addressingMode;
}

//calculate the number of bits of the value/operand
int Instruction::calculating_number_of_bits() 
{
    uint32_t opcode = getOpcode();

    if (getRexprefix() & 0x08)
    {
        return 64;
    }

    for (int i = 0; i < getNumPrefixes(); i++)
    {
        if (getPrefix()[i] == 0x66)
        {
            debugLog("66 prefix");
            return 16;
        }
    }

    static const std::unordered_set<uint32_t> opcode_8bit = {0xA0, 0xA2, 0xC6, 0x88, 0x8A, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, //mov
                                                            0x04, 0x8000,0x8300, 0x00, 0x02, //add
                                                            0x8005, 0x2C, 0x28, 0x2A, //sub
                                                            };

    if(opcode_8bit.count(opcode))
    {
        return 8;
    }
   

    return 32;
}

//get the addressing mode
AddressingMode Instruction::getAddressingMode() {
    return addressingMode;
}

uint64_t Instruction::mask(int nbit) 
{
    switch (nbit)
    {
        case 8:
            return 0xFF;
        case 16:
            return 0xFFFF;
        case 32:
            return 0xFFFFFFFF;
        case 64:
            return 0xFFFFFFFFFFFFFFFF;
        default:
            std::cerr << "Invalid number of bits" << std::endl;
            return 0; // or throw an exception 

    }
}


///////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////



