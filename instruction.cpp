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
    setRM({0, 0, 0});
    setSIB({0, 0, 0});
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

    //fetch the operands


    //get the type of the move instruction
    auto MOVType = move_instruction.find(opcode)->second;

    
    //using switch case to get the operands
    switch (MOVType)
    {
        case MOVType::MOV_MR:                     //move register to R/M
            std::cout << "MOV_MR" << std::endl;
            operandFetch::fetchMR(this, controlUnit, ram);
            break;
        
        case MOVType::MOV_RM:                     //move R/M to register
            std::cout << "MOV_RM" << std::endl;
            std::cout << "opcode: " << opcode << std::endl;
            operandFetch::fetchRM(this, controlUnit, ram);
            break;
        
        
        case MOVType::MOV_MI:                     //move immediate to memory/register
            std::cout << "MOV_MI" << std::endl;
            operandFetch::fetchMI(this, controlUnit, ram);
            break;
        
        case MOVType::MOV_OI:                     //move immediate to reg
            std::cout << "MOV_OI" << std::endl;
            operandFetch::fetchOI(this, controlUnit, ram, opcode);
            break;
        
        case MOVType::MOV_FD:                     //move from offset to Rax
            std::cout << "MOV_FD" << std::endl;
            operandFetch::fetchFD(this, controlUnit, ram);
            break;
        
        case MOVType::MOV_TD:                    //move from Rax to offset
            std::cout << "MOV_TD" << std::endl;
            operandFetch::fetchTD(this, controlUnit, ram);
            break;
        
        default:
            break;
    }


}

// Move instruction
void MoveInstruction::execute(CU* controlUnit, Memory* ram) 
{

    //setting the number of bytes to operate with
    int bit = calcualting_number_of_bits(controlUnit);

    setNbit(bit);

    //getting the type of the move instruction
    auto MOVType = move_instruction.find(getOpcode())->second;

    //using switch case to execute the instruction
    switch (MOVType)
    {
    case MOVType::MOV_MR:                     //move register to R/M
        std::cout << "MOV_MR" << std::endl;
        executeR_M(controlUnit, ram, MOVType::MOV_MR);
        break;
    
    case MOVType::MOV_RM:                     //move R/M to register
        std::cout << "MOV_RM" << std::endl;
        executeR_M(controlUnit, ram, MOVType::MOV_RM);
        break;

    case MOVType::MOV_MI:                     //move immediate to memory/register
        std::cout << "MOV_MI" << std::endl;
        executeMI(controlUnit, ram);
        break;
    
    case MOVType::MOV_OI:                     //move immediate to reg
        std::cout << "MOV_OI" << std::endl;
        executeOI(controlUnit, ram);
        break;

    case MOVType::MOV_FD:                     //move from offset to Rax
        std::cout << "MOV_FD" << std::endl;
        executeFD_TD(controlUnit, ram, MOVType::MOV_FD);
        break;

    case MOVType::MOV_TD:                    //move from Rax to offset
        std::cout << "MOV_TD" << std::endl;
        executeFD_TD(controlUnit, ram, MOVType::MOV_TD);
        break;

    default:
        break;
    }




    
}

int MoveInstruction::calcualting_number_of_bits(CU* controlUnit) 
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
            return 16;
        }
    }

    static const std::unordered_set<uint32_t> opcode_8bit = {0xA0, 0xA2, 0xC6, 0x88, 0x8A, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7};

    if(opcode_8bit.count(opcode))
    {
        return 8;
    }
   

    return 32;
}


