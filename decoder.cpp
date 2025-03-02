
#include "opcode_map.cpp"
#include "decoder.hpp"
#include <iostream>
#include "helpers.hpp"
#include <memory>
//#include "addressingMode.hpp"


Decoder::Decoder()
{

    //nothing to do here
}

Decoder::~Decoder()
{
    //nothing to do here
}




InstructionInfo Decoder::LenghtOfInstruction(int32_t opcode, uint8_t prefix[4],int numPrefixes, bool rex, int16_t rexprefix)
{
    InstructionInfo info;
    info.opcode = opcode;
    info.prefixCount = numPrefixes;

    //search the opcode in the map

    auto it = opcodeMap.find(opcode);
    if (it != opcodeMap.end())
    {
        info.totalLength = it->second.totalLength; //the lenght is satndard for 32 bits
        info.opcodeLength = it->second.opcodeLength;
        info.additionalBytes = it->second.additionalBytes; //additional bytes standars for 32 bits
        info.description = it->second.description;
        info.numOperands = it->second.numOperands;
        info.operandLength = it->second.operandLength;
        info.hasModRM = it->second.hasModRM;
        info.hasDisplacement = it->second.hasDisplacement;
        info.hasImmediate = it->second.hasImmediate;
        
    }
    else
    {
        //if the opcode is not found
        info.totalLength = 0;
        info.opcodeLength = 0;
        info.additionalBytes = 0;
        info.numOperands = 0;
        info.operandLength = 0;
        info.hasModRM = false;
        info.hasDisplacement = false;
        info.hasImmediate = false;
        info.description = "Unknown instruction";

        return info;
    }

    //setting the prefix and changing the lenght of the instruction if there is IO
    for (int i = 0; i < numPrefixes; i++)
    {
        //if there is the prefix for 16bits opernads
        if(prefix[i] == 0x66 and info.hasImmediate)
        {
            //the lenght is reduced by 4 bytes 
            info.totalLength -= (2* info.numOperands);
            info.additionalBytes -= (2* info.numOperands);
            info.operandLength -= 2;
            info.prefix[i] = prefix[i];
            
            break;
        }

        info.prefix[i] = prefix[i];
        
    }

    if (rex and (rexprefix & 0x08) and info.hasImmediate)
    {
        //the lenght is increased by 4 bytes 
        info.totalLength += (4* info.numOperands);
        info.additionalBytes += (4* info.numOperands);
        info.operandLength += 4;
        info.rex = true;
        info.rexprefix = rexprefix;
           
    }
    else if (rex and !info.hasImmediate)
    {
        info.rex = true;
        info.rexprefix = rexprefix;
    }

    else
    {
        info.rex = false;
        info.rexprefix = 0;
    }


    return info;
    

   
}

r_m Decoder::decodeRM(int8_t R_M)
{
    r_m rm;
    rm.byte_r_m = R_M;
    rm.mod = (R_M >> 6) & 0b11;
    rm.reg = (R_M >> 3) & 0b111;
    rm.r_m = (R_M & 0b111);

    return rm;
}

Instruction* Decoder::decodeInstruction(InstructionInfo instruction, CU* controlUnit)
{
    int position = 0;

    position = instruction.prefixCount;

    //searching the rex prefix
    if (instruction.rex)
    {
        position++;
    }

    position += instruction.opcodeLength;

    std::cout << "Position: " << position << std::endl;

    
    //move (in future will be implemented a function for controlloing the opcode)
    if (isMoveInstruction(instruction.opcode))
    {
        return decodeMov(instruction, position, controlUnit);
    }


    return nullptr;
   
    






}
       
