#include "operands.hpp"
#include "instruction.hpp"
#include "controlUnit.hpp"
#include "memory.hpp"
#include <string>


namespace operandFetch {

    //fetching RM operands 
    void fetchRM(Instruction* i, CU* controlUnit, Memory* ram)
    {
        std::cout << "fetchRM" << std::endl;
        //getting the r/m byte
        r_m rm = i->getRM();

        //getting the rex prefix
        uint8_t rex = i->getRexprefix();

        //Case 1: operation between register and register
        if(rm.mod == 0b11)   
        {
            std::string source_register = decodeRegisterReg(rm.reg, rex);
            std::string destination_register = decodeRegisterRM(rm.r_m, rex, false);

            //costructor of the operand
            RegOperand* source = new RegOperand(controlUnit->getRegisters().getRegisterValue(source_register));
            RegOperand* destination = new RegOperand(destination_register);
            //i->setS_register(source_register);
            //i->setD_register(destination_register);

            return;

        }

        //Case 2: operation between register and memory
    
        uint64_t address {calculatingAddressR_M(i,controlUnit, ram)};

        
         
        i->setD_register(decodeRegisterReg(rm.reg, rex));
        i->setS_address(address);
                

    }

    void fetchMR(Instruction* i, CU* controlUnit, Memory* ram)
    {
        //getting the r/m byte
        r_m rm = i->getRM();

        //getting the rex prefix
        uint8_t rex = i->getRexprefix();

        //Case 1: operation between register and register
        if(rm.mod == 0b11)   
        {
            std::string source_register = decodeRegisterReg(rm.reg, rex);
            std::string destination_register = decodeRegisterRM(rm.r_m, rex, false);

            i->setS_register(source_register);
            i->setD_register(destination_register);

            return;

        }

        //Case 2: operation between register and memory
    
        uint64_t address {calculatingAddressR_M(i,controlUnit, ram)};

        
         
        i->setS_register(decodeRegisterReg(rm.reg, rex));
        i->setD_address(address);
                

    }

    void fetchFD(Instruction* i, CU* controlUnit, Memory* ram)
    {
        i->setD_register("RAX");
        i->setS_address(i->getDisplacement());
    
    }

    void fetchTD(Instruction* i, CU* controlUnit, Memory* ram)
    {
        i->setS_register("RAX");
        i->setD_address(i->getDisplacement());

    }

    void fetchOI(Instruction* i, CU* controlUnit, Memory* ram, uint32_t opcode)
    {
        const std::string reg_names[16] = {"RAX", "RCX", "RDX", "RBX", "RSP", "RBP", "RSI", "RDI", // fisrt 8 registri
            "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15"};// last 8 registri
    
        int reg_index = opcode & 0x07;
    
    
        if(i->getRexprefix() & 0x01)  // if Rex.b = 1
        {
        reg_index += 8;
        }
    
    
        //setting the destination register
        i->setD_register(reg_names[reg_index]);

    }

    void fetchMI(Instruction* i, CU* controlUnit, Memory* ram)
    {
        //getting the r/m byte
        r_m rm = i->getRM();

        if(rm.mod == 0b11)
        {
                std::string destination_register = decodeRegisterRM(rm.r_m, i->getRexprefix(), false);
                i->setD_register(destination_register);

            return;

        }

        uint64_t address {calculatingAddressR_M(i,controlUnit, ram)};

        i->setD_address(address);

    }

