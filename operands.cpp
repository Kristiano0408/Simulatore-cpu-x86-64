#include "operands.hpp"
#include "instruction.hpp"
#include "controlUnit.hpp"
#include "memory.hpp"
#include <string>


namespace operandFetch {

    //fetching RM operands 
    void fetchRM(Instruction* i, CU* controlUnit, Memory* ram)
    {
        //declaring the registers (the type of the register is Register an enum class)
        Register source_register; 
        Register destination_register;

        //std::cout << "fetchRM" << std::endl;
        //getting the r/m byte
        r_m rm = i->getRM();

        //getting the rex prefix
        uint8_t rex = i->getRexprefix();

        //Case 1: operation between register and register
        if(rm.mod == 0b11)   
        {
            source_register = decodeRegisterReg(rm.reg, rex);
            destination_register = decodeRegisterRM(rm.r_m, rex, false);

            Operand* sourceOperand = new RegOperand(controlUnit->getRegisters().getReg(source_register).raw());
            Operand* destinationOperand = new RegOperand(controlUnit->getRegisters().getReg(destination_register).raw());


            i->setSourceOperand(sourceOperand);
            i->setDestinationOperand(destinationOperand);

            return;

        }

        //Case 2: operation between register and memory
    
        uint64_t address {calculatingAddressR_M(i,controlUnit, ram)};

        std ::cout<<std::hex << "Address: " << address <<  std::dec <<std::endl;

        std::cout <<std::dec << "Address: " << address << std::endl;


        destination_register = decodeRegisterReg(rm.reg, rex);

        
        //Source operand is an address and destination is a register
        Operand* sourceOperand = new MemOperand(ram, address);
        Operand* destinationOperand = new RegOperand(controlUnit->getRegisters().getReg(destination_register).raw());

        i->setSourceOperand(sourceOperand);
        i->setDestinationOperand(destinationOperand);
                

    }

    void fetchMR(Instruction* i, CU* controlUnit, Memory* ram)
    {
        //declaring the registers (the type of the register is Register an enum class)
        Register source_register; 
        Register destination_register;

        //getting the r/m byte
        r_m rm = i->getRM();

        //getting the rex prefix
        uint8_t rex = i->getRexprefix();

        //Case 1: operation between register and register
        if(rm.mod == 0b11)   
        {
            source_register = decodeRegisterReg(rm.reg, rex);
            destination_register = decodeRegisterRM(rm.r_m, rex, false);

            //operand constructors for source and destination operands
            Operand* sourceOperand = new RegOperand(controlUnit->getRegisters().getReg(source_register).raw());
            Operand* destinationOperand = new RegOperand(controlUnit->getRegisters().getReg(destination_register).raw());


            //setting the source and destination operands
            i->setSourceOperand(sourceOperand);
            i->setDestinationOperand(destinationOperand);

            return;

        }

        //Case 2: operation between register and memory

        //the address is calculated with the calculatingAddressR_M function 
        //and the address is set to the destination operand
        uint64_t address {calculatingAddressR_M(i,controlUnit, ram)};

        //the source is a register
        source_register = decodeRegisterReg(rm.reg, rex);

        //operand constructors for source and destination operands
        Operand* sourceOperand = new RegOperand(controlUnit->getRegisters().getReg(source_register).raw());
        Operand* destinationOperand = new MemOperand(ram, address);

        i->setSourceOperand(sourceOperand);
        i->setDestinationOperand(destinationOperand);
                

    }

    void fetchFD(Instruction* i, CU* controlUnit, Memory* ram)
    {
        //operand constructors for source and destination operands

        //the destination is a register aand the source is a memory address(displacement)
        Operand* sourceOperand = new MemOperand(ram, i->getDisplacement());
        Operand* destinationOperand = new RegOperand(controlUnit->getRegisters().getReg(Register::RAX).raw());


        i->setSourceOperand(sourceOperand);
        i->setDestinationOperand(destinationOperand);
    
    }

    void fetchTD(Instruction* i, CU* controlUnit, Memory* ram)
    {
        //operand constructors for source and destination operands

        //the source is a register and the destination is a memory address(displacement)
        Operand* sourceOperand = new RegOperand(controlUnit->getRegisters().getReg(Register::RAX).raw());
        Operand* destinationOperand = new MemOperand(ram, i->getDisplacement());

        i->setSourceOperand(sourceOperand);
        i->setDestinationOperand(destinationOperand);

    }