Instruction* Decoder::decodeMov(InstructionInfo instruction, int position, CU* controlUnit)
{
    //create the instruction
    MoveInstruction* mov = new MoveInstruction();

    //create the addressing mode
    AddressingMode* addressingMode = new AddressingMode(controlUnit);


    // creating the struct for the r/m operand
    r_m rm;

    int dimOperands = 0;
    int64_t value = 0;
    int32_t displacement = 0;
    int8_t R_M = 0;
    uint8_t prefix[4];

    //setting the parameters of instruction
    mov->setOpcode(instruction.opcode);
    
    mov->setPrefix(instruction.prefix);

    mov->setNumPrefixes(instruction.prefixCount);

    mov->setRex(instruction.rex);

    mov->setRexprefix(instruction.rexprefix);

    //if there is only an immediate operand
    //if(instruction.hasImmediate and !instruction.hasModRM)
    if(isMoveInstructionIO(instruction.opcode))
    {

        switch (instruction.opcode)
        {
            case 0xB8:
                mov->setD_register("RAX");
                break;
            case 0xB9:
                mov->setD_register("RCX");
                break;
            case 0xBA:
                mov->setD_register("RDX");
                break;
            case 0xBB:
                mov->setD_register("RBX");
                break;
            case 0xBC:
                mov->setD_register("RSP");
                break;
            case 0xBD:
                mov->setD_register("RBP");
                break;
            case 0xBE:
                mov->setD_register("RSI");
                break;
            case 0xBF:
                mov->setD_register("RDI");
                break;
            default:
                break;
        }
        //getting the dimension of operands
        

        //setting the value of the operands
        for (int i = 0; i < instruction.operandLength; i++)
        {
            value |= (static_cast<int64_t> (instruction.instruction[position + i]))<< (8 * i);
            std::cout <<  std::hex <<"Value: " << value << std::endl;
         
        }
        
        //casting the value
        if (!instruction.rex)
        {
            value= static_cast<int32_t>(value);
            std::cout <<  std::hex <<"Value: " << value << std::endl;

            
            for (int i = 0; i < instruction.prefixCount; i++)
            {
                if (instruction.prefix[i] == 0x66)
                {
                    value = static_cast<int16_t>(value);
                    std::cout <<  std::hex <<"Value: " << value << std::endl;
                    break;
                }
            }
        }

        

        mov->setValue(value);
        std::cout << "Value: " << mov->getValue() << std::endl;
    
    }


    //if there is r/m operand
    if (instruction.hasModRM && !instruction.hasImmediate)
    {
        R_M = instruction.instruction[position];
        position++;
        
        rm = decodeRM(R_M);

        //register to register
        if (rm.mod == 0b11)
        {
            //register to register
            mov->setS_register(decodeRegisterReg(rm.reg, instruction));
            mov->setD_register(decodeRegisterRM(rm.r_m, instruction));
        }

        //adress no displacement
        else if (rm.mod == 0b00)
        {
            if (isMoveR_M_reg_mem(instruction.opcode))
                //register to memory
                mov->setS_register(decodeRegisterReg(rm.reg, instruction));
            else
                //memory to register
                mov->setD_register(decodeRegisterReg(rm.reg, instruction));


            if (rm.r_m == 0b100)
            {
                
                //there is SIB
                SIB sib = decodeSIB(instruction.instruction[position]);
                position++;

                //convertin the register to string
                std::string base = decodeRegisterSIB_base(sib.base, instruction);
                std::string index = decodeRegisterSIB_index(sib.index, instruction);

                //calcuating the adress of the memory
                if (isMoveR_M_reg_mem(instruction.opcode))
                    mov->setD_address(addressingMode->scaledIndexedAddressing(base, index, sib.scale));
                
                else mov->setS_address(addressingMode->scaledIndexedAddressing(base, index, sib.scale));
               
            }
            
            else if (rm.r_m == 0b101)
            {
                //there is displacement
                for (int i = 0; i < 4; i++)
                {
                    displacement |= (static_cast<int32_t> (instruction.instruction[position + i]))<< (8 * i);
                }
                position += 4;

                //calcuating the adress of the memory
                if (isMoveR_M_reg_mem(instruction.opcode))
                    mov->setD_address(addressingMode->directAddressing(displacement));

                else mov->setS_address(addressingMode->directAddressing(displacement));
            }
            
            else
            {
                //destination register
                if (isMoveR_M_reg_mem(instruction.opcode))
                    mov->setD_address(addressingMode->indirectAddressing(decodeRegisterRM(rm.r_m, instruction)));
                else
                    mov->setS_address(addressingMode->indirectAddressing(decodeRegisterRM(rm.r_m, instruction)));
            }
           
        }

        //address  + 8 bit displacement
        else if (rm.mod == 0b01)
        {
            if (isMoveR_M_reg_mem(instruction.opcode))
                //register to memory
                mov->setS_register(decodeRegisterReg(rm.reg, instruction));
            else
                //memory to register
                mov->setD_register(decodeRegisterReg(rm.reg, instruction));

            if (rm.r_m == 0b100)
            {
                

                //there is SIB
                SIB sib = decodeSIB(instruction.instruction[position]);
                position++;

                //convertin the register to string
                std::string base = decodeRegisterSIB_base(sib.base, instruction);
                std::string index = decodeRegisterSIB_index(sib.index, instruction);

                //getting the displacement
                displacement = instruction.instruction[position];
                position++;

                //calcuating the adress of the memory
                if (isMoveR_M_reg_mem(instruction.opcode))
                    mov->setD_address(addressingMode->scaledIndexedDisplacementAddressing(base, index, sib.scale, displacement));
                
                else mov->setS_address(addressingMode->scaledIndexedDisplacementAddressing(base, index, sib.scale, displacement));
                
            }
            
            else
            {
                displacement = instruction.instruction[position];
                position++;
                //destination register
                if (isMoveR_M_reg_mem(instruction.opcode))
                    mov->setD_address(addressingMode->baseDisplacementAddressing(decodeRegisterRM(rm.r_m, instruction), displacement));
                else
                    mov->setS_address(addressingMode->baseDisplacementAddressing(decodeRegisterRM(rm.r_m, instruction), displacement));
                
                position++;
            }
        }

        //adress + 32 bit displacement
        else if (rm.mod == 0b10)
        {
            if (isMoveR_M_reg_mem(instruction.opcode))
                //register to memory
                mov->setS_register(decodeRegisterReg(rm.reg, instruction));
            else
                //memory to register
                mov->setD_register(decodeRegisterReg(rm.reg, instruction));

            if (rm.r_m == 0b100)
            {
                //there is SIB
                SIB sib = decodeSIB(instruction.instruction[position]);
                position++;

                //convertin the register to string
                std::string base = decodeRegisterSIB_base(sib.base, instruction);
                std::string index = decodeRegisterSIB_index(sib.index, instruction);

                //getting the displacement
                for (int i = 0; i < 4; i++)
                {
                    displacement |= (static_cast<int32_t> (instruction.instruction[position + i]))<< (8 * i);

                }

                position += 4;

                //calcuating the adress of the memory
                if (isMoveR_M_reg_mem(instruction.opcode))
                    mov->setD_address(addressingMode->scaledIndexedDisplacementAddressing(base, index, sib.scale, displacement));
                
                else mov->setS_address(addressingMode->scaledIndexedDisplacementAddressing(base, index, sib.scale, displacement));
            }
            else
            {
                for (int i = 0; i < 4; i++)
                {
                    displacement |= (static_cast<int32_t> (instruction.instruction[position + i]))<< (8 * i);
                }

                position += 4;

                //destination 

                if (isMoveR_M_reg_mem(instruction.opcode))
                    mov->setD_address(addressingMode->baseDisplacementAddressing(decodeRegisterRM(rm.r_m, instruction), displacement));
                else
                    mov->setS_address(addressingMode->baseDisplacementAddressing(decodeRegisterRM(rm.r_m, instruction), displacement));


            }
            

            
        }


        
        
        

        
        
    





    }

    delete addressingMode;

    return mov;
}