    uint64_t calculatingAddressR_M(Instruction* i, CU* controlUnit, Memory* ram)
    {
        //getting the r/m byte
        r_m rm = i->getRM();

        //getting the rex prefix
        uint8_t rex = i->getRexprefix();

        //getting the displacement
        uint64_t displacement = i->getDisplacement();

        //getting the SIB byte
        SIB sib = i->getSIB();

        //if there is no SIB
        if(!i->getHasSIB())
        {
            if (rm.r_m == 0b101 && rm.mod == 0b00)
            {
                //calculation of the address with  RIP displacement
                return controlUnit->getAddressingMode().BaseDisplacementAddressing("RIP", displacement);
            }
            else
            {
                //destination adress is in the register
                return (controlUnit->getAddressingMode().indirectAddressing(decodeRegisterRM(rm.r_m, rex, i->getHasSIB())) + displacement);

            }
        }
        else
        {
            //calculation of the address with SIB
            std::string base = decodeRegisterSIB_base(sib.base, rex, i->getHasSIB());
            std::string index = decodeRegisterSIB_index(sib.index, rex, i->getHasSIB());
            uint64_t address = 0;

            //if the base is 0b101 and the index is 0b100, there is no index and base is 32 bit displacement
            if (sib.base == 0b101 && sib.index == 0b100 && rm.mod == 0b00)
            {
                address = i->getSIBdisplacement();
            }
            //if the base is 0b101 and the index is not 0b100, base(displacement), index and scale addressing
            else if (sib.base == 0b101 && sib.index != 0b100 && rm.mod == 0b00)
            {
                address = i->getSIBdisplacement() + controlUnit->getAddressingMode().BaseScaleAddressing(index, sib.scale);
            }
            //if the base is not 0b101 and the index is 0b100, normal base addressing
            else if (sib.base != 0b101 && sib.index == 0b100)
            {
                address = controlUnit->getAddressingMode().BaseAddressing(base);
            }
            //if the base is not 0b101 and the index is not 0b100, base, index and scale addressing
            else
            {
                address = controlUnit->getAddressingMode().BaseIndexScaleAddressing(base, index, sib.scale);
            }

            //if the mod is 0b01 or 0b10, there is a displacement to add
            if (rm.mod == 0b01 || rm.mod == 0b10)
            {
                address += displacement;
            }

            return address;


        }

    }

}



void Operand::setSize(int s) {
    if (s == 8 || s == 16 || s == 32 || s == 64) {
        this->size = s;
    } else {
        throw std::invalid_argument("Invalid size. Size must be 1, 2, 4, or 8 bytes.");
    }
}

int Operand::getSize() const {
    return this->size;
}



void RegOperand::setValue(uint64_t v) {
    this->reg = v;
}

uint64_t RegOperand::getValue() {
    return this->reg;
}

void MemOperand::setValue(uint64_t v) {
    
    if(this->mem == nullptr)
    {
        throw std::runtime_error("Memory pointer is null. Cannot set value.");
    }
    else if (this->address == 0)
    {
        throw std::invalid_argument("Address is null. Cannot set value.");
    }
    else if (this->size == 0)
    {
        throw std::invalid_argument("Size is null. Cannot set value.");
    }
    else
    {
        switch (size)
        {
            case 8:
                this->mem->writeByte(this->address, static_cast<uint8_t>(v));
                break;
            
            case 16:
                this->mem->writeWord(this->address, static_cast<uint16_t>(v));
                break;
            
            case 32:
                this->mem->writeDWord(this->address, static_cast<uint32_t>(v));
                break;
            
            case 64:
                this->mem->writeQWord(this->address, static_cast<uint64_t>(v));
                break;

        }
    }
}

uint64_t MemOperand::getValue() {
    if(this->mem == nullptr)
    {
        throw std::runtime_error("Memory pointer is null. Cannot get value.");
    }
    else if (this->address == 0)
    {
        throw std::invalid_argument("Address is null. Cannot get value.");
    }
    else if (this->size == 0)
    {
        throw std::invalid_argument("Size is null. Cannot get value.");
    }
    else
    {
        switch (size)
        {
            case 8:
                return this->mem->readByte(this->address);
            
            case 16:
                return this->mem->readWord(this->address);
            
            case 32:
                return this->mem->readDWord(this->address);
            
            case 64:
                return this->mem->readQWord(this->address);
            
            default:
                return 0; // or throw an exception
        }
    }
}