void MoveInstruction::executeR_M(CU* controlUnit, Memory* ram, MOVType type)
{
    //gettting the reg, address and value to operate with
    std::string S_reg = getS_register();
    std::string D_reg = getD_register();
    uint64_t S_addr= getS_address();
    uint64_t D_addr = getD_address();

    uint64_t S_value = 0;
    uint64_t S_value_casted = 0;

    switch(type)
    {
        case MOVType::MOV_MR: //move register to R/M
            std::cout << "MOV_MR" << std::endl;

            //getting the value of the source register
            S_value = controlUnit->getRegisters().getRegisterValue(S_reg);

            //casting the value to the number of bits of the operand (8, 16, 32, 64) and zero extending it
            S_value_casted = castingValue(S_value, getNbit());

            if(D_addr != 0)
            {
                //if the destination address is not 0, we are moving to memory
                switch (getNbit())
                {
                    case 8:
                        ram->writeByte(D_addr,(uint8_t)S_value_casted);
                        break;
                    case 16:
                        ram->writeWord(D_addr, (uint16_t)S_value_casted);
                        break;
                    case 32:
                        ram->writeDWord(D_addr, (uint32_t)S_value_casted);
                        break;
                    case 64:
                        ram->writeQWord(D_addr, (uint64_t)S_value_casted);
                        break;
                    default:
                        break;  
                
                    
                }
            }
            else
            {
                //if the destination address is 0, we are moving to register
                controlUnit->getRegisters().setRegisterValue(D_reg, S_value_casted);
                
            }
           
            break;
        
        case MOVType::MOV_RM: //move R/M to register
            std::cout << "MOV_RM" << std::endl;

            S_value = 0;

            //getting the value of the source register
            if (S_addr != 0)
            {
                //if the source address is not 0, we are moving from memory
                switch (getNbit())
                {
                    case 8:
                        S_value = ram->readByte(S_addr);
                        break;
                    case 16:
                        S_value = ram->readWord(S_addr);
                        break;
                    case 32:
                        S_value = ram->readDWord(S_addr);
                        break;
                    case 64:
                        S_value = ram->readQWord(S_addr);
                        break;
                    default:
                        break;  
                
                    
                }
            }
            else
            {
                //if the source address is 0, we are moving from register
                S_value = controlUnit->getRegisters().getRegisterValue(S_reg);
                
            }

            //casting the value to the number of bits of the operand (8, 16, 32, 64) and zero extending it
            S_value_casted = castingValue(S_value, getNbit());

            //setting the value to the destination register
            controlUnit->getRegisters().setRegisterValue(D_reg, S_value_casted);

            break;
        
        default:
            break;
    }


}

void MoveInstruction::executeMI(CU* controlUnit, Memory* ram)
{

    //gettting the reg, address and value to operate with
    std::string S_reg = getS_register();
    std::string D_reg = getD_register();
    uint64_t value = getValue();


    //casting not necessary, we are moving immediate value to memory/register and the value is already in the correct format

    if (getD_address() != 0)
    {
        //if the destination address is not 0, we are moving to memory
        switch (getNbit())
        {
            case 8:
                ram->writeByte(getD_address(),(uint8_t)value);
                break;
            case 16:
                ram->writeWord(getD_address(), (uint16_t)value);
                break;
            case 32:
                ram->writeDWord(getD_address(), (uint32_t)value);
                break;
            case 64:
                ram->writeQWord(getD_address(), (uint64_t)value);
                break;
            default:
                break;  
            
                
        }
    }
    else
    {
        //if the destination address is 0, we are moving to register
        controlUnit->getRegisters().setRegisterValue(D_reg, value);
        
    }


   
   

}

void MoveInstruction::executeOI(CU* controlUnit, Memory* ram) 
{
    //gettting the reg, address and value to operate with
    std::string D_reg = getD_register();
    uint64_t value = getValue();


    //casting not necessary, we are moving immediate value to memory/register and the value is already in the correct format
    controlUnit->getRegisters().setRegisterValue(D_reg, value);



}

void MoveInstruction::executeFD_TD(CU* controlUnit, Memory* ram, MOVType type) 
{
    uint64_t value = 0;
    uint64_t reg_address;
    switch (type)
    {
        case MOVType::MOV_FD: //move from offset to Rax
            
        //getting the value from the offset
            reg_address = getS_address();
            switch(getNbit())
            {
                case 8:
                    value = ram->readByte(reg_address);
                    break;
                case 16:
                    value = ram->readWord(reg_address);
                    break;
                case 32:
                    value = ram->readDWord(reg_address);
                    break;
                case 64:
                    value = ram->readQWord(reg_address);
                    break;
                default:
                    break;  
                
                    
            }

            //setting the value to the destination register
            controlUnit->getRegisters().setRegisterValue("RAX", value);
            
            break;
            
        case MOVType::MOV_TD: //move from Rax to offset
            
            //getting the value from the register
            value = controlUnit->getRegisters().getRegisterValue("RAX");

            //casting the value to the number of bits of the operand (8, 16, 32, 64) and zero extending it
            value = castingValue(value, getNbit());

            //getting the address to move the value to
            reg_address = getD_address();

            //writing the value to the address
            switch(getNbit())
            {
                case 8:
                    ram->writeByte(reg_address, (uint8_t)value);
                    break;
                case 16:
                    ram->writeWord(reg_address, (uint16_t)value);
                    break;
                case 32:
                    ram->writeDWord(reg_address, (uint32_t)value);
                    break;
                case 64:
                    ram->writeQWord(reg_address, (uint64_t)value);
                    break;
                default:
                    break;        
            }
            
            break;

        default:
            break;
    }

}


uint64_t MoveInstruction::castingValue(uint64_t value, int nbit) 
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




          


    

    