const std::string& Decoder::decodeRegisterReg(uint8_t reg, InstructionInfo info)
{
    if (info.rexprefix & 0x04)
    {
        reg += 8;
    }
    
    switch (reg)
    {
        case 0:
            return "RAX";
            break;
        case 1:
            return "RCX";
            break;
        case 2:
            return "RDX";
            break;
        case 3:
            return "RBX";
            break;
        case 4:
            return "RSP";
            break;
        case 5:
            return "RBP";
            break;
        case 6:
            return "RSI";
            break;
        case 7:
            return "RDI";
            break;
        case 8:
            return "R8";
            break;
        case 9:
            return "R9";
            break;
        case 10:
            return "R10";
            break;
        case 11:
            return "R11";
            break;
        case 12:
            return "R12";
            break;
        case 13:
            return "R13";
            break;
        case 14:
            return "R14";
            break;
        case 15:
            return "R15";
            break;
        default:
            return "Unknown register";
            break;
    }
}

const std::string& Decoder::decodeRegisterRM(uint8_t reg, InstructionInfo info)
{
    if (info.rexprefix & 0x01 && !info.hasSIB)
    {
        reg += 8;
    }
    
    switch (reg)
    {
        case 0:
            return "RAX";
            break;
        case 1:
            return "RCX";
            break;
        case 2:
            return "RDX";
            break;
        case 3:
            return "RBX";
            break;
        case 4:
            return "RSP";
            break;
        case 5:
            return "RBP";
            break;
        case 6:
            return "RSI";
            break;
        case 7:
            return "RDI";
            break;
        case 8:
            return "R8";
            break;
        case 9:
            return "R9";
            break;
        case 10:
            return "R10";
            break;
        case 11:
            return "R11";
            break;
        case 12:
            return "R12";
            break;
        case 13:
            return "R13";
            break;
        case 14:
            return "R14";
            break;
        case 15:
            return "R15";
            break;
        default:
            return "Unknown register";
            break;
    }
}

