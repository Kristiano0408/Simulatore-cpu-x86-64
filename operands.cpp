#include "operands.hpp"
#include "instruction.hpp"
#include "cpu.hpp"
#include "memory.hpp"
#include <string>
#include "registerFile.hpp"
#include "bus.hpp"

namespace operandFetch {

    //fetching RM operands 
    void fetchRM(Instruction* i, Bus& bus)
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

            auto sourceOperand = std::make_unique<RegOperand>(bus.getCPU().getRegisters().getReg(source_register).raw());
            auto destinationOperand = std::make_unique<RegOperand>(bus.getCPU().getRegisters().getReg(destination_register).raw());


            i->setSourceOperand(std::move(sourceOperand));
            i->setDestinationOperand(std::move(destinationOperand));

            return;

        }

        //Case 2: operation between register and memory

        uint64_t address {calculatingAddressR_M(i, bus)};

        std ::cout<<std::hex << "Address: " << address <<  std::dec <<std::endl;

        std::cout <<std::dec << "Address: " << address << std::endl;


        destination_register = decodeRegisterReg(rm.reg, rex);

        
        //Source operand is an address and destination is a register
        auto sourceOperand = std::make_unique<MemOperand>(bus.getMemory(), address);
        auto destinationOperand = std::make_unique<RegOperand>(bus.getCPU().getRegisters().getReg(destination_register).raw());

        i->setSourceOperand(std::move(sourceOperand));
        i->setDestinationOperand(std::move(destinationOperand));
                

    }

    void fetchMR(Instruction* i, Bus& bus)
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
            auto sourceOperand = std::make_unique<RegOperand>(bus.getCPU().getRegisters().getReg(source_register).raw());
            auto destinationOperand = std::make_unique<RegOperand>(bus.getCPU().getRegisters().getReg(destination_register).raw());


            //setting the source and destination operands
            i->setSourceOperand(std::move(sourceOperand));
            i->setDestinationOperand(std::move(destinationOperand));

            return;

        }

        //Case 2: operation between register and memory

        //the address is calculated with the calculatingAddressR_M function 
        //and the address is set to the destination operand
        uint64_t address {calculatingAddressR_M(i, bus)};

        //the source is a register
        source_register = decodeRegisterReg(rm.reg, rex);

        //operand constructors for source and destination operands
        auto sourceOperand = std::make_unique<RegOperand>(bus.getCPU().getRegisters().getReg(source_register).raw());
        auto destinationOperand = std::make_unique<MemOperand>(bus.getMemory(), address);


        i->setSourceOperand(std::move(sourceOperand));
        i->setDestinationOperand(std::move(destinationOperand));
                

    }

    void fetchFD(Instruction* i, Bus& bus)
    {
        //operand constructors for source and destination operands

        //the destination is a register aand the source is a memory address(displacement)
        auto sourceOperand = std::make_unique<MemOperand>(bus.getMemory(), i->getDisplacement());
        auto destinationOperand = std::make_unique<RegOperand>(bus.getCPU().getRegisters().getReg(Register::RAX).raw());


        i->setSourceOperand(std::move(sourceOperand));
        i->setDestinationOperand(std::move(destinationOperand));
    
    }

    void fetchTD(Instruction* i, Bus& bus)
    {
        //operand constructors for source and destination operands

        //the source is a register and the destination is a memory address(displacement)
        auto sourceOperand = std::make_unique<RegOperand>(bus.getCPU().getRegisters().getReg(Register::RAX).raw());
        auto destinationOperand = std::make_unique<MemOperand>(bus.getMemory(), i->getDisplacement());

        i->setSourceOperand(std::move(sourceOperand));
        i->setDestinationOperand(std::move(destinationOperand));

    }

    void fetchOI(Instruction* i, Bus& bus, uint32_t opcode)
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
        auto destinationOperand = std::make_unique<RegOperand>(bus.getCPU().getRegisters().getReg(register_name[reg_index]).raw());

        i->setSourceOperand(std::move(sourceOperand)); // no source operand for immediate move
        i->setDestinationOperand(std::move(destinationOperand)); // set destination operand to the register


    }

    void fetchMI(Instruction* i, Bus& bus)
    {
        //getting the r/m byte
        r_m rm = i->getRM();

        if(rm.mod == 0b11)
        {
                Register  destination_register = decodeRegisterRM(rm.r_m, i->getRexprefix(), false);

                
                
                auto sourceOperand = std::make_unique<ImmediateOperand>(i->getValue());
                auto destinationOperand = std::make_unique<RegOperand>(bus.getCPU().getRegisters().getReg(destination_register).raw());

                i->setSourceOperand(std::move(sourceOperand));
                i->setDestinationOperand(std::move(destinationOperand)); // set destination operand to the register

            return;

        }

        uint64_t address {calculatingAddressR_M(i, bus)};

        //the source is an immediate value and the destination is a memory address
        auto sourceOperand = std::make_unique<ImmediateOperand>(i->getValue());
        auto destinationOperand = std::make_unique<MemOperand>(bus.getMemory(), address);


        i->setSourceOperand(std::move(sourceOperand)); // no source operand for immediate move
        i->setDestinationOperand(std::move(destinationOperand)); // set destination operand to the register


    }

    uint64_t calculatingAddressR_M(Instruction* i, Bus& bus)
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
                return AddressCalculator::BaseDisplacementAddressing(bus, Register::RIP, displacement);
            }
            else
            {
                //destination adress is in the register
                return AddressCalculator::indirectAddressing(bus, decodeRegisterRM(rm.r_m, rex, i->getHasSIB())) + displacement;

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
                address = i->getSIBdisplacement() + AddressCalculator::BaseScaleAddressing(bus, index, sib.scale);
            }
            //if the base is not 0b101 and the index is 0b100, normal base addressing
            else if (sib.base != 0b101 && sib.index == 0b100)
            {
                address = AddressCalculator::BaseAddressing(bus, base);
            }
            //if the base is not 0b101 and the index is not 0b100, base, index and scale addressing
            else
            {
                address += AddressCalculator::BaseIndexScaleAddressing(bus, base, index, sib.scale);
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



Result<void> RegOperand::setValue(uint64_t v) 
{
    this->reg = v;
    return Result<void>{true, {}};
}

Result<uint64_t> RegOperand::getValue() 
{
    return Result<uint64_t>{this->reg, true};
}

Result<void> MemOperand::setValue(uint64_t v) {
    

    /*else if (this->address == 0)
    {
        throw std::invalid_argument("Address is null. Cannot set value.");
    }*/
    if (this->size == 0)
    {   
        return Result<void>{false, {ComponentType::OPERAND, EventType::ERROR,ErrorType::INVALID_SIZE, "Size is null. Cannot set value."}};
    }
    else
    {
        switch (size)
        {
            case 8:
                return this->cache.write<uint8_t>(this->address, static_cast<uint8_t>(v));

            case 16:
                return this->cache.write<uint16_t>(this->address, static_cast<uint16_t>(v));

            case 32:
                return this->cache.write<uint32_t>(this->address, static_cast<uint32_t>(v));

            case 64:
                return this->cache.write<uint64_t>(this->address, v);

        }
    }
}

Result<uint64_t> MemOperand::getValue() {
    /*else if (this->address == 0)
    {
        throw std::invalid_argument("Address is null. Cannot get value.");
    }*/
    if (this->size == 0)
    {
        return Result<uint64_t>{0,false, {ComponentType::OPERAND, EventType::ERROR,ErrorType::INVALID_SIZE, "Size is null. Cannot get value."}};
    }
    else
    {
        //extarcting value first from cache, the from memory if necessary



        Result<uint64_t> result;

        uint64_t real_value;

       switch(size) 
       {
            case 8: {
                Result<uint8_t> res = cache.read<uint8_t>(address);
                if (res.success) std::memcpy(&real_value, &res.data, 1);
                result.success = res.success;
                result.errorInfo = res.errorInfo;
                break;
            }
            case 16: {
                Result<uint16_t> res = cache.read<uint16_t>(address);
                if (res.success) std::memcpy(&real_value, &res.data, 2);
                result.success = res.success;
                result.errorInfo = res.errorInfo;
                break;
            }
            case 32: {
                Result<uint32_t> res = cache.read<uint32_t>(address);
                if (res.success) std::memcpy(&real_value, &res.data, 4);
                result.success = res.success;
                result.errorInfo = res.errorInfo;
                break;
            }
            case 64: {
                Result<uint64_t> res = cache.read<uint64_t>(address);
                if (res.success) real_value = res.data;
                result.success = res.success;
                result.errorInfo = res.errorInfo;
                break;
            }
            default:
                return Result<uint64_t>{0, false, {ComponentType::OPERAND, EventType::ERROR, ErrorType::INVALID_SIZE, "Invalid size"}};
        }

    }
}

Result<void> ImmediateOperand::setValue(uint64_t v) {
    this->value = v;
    return Result<void>{true, {ComponentType::OPERAND, EventType::NONE, ErrorType::NONE, ""}};
}

Result<uint64_t> ImmediateOperand::getValue() {
    return Result<uint64_t>{this->value, true, {ComponentType::OPERAND, EventType::NONE, ErrorType::NONE, ""}};
}
