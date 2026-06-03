#include "operands.hpp"
#include "instruction.hpp"
#include "cpu.hpp"
#include "memory.hpp"
#include <string>
#include "registerFile.hpp"
#include "bus.hpp"
#include "helpers.hpp"

//namespace for operand fetching 
//every addressing mode has its own function that take the istruction and the referecne to then bus for loading
 //the right values inside the smart pointers that store the operands
namespace operandFetch {

    
    //fetching RM operands 
    void fetchRM(Instruction* i, Bus& bus)
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

            auto sourceOperand = std::make_unique<RegOperand>(bus.getCPU().getRegisters().getReg(source_register).raw());
            auto destinationOperand = std::make_unique<RegOperand>(bus.getCPU().getRegisters().getReg(destination_register).raw());


            i->setSourceOperand(std::move(sourceOperand));
            i->setDestinationOperand(std::move(destinationOperand));

            return;

        }

         //Case 2: operation between register and memory

        uint64_t address {calculatingAddressR_M(i, bus)};

        destination_register = decodeRegisterReg(rm.reg, rex);
 
        //Source operand is an address and destination is a register
        auto sourceOperand = std::make_unique<MemOperand>(bus, address, i->getInstructionId());
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
        auto destinationOperand = std::make_unique<MemOperand>(bus, address, i->getInstructionId());


        i->setSourceOperand(std::move(sourceOperand));
        i->setDestinationOperand(std::move(destinationOperand));
                

    }

    void fetchFD(Instruction* i, Bus& bus)
    {
        //operand constructors for source and destination operands

        //the destination is a register and the source is a memory address(displacement)
        auto sourceOperand = std::make_unique<MemOperand>(bus, i->getDisplacement(), i->getInstructionId());
        auto destinationOperand = std::make_unique<RegOperand>(bus.getCPU().getRegisters().getReg(Register::RAX).raw());


        i->setSourceOperand(std::move(sourceOperand));
        i->setDestinationOperand(std::move(destinationOperand));
    
    }

    void fetchTD(Instruction* i, Bus& bus)
    {
        //operand constructors for source and destination operands

        //the source is a register and the destination is a memory address(displacement)
        auto sourceOperand = std::make_unique<RegOperand>(bus.getCPU().getRegisters().getReg(Register::RAX).raw());
        auto destinationOperand = std::make_unique<MemOperand>(bus, i->getDisplacement(), i->getInstructionId());

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
        auto destinationOperand = std::make_unique<MemOperand>(bus, address, i->getInstructionId());


        i->setSourceOperand(std::move(sourceOperand)); // no source operand for immediate move
        i->setDestinationOperand(std::move(destinationOperand)); // set destination operand to the register


    }

    void fetchI(Instruction* i, Bus& bus)
    {
        //the source is an immediate value and the destination is a register
        auto sourceOperand = std::make_unique<ImmediateOperand>(i->getValue());
        auto destinationOperand = std::make_unique<RegOperand>(bus.getCPU().getRegisters().getReg(Register::RAX).raw());

        i->setSourceOperand(std::move(sourceOperand));
        i->setDestinationOperand(std::move(destinationOperand));

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



Result<void> RegOperand::setValue(anydata v, [[maybe_unused]] std::function<void()> callback)
{
    // Using std::visit to handle the variant type 

    std::visit([this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;

        //assingning the value to the register based on the type(8 bit will modify only the lower 8 bit of the register, 16 bit the lower 16 bit and so on)
        if constexpr (std::is_integral_v<T>) {
            
            constexpr uint64_t bitCount = sizeof(T) * 8;
            const uint64_t mask = (bitCount == 64) ? ~0ULL : ((1ULL << bitCount) - 1);

            // Applica la maschera coerente alla size del tipo
            const uint64_t value = static_cast<uint64_t>(arg) & mask;
            this->reg = (this->reg & ~mask) | value;
            
        }
    }, v);

    return Result<void>{true, {}};
}

Result<anydata> RegOperand::getValue([[maybe_unused]] std::function<void()> callback) 
{
    anydata result;
    int64_t mask;

    if(this->size == 64)
        mask = 0xFFFFFFFFFFFFFFFF;
    else
        mask = ((1ULL << (this->size)) - 1);

    result = this->reg & mask;


    return Result<anydata>{result, true, {}};
}


Result<void> MemOperand::setValue(anydata v, std::function<void()> callback) {
    
    if (this->size == 0)
        return Result<void>{false, {ComponentType::OPERAND, EventType::ERROR,ErrorType::INVALID_SIZE, "Size is null. Cannot set value."}};

    TypeofData dataTypeSize;
    std::array<uint8_t, 15> out{}; // Buffer per i dati da scrivere, dimensione massima di 15 byte
    
    std::visit([&](auto&& val)
    {
        using T = std::decay_t<decltype(val)>;

        if constexpr (!std::is_same_v<T, std::monostate>)
        {
            std::memcpy(out.data(), &val, sizeof(T)); // Copia i dati nel buffer
            switch(sizeof(T))
            {
                case 1:
                    dataTypeSize = TypeofData::UINT_8T;
                    break;
                case 2:
                    dataTypeSize = TypeofData::UINT_16T;
                    break;
                case 4:
                    dataTypeSize = TypeofData::UINT_32T;
                    break;
                case 8:
                    dataTypeSize = TypeofData::UINT_64T;
                    break;
                default:
                    dataTypeSize = TypeofData::UNKNOWN;
                    break;
            }
        }
    }, v);

    if(!requestSent)
    {
        //sending the write request to the cache manager

        //debugLog("MemOperand: Sending write request to address " + to_string_hex(this->address) + " with value " + to_string_hex(v) + " and size " + std::to_string(this->size) + " bytes.");
        requestSent = true;

        bus.getCPU().getCacheManager().setRequest(std::make_unique<CacheRequest>(RequestType::WRITE, dataTypeSize, this->address, out, false, instructionID, callback));

        return Result<void>{false, {ComponentType::OPERAND, EventType::ERROR, ErrorType::WAITING_MEMORY, "Write request sent. Waiting for completion."}};
    }
    else
    {
        //request already sent, waiting for completion

        debugLog("MemOperand: Write request already sent to address " + to_string_hex(this->address) + ". Waiting for completion.");

        //checking if the request is completed
        auto it = bus.getCPU().cacheResponseQueue.find(instructionID);

        if (it != bus.getCPU().cacheResponseQueue.end())
        {
            //request completed
            debugLog("MemOperand: Write request completed for address " + to_string_hex(this->address) + ".");
            
            //extracting the result
            Result<void> result;
            Result<anydata>& response = *(it->second);

            result.success = response.success;
            result.errorInfo = response.errorInfo;
            
            if (result.success)
            {
                debugLog("MemOperand: Write request successful for address " + to_string_hex(this->address) + ".");
            }
            else
            {
                debugLog("MemOperand: Write request failed for address " + to_string_hex(this->address) + ": " + response.errorInfo.message);
            }

            bus.getCPU().cacheResponseQueue.erase(it);
            requestSent = false; //resetting the flag for future requests

            return result;
        }
        else
        {
            //request not completed
            debugLog("MemOperand: Write request not completed for address " + to_string_hex(this->address) + ".");
            return Result<void>{false, {ComponentType::OPERAND, EventType::ERROR, ErrorType::WAITING_MEMORY, "Write request not completed yet."}};
        }

    }
}

Result<anydata> MemOperand::getValue(std::function<void()> callback) {
    

    if (this->size == 0)
    {
        return Result<anydata>{{},false, {ComponentType::OPERAND, EventType::ERROR,ErrorType::INVALID_SIZE, "Size is null. Cannot get value."}};
    }

    //extracting value first from cache, then from memory if necessary
    Result<anydata> result;

    if(!readRequestSent)
    {
        //sending the read request to the cache manager

        debugLog("MemOperand: Sending read request to address " + to_string_hex(this->address) + " with size " + std::to_string(this->size) + " bytes.");
        readRequestSent = true;
        TypeofData dataTypeSize;
        switch (this->size)
        {
            case 8:
                dataTypeSize = TypeofData::UINT_8T;
                break;
            case 16:
                dataTypeSize = TypeofData::UINT_16T;
                break;
            case 32:
                dataTypeSize = TypeofData::UINT_32T;
                break;
            case 64:
                dataTypeSize = TypeofData::UINT_64T;
                break;
            default:
                dataTypeSize = TypeofData::UNKNOWN;
                break;
        }

        bus.getCPU().getCacheManager().setRequest(std::make_unique<CacheRequest>(RequestType::READ, dataTypeSize, this->address,std::array<uint8_t, 15>{}, false, instructionID, callback));

        return Result<anydata>{{}, false, {ComponentType::OPERAND, EventType::ERROR, ErrorType::WAITING_MEMORY, "Read request sent. Waiting for completion."}};
    }
    else
    {
        //request already sent, waiting for completion

        debugLog("MemOperand: Read request already sent to address " + to_string_hex(this->address) + ". Waiting for completion.");

        //checking if the request is completed
        auto it = bus.getCPU().cacheResponseQueue.find(instructionID);

        if (it != bus.getCPU().cacheResponseQueue.end())
        {
            //request completed
            debugLog("MemOperand: Read request completed for address " + to_string_hex(this->address) + ".");
            
            //extracting the result
            Result<anydata>& response = *(it->second);

            result.success = response.success;
            result.errorInfo = response.errorInfo;
            
            if (result.success)
            {
                debugLog("MemOperand: Read request successful for address " + to_string_hex(this->address) + ".");
                //getting the value based on the size
                std::memcpy(&result.data, &response.data, sizeof(response.data));
            }
            else
            {
                debugLog("MemOperand: Read request failed for address " + to_string_hex(this->address) + ": " + response.errorInfo.message);
            }

            bus.getCPU().cacheResponseQueue.erase(it);
            readRequestSent = false; //resetting the flag for future requests

            return result;
        }
        else
        {
            //request not completed
            debugLog("MemOperand: Read request not completed for address " + to_string_hex(this->address) + ".");
            return Result<anydata>{{}, false, {ComponentType::OPERAND, EventType::ERROR, ErrorType::WAITING_MEMORY, "Read request not completed yet."}};
        }
    }

}

Result<void> ImmediateOperand::setValue(anydata v, [[maybe_unused]] std::function<void()> callback) {
    this->value = v;
    return Result<void>{true, {ComponentType::OPERAND, EventType::NONE, ErrorType::NONE, ""}};
}

Result<anydata> ImmediateOperand::getValue([[maybe_unused]] std::function<void()> callback) {
    return Result<anydata>{this->value, true, {ComponentType::OPERAND, EventType::NONE, ErrorType::NONE, ""}};
}