    void fetchOI(Instruction* i, CU* controlUnit, Memory* ram, uint32_t opcode)
    {
        Register register_name[16] = {Register::RAX, Register::RCX, Register::RDX, Register::RBX, Register::RSP, Register::RBP, Register::RSI, Register::RDI,
                                    Register::R8,Register::R9, Register::R10, Register::R11, Register::R12, Register::R13, Register::R14, Register::R15};
    
        
        //getting the register index from the opcode
        int reg_index = opcode & 0x07;
    
        //if Rex.b = 1, add 8 to the register index
        if(i->getRexprefix() & 0x01)  
        {
        reg_index += 8;
        }
        
        //operand constructors for destination operand (source is null because we are moving immediate value)
        Operand* sourceOperand = new ImmediateOperand(i->getValue());
        Operand* destinationOperand = new RegOperand(controlUnit->getRegisters().getReg(register_name[reg_index]).raw());
        
        i->setSourceOperand(sourceOperand); // no source operand for immediate move
        i->setDestinationOperand(destinationOperand); // set destination operand to the register


    }

    void fetchMI(Instruction* i, CU* controlUnit, Memory* ram)
    {
        //getting the r/m byte
        r_m rm = i->getRM();

        if(rm.mod == 0b11)
        {
                Register  destination_register = decodeRegisterRM(rm.r_m, i->getRexprefix(), false);
                
                Operand* sourceOperand = new ImmediateOperand(i->getValue());
                Operand* destinationOperand = new RegOperand(controlUnit->getRegisters().getReg(destination_register).raw());

                i->setSourceOperand(sourceOperand); 
                i->setDestinationOperand(destinationOperand); // set destination operand to the register

            return;

        }

        uint64_t address {calculatingAddressR_M(i,controlUnit, ram)};

        //the source is an immediate value and the destination is a memory address
        Operand* sourceOperand = new ImmediateOperand(i->getValue());
        Operand* destinationOperand = new MemOperand(ram, address);


        i->setSourceOperand(sourceOperand); // no source operand for immediate move
        i->setDestinationOperand(destinationOperand); // set destination operand to the register


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
                return AddressCalculator::BaseDisplacementAddressing(controlUnit, Register::RIP, displacement);
            }
            else
            {
                //destination adress is in the register
                return AddressCalculator::indirectAddressing(controlUnit, decodeRegisterRM(rm.r_m, rex, i->getHasSIB())) + displacement;

            }
        }
        else
        {
            //calculation of the address with SIB
            Register base = decodeRegisterSIB_base(sib.base, rex, i->getHasSIB());
            Register index = decodeRegisterSIB_index(sib.index, rex, i->getHasSIB());
            uint64_t address = 0;

            //if the base is 0b101 and the index is 0b100, there is no index and base is 32 bit displacement
            if (sib.base == 0b101 && sib.index == 0b100 && rm.mod == 0b00)
            {
                address = i->getSIBdisplacement();
            }
            //if the base is 0b101 and the index is not 0b100, base(displacement), index and scale addressing
            else if (sib.base == 0b101 && sib.index != 0b100 && rm.mod == 0b00)
            {
                address = i->getSIBdisplacement() + AddressCalculator::BaseScaleAddressing(controlUnit, index, sib.scale);
            }
            //if the base is not 0b101 and the index is 0b100, normal base addressing
            else if (sib.base != 0b101 && sib.index == 0b100)
            {
                address = AddressCalculator::BaseAddressing(controlUnit, base);
            }
            //if the base is not 0b101 and the index is not 0b100, base, index and scale addressing
            else
            {
                address += AddressCalculator::BaseIndexScaleAddressing(controlUnit, base, index, sib.scale);
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
    /*else if (this->address == 0)
    {
        throw std::invalid_argument("Address is null. Cannot set value.");
    }*/
    else if (this->size == 0)
    {
        throw std::invalid_argument("Size is null. Cannot set value.");
    }
    else
    {
        switch (size)
        {
            case 8:
                this->mem->writeGeneric<uint8_t>(this->address, static_cast<uint8_t>(v));
                break;
            
            case 16:
                this->mem->writeGeneric<uint16_t>(this->address, static_cast<uint16_t>(v));
                break;
            
            case 32:
                this->mem->writeGeneric<uint32_t>(this->address, static_cast<uint32_t>(v));
                break;
            
            case 64:
                this->mem->writeGeneric<uint64_t>(this->address, v);
                break;

        }
    }
}

uint64_t MemOperand::getValue() {
    if(this->mem == nullptr)
    {
        throw std::runtime_error("Memory pointer is null. Cannot get value.");
    }
    /*else if (this->address == 0)
    {
        throw std::invalid_argument("Address is null. Cannot get value.");
    }*/
    else if (this->size == 0)
    {
        throw std::invalid_argument("Size is null. Cannot get value.");
    }
    else
    {
        switch (size)
        {
            case 8:
                return this->mem->readGeneric<uint8_t>(this->address);
            
            case 16:
                return this->mem->readGeneric<uint16_t>(this->address);
            
            case 32:
                return this->mem->readGeneric<uint32_t>(this->address);
            
            case 64:
                return this->mem->readGeneric<uint64_t>(this->address);
            
            default:
                return 0; // or throw an exception
        }
    }
}

void ImmediateOperand::setValue(uint64_t v) {
    this->value = v;
}

uint64_t ImmediateOperand::getValue() {
    return this->value;
}
