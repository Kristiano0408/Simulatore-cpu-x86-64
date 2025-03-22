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


    //istruction 88 and 89 and 8A and 8B (move with R/M)
    if (isMoveInstructionR_M(opcode))
    {   
        //operation between register and register
        if(getRM().mod == 0b11)
        {
            std::string source_register = decodeRegisterReg(getRM().reg, getRexprefix());
            std::string destination_register = decodeRegisterRM(getRM().r_m, getRexprefix(), false);

            setS_register(source_register);
            setD_register(destination_register);

        }
        else if (getRM().mod == 0b00)    //operation between register and memory without displacemnet
        {
            

            if (isMoveR_M_reg_mem(opcode))
            {
                std::cout <<"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
                //from register to memory
                

                //source register
                setS_register(decodeRegisterReg(getRM().reg, getRexprefix()));

                if(!getHasSIB())
                {
                    if (getRM().r_m == 0b101)
                    {
                        //calculation of the address with displacement
                        setD_address(controlUnit->getAddressingMode().BaseDisplacementAddressing("RIP", getDisplacement()));
                    }
                    else
                    {
                        //destination adress is in the register
                        setD_address(controlUnit->getAddressingMode().indirectAddressing(decodeRegisterRM(getRM().r_m, getRexprefix(), getHasSIB())));
                    }
                }
                else
                {
                    //calculation of the address with SIB
                    std::string base = decodeRegisterSIB_base(getSIB().base, getRexprefix(), getHasSIB());
                    std::string index = decodeRegisterSIB_index(getSIB().index, getRexprefix(), getHasSIB());
                    uint64_t address = 0;


                    
                    if (getSIB().base == 0b101 && getSIB().index == 0b100)
                    {
                        //calculation of the address with displacement
                        address = getSIBdisplacement();


                    }
                    else if (getSIB().base == 0b101 && getSIB().index != 0b100)
                    {

                        address = getSIBdisplacement() + controlUnit->getAddressingMode().BaseScaleAddressing(index, getSIB().scale);
                    }
                    else if (getSIB().base != 0b101 && getSIB().index == 0b100)
                    {
                        address = controlUnit->getAddressingMode().BaseAddressing(base);
                    }
                    else
                    {
                        address = controlUnit->getAddressingMode().BaseIndexScaleAddressing(base, index, getSIB().scale);
                    }

                    //destination address
                    setD_address(address);

                }
      
        
            }
            else if (isMoveR_M_mem_reg(opcode))
            {   
                std::cout <<"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
                //from memory to register

                //destination register
                setD_register(decodeRegisterReg(getRM().reg, getRexprefix()));

                if (!getHasSIB())
                {
                    if (getRM().r_m == 0b101)
                    {
                        //calculation of the address with displacement
                        setS_address(controlUnit->getAddressingMode().BaseDisplacementAddressing("RIP", getDisplacement()));
                    }
                    else
                    {
                        //destination adress is in the register
                        setS_address(controlUnit->getAddressingMode().indirectAddressing(decodeRegisterRM(getRM().r_m, getRexprefix(), getHasSIB())));
                    }
                }
                else
                {
                    //calculation of the address with SIB
                    std::string base = decodeRegisterSIB_base(getSIB().base, getRexprefix(), getHasSIB());
                    std::string index = decodeRegisterSIB_index(getSIB().index, getRexprefix(), getHasSIB());
                    uint64_t address = 0;


                    
                    if (getSIB().base == 0b101 && getSIB().index == 0b100)
                    {
                        //calculation of the address with displacement
                        address = getSIBdisplacement();


                    }
                    else if (getSIB().base == 0b101 && getSIB().index != 0b100)
                    {

                        address = getSIBdisplacement() + controlUnit->getAddressingMode().BaseScaleAddressing(index, getSIB().scale);
                    }
                    else if (getSIB().base != 0b101 && getSIB().index == 0b100)
                    {
                        address = controlUnit->getAddressingMode().BaseAddressing(base);
                    }
                    else
                    {
                        address = controlUnit->getAddressingMode().BaseIndexScaleAddressing(base, index, getSIB().scale);
                    }

                    //destination address
                    setS_address(address);

                }
            }
        }
        else if (getRM().mod == 0b01) //operation between register and memory with 8 bit displacement
        {
            if (isMoveR_M_reg_mem)
            {
                //from register to memory
                

                //source register
                setS_register(decodeRegisterReg(getRM().reg, getRexprefix()));

                if(!getHasSIB())
                {
                    //destination adress is in the register
                    setD_address(controlUnit->getAddressingMode().indirectAddressing(decodeRegisterRM(getRM().r_m, getRexprefix(), getHasSIB())) + getDisplacement());
                }
                else
                {
                    //calculation of the address with SIB
                    std::string base = decodeRegisterSIB_base(getSIB().base, getRexprefix(), getHasSIB());
                    std::string index = decodeRegisterSIB_index(getSIB().index, getRexprefix(), getHasSIB());
                    uint64_t address = 0;

                    //if the index is 0b100, there is no index
                    if (getSIB().index == 0b100)
                    {
                        //only base
                        address = controlUnit->getAddressingMode().BaseDisplacementAddressing(base, getDisplacement());
    
                    }
                    else
                    {
                        //base, index and scale and displacement
                        address = controlUnit->getAddressingMode().BaseIndexScaleDisplacementAddressing(base, index, getSIB().scale, getDisplacement());
                        
                    }
                    
                    //destination address
                    setD_address(address);
                }
            }
            else
            {
                //from memory to register

                //destination register
                setD_register(decodeRegisterReg(getRM().reg, getRexprefix()));

                if (!getHasSIB())
                {
                    //destination adress is in the register
                    setS_address(controlUnit->getAddressingMode().indirectAddressing(decodeRegisterRM(getRM().r_m, getRexprefix(), getHasSIB())) + getDisplacement());
                }
                else
                {
                    //calculation of the address with SIB
                    std::string base = decodeRegisterSIB_base(getSIB().base, getRexprefix(), getHasSIB());
                    std::string index = decodeRegisterSIB_index(getSIB().index, getRexprefix(), getHasSIB());
                    uint64_t address = 0;

                    //if the index is 0b100, there is no index
                    if (getSIB().index == 0b100)
                    {
                        //only base
                        address = controlUnit->getAddressingMode().BaseDisplacementAddressing(base, getDisplacement());
                    }
                    else
                    {
                    
                        //base, index and scale and displacement
                        address = controlUnit->getAddressingMode().BaseIndexScaleDisplacementAddressing(base, index, getSIB().scale, getDisplacement());
                      
                        
                    }
                    //source address
                    setS_address(address);
                }
            }

        
        
        
        
        
        
        
        }
        else if (getRM().mod == 0b10) //operation between register and memory with 32 bit displacement
        {
            if (isMoveR_M_reg_mem)
            {
                //from register to memory
                

                //source register
                setS_register(decodeRegisterReg(getRM().reg, getRexprefix()));

                if(!getHasSIB())
                {
                    //destination adress is in the register + displacement
                    setD_address(controlUnit->getAddressingMode().indirectAddressing(decodeRegisterRM(getRM().r_m, getRexprefix(), getHasSIB())) + getDisplacement());
                }
                else
                {
                    //calculation of the address with SIB
                    std::string base = decodeRegisterSIB_base(getSIB().base, getRexprefix(), getHasSIB());
                    std::string index = decodeRegisterSIB_index(getSIB().index, getRexprefix(), getHasSIB());
                    uint64_t address = 0;

                    //if the index is 0b100, there is no index
                    if (getSIB().index == 0b100)
                    {
                        //only base
                        address = controlUnit->getAddressingMode().BaseDisplacementAddressing(base, getDisplacement());
                        
                    }
                    else
                    {
                        address = controlUnit->getAddressingMode().BaseIndexScaleDisplacementAddressing(base, index, getSIB().scale, getDisplacement());
                       
                    }
                    
                    //destination address
                    setD_address(address);
                }
            }
            else
            {
                //from memory to register

                //destination register
                setD_register(decodeRegisterReg(getRM().reg, getRexprefix()));

                if (!getHasSIB())
                {
                    //destination adress is in the register + displacement
                    setS_address(controlUnit->getAddressingMode().indirectAddressing(decodeRegisterRM(getRM().r_m, getRexprefix(), getHasSIB())) + getDisplacement());
                }
                else
                {
                    //calculation of the address with SIB
                    std::string base = decodeRegisterSIB_base(getSIB().base, getRexprefix(), getHasSIB());
                    std::string index = decodeRegisterSIB_index(getSIB().index, getRexprefix(), getHasSIB());
                    uint64_t address = 0;

                    //if the index is 0b100, there is no index
                    if (getSIB().index == 0b100)
                    {
                        //only base
                        address = controlUnit->getAddressingMode().BaseDisplacementAddressing(base, getDisplacement());
                    }
                    else
                    {
                        //base, index and scale and displacement
                        address = controlUnit->getAddressingMode().BaseIndexScaleDisplacementAddressing(base, index, getSIB().scale, getDisplacement());
                      
                    }
                    
                    //source address
                    setS_address(address);
                }

            }


        }
   

    }

    //istruction B0- BF (move io without R/M)
    else if(isMoveInstructionIO(opcode) or isMoveInstructionIO_8bit(opcode))
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

    //istruction C6-C7 (move io with R/M)
    if (isMoveInstructionIO_mem(opcode))
    {
        //the destination is a register
        if(getRM().mod == 0b11)
        {
            std::string destination_register = decodeRegisterRM(getRM().r_m, getRexprefix(), false);
            setD_register(destination_register);

        }
        else if (getRM().mod == 0b00)    //operation between register and memory without displacemnet
        {

            if(!getHasSIB())
                {
                    if (getRM().r_m == 0b101)
                    {
                        //calculation of the address with displacement
                        setD_address(controlUnit->getAddressingMode().BaseDisplacementAddressing("RIP", getDisplacement()));
                    }
                    else
                    {
                        //destination adress is in the register
                        setD_address(controlUnit->getAddressingMode().indirectAddressing(decodeRegisterRM(getRM().r_m, getRexprefix(), getHasSIB())));
                    }
                }
            else
                {
                    //calculation of the address with SIB
                    std::string base = decodeRegisterSIB_base(getSIB().base, getRexprefix(), getHasSIB());
                    std::string index = decodeRegisterSIB_index(getSIB().index, getRexprefix(), getHasSIB());
                    uint64_t address = 0;


                    
                    if (getSIB().base == 0b101 && getSIB().index == 0b100)
                    {
                        //calculation of the address with displacement
                        address = getSIBdisplacement();


                    }
                    else if (getSIB().base == 0b101 && getSIB().index != 0b100)
                    {

                        address = getSIBdisplacement() + controlUnit->getAddressingMode().BaseScaleAddressing(index, getSIB().scale);
                    }
                    else if (getSIB().base != 0b101 && getSIB().index == 0b100)
                    {
                        address = controlUnit->getAddressingMode().BaseAddressing(base);
                    }
                    else
                    {
                        address = controlUnit->getAddressingMode().BaseIndexScaleAddressing(base, index, getSIB().scale);
                    }

                    //destination address
                    setD_address(address);

                }
      
            

    }
        else if (getRM().mod == 0b01)   //operation between register and memory with 8 bit dispalcemnet
        {
            if(!getHasSIB())
            {
                //destination adress is in the register
                setD_address(controlUnit->getAddressingMode().indirectAddressing(decodeRegisterRM(getRM().r_m, getRexprefix(), getHasSIB())) + getDisplacement());
            }
            else
            {
                //calculation of the address with SIB
                std::string base = decodeRegisterSIB_base(getSIB().base, getRexprefix(), getHasSIB());
                std::string index = decodeRegisterSIB_index(getSIB().index, getRexprefix(), getHasSIB());
                uint64_t address = 0;

                //if the index is 0b100, there is no index
                if (getSIB().index == 0b100)
                {
                    //only base
                    address = controlUnit->getAddressingMode().BaseDisplacementAddressing(base, getDisplacement());

                }
                else
                {
                    //base, index and scale and displacement
                    address = controlUnit->getAddressingMode().BaseIndexScaleDisplacementAddressing(base, index, getSIB().scale, getDisplacement());
                    
                }
                
                //destination address
                setD_address(address);
            }

        }
        else if (getRM().mod == 0b10)   //operation between register and memory wit 32 bit dispalcemnet
        {
            if(!getHasSIB())
            {
                //destination adress is in the register
                setD_address(controlUnit->getAddressingMode().indirectAddressing(decodeRegisterRM(getRM().r_m, getRexprefix(), getHasSIB())) + getDisplacement());
            }
            else
            {
                //calculation of the address with SIB
                std::string base = decodeRegisterSIB_base(getSIB().base, getRexprefix(), getHasSIB());
                std::string index = decodeRegisterSIB_index(getSIB().index, getRexprefix(), getHasSIB());
                uint64_t address = 0;

                //if the index is 0b100, there is no index
                if (getSIB().index == 0b100)
                {
                    //only base
                    address = controlUnit->getAddressingMode().BaseDisplacementAddressing(base, getDisplacement());
                    
                }
                else
                {
                    //base, index and scale and displacement
                    address = controlUnit->getAddressingMode().BaseIndexScaleDisplacementAddressing(base, index, getSIB().scale, getDisplacement());
                    
                }
                
                //destination address
                setD_address(address);
            }

        }
    }

    //istruction A0-A3 (move between RAX and memory)
    if (isMoveInstructionOffset(opcode))
    {
        if (isMoveInstructionOffsetMem_RAx)
        {
            S_address = getDisplacement();
            D_register = "RAX";
        }
        else
        {
            S_register = "RAX";
            D_address = getDisplacement();
        }
    }


}




// Move instruction
void MoveInstruction::execute(CU* controlUnit, Memory* ram) 
{


    
}