const std::string& Decoder::decodeRegisterSIB_base(uint8_t reg, InstructionInfo info)
{
    if (info.rexprefix & 0x01 && info.hasSIB)
    {
        reg += 8;
    }

    switch (reg)
    {
        case 0:
            return "RAX";
            break;
        case 1:
            return "RCX";
            break;
        case 2:
            return "RDX";
            break;
        case 3:
            return "RBX";
            break;
        case 4:
            return "RSP";
            break;
        case 5:
            return "RBP";
            break;
        case 6:
            return "RSI";
            break;
        case 7:
            return "RDI";
            break;
        case 8:
            return "R8";
            break;
        case 9:
            return "R9";
            break;
        case 10:
            return "R10";
            break;
        case 11:
            return "R11";
            break;
        case 12:
            return "R12";
            break;
        case 13:
            return "R13";
            break;
        case 14:
            return "R14";
            break;
        case 15:
            return "R15";
            break;
        default:
            return "Unknown register";
            break;
    }

}

const std::string& Decoder::decodeRegisterSIB_index(uint8_t reg, InstructionInfo info)
{
    if (info.rexprefix & 0x02 && info.hasSIB)
    {
        reg += 8;
    }

    switch (reg)
    {
        case 0:
            return "RAX";
            break;
        case 1:
            return "RCX";
            break;
        case 2:
            return "RDX";
            break;
        case 3:
            return "RBX";
            break;
        case 4:
            return "RSP";
            break;
        case 5:
            return "RBP";
            break;
        case 6:
            return "RSI";
            break;
        case 7:
            return "RDI";
            break;
        case 8:
            return "R8";
            break;
        case 9:
            return "R9";
            break;
        case 10:
            return "R10";
            break;
        case 11:
            return "R11";
            break;
        case 12:
            return "R12";
            break;
        case 13:
            return "R13";
            break;
        case 14:
            return "R14";
            break;
        case 15:
            return "R15";
            break;
        default:
            return "Unknown register";
            break;
    }
}

SIB Decoder::decodeSIB(int8_t sib)
{
    SIB sibStruct;
    sibStruct.byte_sib = sib;
    sibStruct.base = sib & 0b111;
    sibStruct.index = (sib >> 3) & 0b111;
    sibStruct.scale = (sib >> 6) & 0b11;

    return sibStruct;
}