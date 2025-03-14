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
    setD_address(0);
    setS_address(0);
    setS_register("");
    setD_register("");



    
    
}



//getters and setters for the operands
void MoveInstruction::setS_address(uint64_t S_address) {
    this->S_address = S_address;
}

uint64_t MoveInstruction::getS_address() {
    return S_address;
}

void MoveInstruction::setD_address(uint64_t D_address) {
    this->D_address = D_address;
}

uint64_t MoveInstruction::getD_address() {
    return D_address;
}

void MoveInstruction::setS_register(const std::string& S_register) {
    this->S_register = S_register;
}

std::string MoveInstruction::getS_register() const {
    return S_register;
}

void MoveInstruction::setD_register(const std::string& D_register) {
    this->D_register = D_register;
}

std::string MoveInstruction::getD_register() const {
    return D_register;
}

//fetch the operands
void MoveInstruction::fetchOperands(CU* controlUnit, Memory* ram) {

    //fetch the operands

    //istruction 88 and 89 and 8A and 8B
    if (isMoveInstructionR_M(getOpcode()))
    {
        if(getRM().mod == 0b11)
        {
            std::string source_register = decodeRegisterReg(getRM().reg, getRexprefix());
            std::string destination_register = decodeRegisterRM(getRM().r_m, getRexprefix(), false);

            setS_register(source_register);
            setD_register(destination_register);

        }
        else
        {   
            //operation between register and memory
            if (isMoveR_M_mem_reg)
            {
                //from memory to register
                setD_register(decodeRegisterReg(getRM().reg, getRexprefix()));

                if(!getHasSIB())
                {
                    //from memory to register
                    setS_register(decodeRegisterRM(getRM().r_m, getRexprefix(), false));
                }
                else
                {
                    //the memory address is calculated with the SIB in the execution of the instruction
                }

            }
            else 
            {
                //from register to memory
                setS_register(decodeRegisterReg(getRM().reg, getRexprefix()));

                if(!getHasSIB())
                {
                    //from register to memory
                    setD_register(decodeRegisterRM(getRM().r_m, getRexprefix(), false));
                }
                else
                {
                    //the memory address is calculated with the SIB in the execution of the instruction
                }
            }


        
            }
        }

    //istruction b0-b7 and b8-bf
    else if (isMoveInstructionIO(getOpcode()) or isMoveInstructionIO_8bit(getOpcode()))
    {
        //immediate operand to A register
        switch(getOpcode())
        {
            case 0xB0:
            case 0xB8:
                if(getRexprefix() == 0x49)
                    setD_register("R8");
                else
                    setD_register("RAX");

            case 0xB1:
            case 0xB9:
                if(getRexprefix() == 0x49)
                    setD_register("R9");
                else
                    setD_register("RCX");

            case 0xB2:
            case 0xBA:
                if(getRexprefix() == 0x49)
                    setD_register("R10");
                else
                    setD_register("RDX");
            
            case 0xB3:
            case 0xBB:
                if(getRexprefix() == 0x49)
                    setD_register("R11");
                else
                    setD_register("RBX");
            
            case 0xB4:
            case 0xBC:
                if(getRexprefix() == 0x49)
                    setD_register("R12");
                else
                    setD_register("RSP");
            
            case 0xB5:
            case 0xBD:
                if(getRexprefix() == 0x49)
                    setD_register("R13");
                else
                    setD_register("RBP");
            
            case 0xB6:
            case 0xBE:
                if(getRexprefix() == 0x49)
                    setD_register("R14");
                else
                    setD_register("RSI");

            
            case 0xB7:
            case 0xBF:
                if(getRexprefix() == 0x49)
                    setD_register("R15");
                else
                    setD_register("RDI");

            default:
                break;
        
        }

            
            








    }

    //istruction c6 and c7
    else if (isMoveInstructionIO_mem)
    {
        if(getRM().mod == 0b11)
        {
            std::string destination_register = decodeRegisterRM(getRM().r_m, getRexprefix(), false);
            setD_register(destination_register);

        }
        else
        {   
            
    }
    
    
        


   
   
    



}




// Move instruction
void MoveInstruction::execute(CU* controlUnit, Memory* ram) 
{


    
}




