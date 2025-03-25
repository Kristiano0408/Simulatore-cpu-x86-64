#include "instruction.hpp"
#include <cstdint>
#include "controlUnit.hpp"


Instruction::~Instruction() {
    //nothing to do here
}

void Instruction::execute(CU* cu, Memory* memory) {
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

void Instruction::setRexprefix(int8_t rexprefix) {
    this->rexprefix = rexprefix;
}

int8_t Instruction::getRexprefix() {
    return rexprefix;
}

void Instruction::decodeNbit() {
    
    if (rexprefix && 0x08)
    {
        nbit = 64;
    } else {
        nbit = 32;
    }

    for (int i = 0; i < numPrefixes; i++)
    {
        if (prefix[i] == 0x66)
        {
            nbit = 16;
            break;
        }
    }


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




//getters and setters for the operands
void Instruction::setS_address(uint64_t S_address) {
    this->S_address = S_address;
}

uint64_t Instruction::getS_address() {
    return S_address;
}

void Instruction::setD_address(uint64_t D_address) {
    this->D_address = D_address;
}

uint64_t Instruction::getD_address() {
    return D_address;
}

void Instruction::setS_register(const std::string& S_register) {
    this->S_register = S_register;
}

std::string Instruction::getS_register() const {
    return S_register;
}

void Instruction::setD_register(const std::string& D_register) {
    this->D_register = D_register;
}

std::string Instruction::getD_register() const {
    return D_register;
}


//Move instruction

//constructor
MoveInstruction::MoveInstruction() {
    //resets the values
    setHasDisplacement(false);
    setHasImmediate(false);
    setHasModRM(false);
    setHasSIB(false);
    setMemToReg(false);
    setRegToMem(false);
    setRegToReg(false);
    setNumPrefixes(0);
    setRex(false);
    setRexprefix(0);
    setNbit(0);
    setOpcode(0);
    setValue(0);
    setDisplacement(0);
    setSIBdisplacement(0);
    setD_address(0);
    setS_address(0);
    setS_register("NULL");
    setD_register("NULL");



    
    
}


//fetch the operands
void MoveInstruction::fetchOperands(CU* controlUnit, Memory* ram) {

    //getting the opcode
    uint32_t opcode = getOpcode();

    std::cout << "opcode: " << opcode << std::endl;
    //fetch the operands


    //get the type of the move instruction
    auto MOVType = move_instruction.find(opcode)->second;

    
    //using switch case to get the operands
    switch (MOVType)
    {
        case MOVType::MOV_MR:                     //move register to R/M
            std::cout << "MOV_MR" << std::endl;
            fetchOperandsR_M(controlUnit, ram, MOVType::MOV_MR);
            break;
        
        case MOVType::MOV_RM:                     //move R/M to register
            std::cout << "MOV_RM" << std::endl;
            fetchOperandsR_M(controlUnit, ram, MOVType::MOV_RM);
            break;
        
        
        case MOVType::MOV_MI:                     //move immediate to memory/register
            std::cout << "MOV_MI" << std::endl;
            fetchOperandsMI(controlUnit, ram);
            break;
        
        case MOVType::MOV_OI:                     //move immediate to reg
            std::cout << "MOV_OI" << std::endl;
            fetchOperandsOI(controlUnit, ram, opcode);
            break;
        
        case MOVType::MOV_FD:                     //move from offset to Rax
            std::cout << "MOV_FD" << std::endl;
            fetchOperandsFD_TD(controlUnit, ram, MOVType::MOV_FD);
            break;
        
        case MOVType::MOV_TD:                    //move from Rax to offset
            fetchOperandsFD_TD(controlUnit, ram, MOVType::MOV_TD);
            break;
        
        default:
            break;
    }


}




// Move instruction
void MoveInstruction::execute(CU* controlUnit, Memory* ram) 
{


    
}


void MoveInstruction::fetchOperandsR_M(CU* controlUnit, Memory* ram, MOVType type)
{   
    
    //Case 1: operation between register and register
    if(getRM().mod == 0b11)   
    {
        std::string source_register = decodeRegisterReg(getRM().reg, getRexprefix());
        std::string destination_register = decodeRegisterRM(getRM().r_m, getRexprefix(), false);

        setS_register(source_register);
        setD_register(destination_register);

        return;

    }

    //Case 2: operation between register and memory
   
    uint64_t address {calculatingAddressR_M(controlUnit, ram)};

    std::cout << "Address: " << address << std::endl;

    //switch case to set the source and destination operands based on the direction of the operation
    switch(type)
    {
        case MOVType::MOV_MR:  //move register to R/M
            setS_register(decodeRegisterReg(getRM().reg, getRexprefix()));
            setD_address(address);
            break;
            
        case MOVType::MOV_RM:  //move R/M to register
            setD_register(decodeRegisterReg(getRM().reg, getRexprefix()));
            setS_address(address);
            break;
            
        default:
            break;
    }
    




}

uint64_t MoveInstruction::calculatingAddressR_M(CU* controlUnit, Memory* ram)
{
     //if there is no SIB
    if(!getHasSIB())
    {
        if (getRM().r_m == 0b101 && getRM().mod == 0b00)
        {
            //calculation of the address with  RIP displacement
            return controlUnit->getAddressingMode().BaseDisplacementAddressing("RIP", getDisplacement());
        }
        else
        {
            //destination adress is in the register
            return (controlUnit->getAddressingMode().indirectAddressing(decodeRegisterRM(getRM().r_m, getRexprefix(), getHasSIB())) + getDisplacement());

        }
    }
    else
    {
        //calculation of the address with SIB
        std::string base = decodeRegisterSIB_base(getSIB().base, getRexprefix(), getHasSIB());
        std::string index = decodeRegisterSIB_index(getSIB().index, getRexprefix(), getHasSIB());
        uint64_t address = 0;

        //if the base is 0b101 and the index is 0b100, there is no index and base is 32 bit displacement
        if (getSIB().base == 0b101 && getSIB().index == 0b100 && getRM().mod == 0b00)
        {
            address = getSIBdisplacement();
        }
        //if the base is 0b101 and the index is not 0b100, base(displacement), index and scale addressing
        else if (getSIB().base == 0b101 && getSIB().index != 0b100 && getRM().mod == 0b00)
        {
            address = getSIBdisplacement() + controlUnit->getAddressingMode().BaseScaleAddressing(index, getSIB().scale);
        }
        //if the base is not 0b101 and the index is 0b100, normal base addressing
        else if (getSIB().base != 0b101 && getSIB().index == 0b100)
        {
            address = controlUnit->getAddressingMode().BaseAddressing(base);
        }
        //if the base is not 0b101 and the index is not 0b100, base, index and scale addressing
        else
        {
            address = controlUnit->getAddressingMode().BaseIndexScaleAddressing(base, index, getSIB().scale);
        }

        //if the mod is 0b01 or 0b10, there is a displacement to add
        if (getRM().mod == 0b01 || getRM().mod == 0b10)
        {
            address += getDisplacement();
        }

        return address;


    }

}

void MoveInstruction::fetchOperandsOI(CU* controlUnit, Memory* ram, uint32_t opcode)
{
    const std::string reg_names[16] = {"RAX", "RCX", "RDX", "RBX", "RSP", "RBP", "RSI", "RDI", // fisrt 8 registri
        "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15"};// last 8 registri

    int reg_index = opcode & 0x07;


    if(getRexprefix() & 0x01)  // if Rex.b = 1
    {
    reg_index += 8;
    }


    //setting the destination register
    setD_register(reg_names[reg_index]);

}

void MoveInstruction::fetchOperandsMI(CU* controlUnit, Memory* ram)
{
    if(getRM().mod == 0b11)
    {
            std::string destination_register = decodeRegisterRM(getRM().r_m, getRexprefix(), false);
            setD_register(destination_register);

        return;

    }

    uint64_t address {calculatingAddressR_M(controlUnit, ram)};

    setD_address(address);

}
   
void MoveInstruction::fetchOperandsFD_TD(CU* controlUnit, Memory* ram, MOVType type)
{
    switch (type)
    {
        case MOVType::MOV_FD:  //move from offset to Rax
            setD_register("RAX");
            setS_address(getDisplacement());
            break;
            
        case MOVType::MOV_TD:  //move from Rax to offset
            setS_register("RAX");
            setD_address(getDisplacement());
            break;
            
        default:
            break;
    }
}
      
        
          


    

    


