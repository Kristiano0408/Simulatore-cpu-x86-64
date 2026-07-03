#include "operands.hpp"
#include "instruction.hpp"
#include "cpu.hpp"
#include "memory.hpp"
#include "registerFile.hpp"
#include "helpers.hpp"

//namespace for operand fetching 
//every addressing mode has its own function that take the istruction and the referecne to then bus for loading
 //the right values inside the smart pointers that store the operands
namespace operandFetch {

    
    //fetching RM operands 
    void fetchRM(Instruction* i, RegisterFile& registers)
    {
        //declaring the registers (the type of the register is Register an enum class)
        Register source_register; 
        Register destination_register;

        //getting the r/m byte
        r_m rm = i->getRM();

        //getting the rex prefix
        uint8_t rex = i->getRexprefix();


        //Case 1: operation between register and register (maybe you can use the regToReg boolean variable)
        if(rm.mod == 0b11)   
        {
            source_register = decodeRegisterRM(rm.r_m, rex, false);
            destination_register = decodeRegisterReg(rm.reg, rex);

            auto sourceOperand = std::make_unique<RegOperand>(registers.getReg(source_register).raw());
            auto destinationOperand = std::make_unique<RegOperand>(registers.getReg(destination_register).raw());


            i->setSourceOperand(std::move(sourceOperand));
            i->setDestinationOperand(std::move(destinationOperand));

            return;

        }

         //Case 2: operation between register and memory

        uint64_t address {calculatingAddressR_M(i, registers)};

        destination_register = decodeRegisterReg(rm.reg, rex);
 
        //Source operand is an address and destination is a register
        auto sourceOperand = std::make_unique<MemOperand>(address);
        auto destinationOperand = std::make_unique<RegOperand>(registers.getReg(destination_register).raw());

        i->setSourceOperand(std::move(sourceOperand));
        i->setDestinationOperand(std::move(destinationOperand));

    }


    void fetchMR(Instruction* i, RegisterFile& registers)
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
            auto sourceOperand = std::make_unique<RegOperand>(registers.getReg(source_register).raw());
            auto destinationOperand = std::make_unique<RegOperand>(registers.getReg(destination_register).raw());


            //setting the source and destination operands
            i->setSourceOperand(std::move(sourceOperand));
            i->setDestinationOperand(std::move(destinationOperand));

            return;

        }

        //Case 2: operation between register and memory

        //the address is calculated with the calculatingAddressR_M function 
        //and the address is set to the destination operand
        uint64_t address {calculatingAddressR_M(i, registers)};

        //the source is a register
        source_register = decodeRegisterReg(rm.reg, rex);

        //operand constructors for source and destination operands
        auto sourceOperand = std::make_unique<RegOperand>(registers.getReg(source_register).raw());
        auto destinationOperand = std::make_unique<MemOperand>(address);


        i->setSourceOperand(std::move(sourceOperand));
        i->setDestinationOperand(std::move(destinationOperand));
                

    }

    void fetchFD(Instruction* i, RegisterFile& registers)
    {
        //operand constructors for source and destination operands

        //the destination is a register and the source is a memory address(displacement)
        auto sourceOperand = std::make_unique<MemOperand>(i->getDisplacement());
        auto destinationOperand = std::make_unique<RegOperand>(registers.getReg(Register::RAX).raw());


        i->setSourceOperand(std::move(sourceOperand));
        i->setDestinationOperand(std::move(destinationOperand));
    
    }

    void fetchTD(Instruction* i, RegisterFile& registers)
    {
        //operand constructors for source and destination operands

        //the source is a register and the destination is a memory address(displacement)
        auto sourceOperand = std::make_unique<RegOperand>(registers.getReg(Register::RAX).raw());
        auto destinationOperand = std::make_unique<MemOperand>(i->getDisplacement());

        i->setSourceOperand(std::move(sourceOperand));
        i->setDestinationOperand(std::move(destinationOperand));

    }

    void fetchOI(Instruction* i, RegisterFile& registers, uint32_t opcode)
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
        
        //operand constructors for destination operand 
        //the source is an immediate value and the destination is a register
        auto sourceOperand = std::make_unique<ImmediateOperand>(i->getValue());
        auto destinationOperand = std::make_unique<RegOperand>(registers.getReg(register_name[reg_index]).raw());

        i->setSourceOperand(std::move(sourceOperand)); // no source operand for immediate move
        i->setDestinationOperand(std::move(destinationOperand)); // set destination operand to the register


    }

    void fetchMI(Instruction* i, RegisterFile& registers)
    {
        //getting the r/m byte
        r_m rm = i->getRM();

        if(rm.mod == 0b11)
        {
                Register  destination_register = decodeRegisterRM(rm.r_m, i->getRexprefix(), false);

                
                
                auto sourceOperand = std::make_unique<ImmediateOperand>(i->getValue());
                auto destinationOperand = std::make_unique<RegOperand>(registers.getReg(destination_register).raw());

                i->setSourceOperand(std::move(sourceOperand));
                i->setDestinationOperand(std::move(destinationOperand)); // set destination operand to the register

            return;

        }

        uint64_t address {calculatingAddressR_M(i, registers)};

        //the source is an immediate value and the destination is a memory address
        auto sourceOperand = std::make_unique<ImmediateOperand>(i->getValue());
        auto destinationOperand = std::make_unique<MemOperand>(address);

        i->setSourceOperand(std::move(sourceOperand)); // no source operand for immediate move
        i->setDestinationOperand(std::move(destinationOperand)); // set destination operand to the register


    }

    void fetchI(Instruction* i, RegisterFile& registers)
    {
        //the source is an immediate value and the destination is a register
        auto sourceOperand = std::make_unique<ImmediateOperand>(i->getValue());
        auto destinationOperand = std::make_unique<RegOperand>(registers.getReg(Register::RAX).raw());

        i->setSourceOperand(std::move(sourceOperand));
        i->setDestinationOperand(std::move(destinationOperand));

    }
    


    uint64_t calculatingAddressR_M(Instruction* i, RegisterFile& registers)
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
                return AddressCalculator::BaseDisplacementAddressing(registers, Register::RIP, displacement);
            }
            else
            {
                //destination adress is in the register
                return AddressCalculator::indirectAddressing(registers, decodeRegisterRM(rm.r_m, rex, i->getHasSIB())) + displacement;

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
                address = i->getSIBdisplacement() + AddressCalculator::BaseScaleAddressing(registers, index, sib.scale);
            }
            //if the base is not 0b101 and the index is 0b100, normal base addressing
            else if (sib.base != 0b101 && sib.index == 0b100)
            {
                address = AddressCalculator::BaseAddressing(registers, base);
            }
            //if the base is not 0b101 and the index is not 0b100, base, index and scale addressing
            else
            {
                address += AddressCalculator::BaseIndexScaleAddressing(registers, base, index, sib.scale);
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


void Operand::setType(OperandType t) {
    this->type = t;
}

OperandType Operand::getType() const {
    return this->type;
}






