#include "decoder.hpp"
#include <iostream>
#include <memory>
//#include "addressingMode.hpp"
#include "opcode_map.cpp"

Decoder::Decoder()
{

    //nothing to do here
}

Decoder::~Decoder()
{
    //nothing to do here
}




InstructionInfo Decoder::LenghtOfInstruction(uint32_t opcode, uint8_t prefix[4],int numPrefixes, bool rex, uint16_t rexprefix)
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

    //setting the prefix and changing the lenght of the instruction if there is IO or offset
    for (int i = 0; i < numPrefixes; i++)
    {
        //if there is the prefix for 16bits opernads
        if(prefix[i] == 0x66 and (info.hasImmediate))
        {
            //the lenght is reduced by 2 bytes
            info.totalLength -= 2;
            info.additionalBytes -= 2;
            info.operandLength -= 2;
            info.prefix[i] = prefix[i];
            
            break;
        }

        info.prefix[i] = prefix[i];
        
    }

    if (rex and (rexprefix & 0x08) and (info.hasImmediate or info.hasDisplacement))
    {
        //the lenght is increased by 4 bytes 
        info.totalLength += 4;
        info.additionalBytes += 4;
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
    Instruction* mov = new MoveInstruction();


    uint64_t value = 0;
    uint64_t displacement = 0;



    //setting the parameters of instruction
    mov->setOpcode(instruction.opcode);
    
    mov->setPrefix(instruction.prefix);

    mov->setNumPrefixes(instruction.prefixCount);

    mov->setRex(instruction.rex);

    
    //if there is a a0-a3 opcode
    if (instruction.hasDisplacement)
    {
        mov->setHasDisplacement(true);

        if(isMoveInstructionOffsetRAX_mem(instruction.opcode))
        {
            mov->setRegToMem(true);
        }
        else if(isMoveInstructionOffsetMem_RAx(instruction.opcode))
        {
            mov->setMemToReg(true);
        }

        displacement = 0;


        for (int i = 0; i < instruction.operandLength; i++)
        {   
            std::cout << "Byte: " << std::hex << static_cast<int>(instruction.instruction[position + i]) << std::endl;
            displacement |= static_cast<uint64_t>(instruction.instruction[position + i]) << (i * 8);
        }

        std::cout << "Displacement: " << displacement << std::endl;

        position += instruction.operandLength;

        mov->setDisplacement(displacement);
    }




    //if there is immediate value and no ModRM
    if (instruction.hasImmediate && !instruction.hasModRM)
    {
        value = 0;
        //decode the immediate value
        for (int i = 0; i < instruction.operandLength; i++) 
            value |= static_cast<uint64_t>(instruction.instruction[position + i]) << (i * 8);

        position += instruction.operandLength;

        mov->setHasImmediate(true);
        mov->setValue(value);
        
    }
    
    else if (instruction.hasModRM)
    {
        //decode the ModRM
        r_m rm = decodeRM(instruction.instruction[position]);
        position++;
        mov->setRM(rm);
        mov->setHasModRM(true);
 
        if(isMoveR_M_mem_reg(instruction.opcode)) mov->setMemToReg(true);
       
        else if(isMoveR_M_reg_mem(instruction.opcode)) mov->setRegToMem(true);


        //control of the varius cases of addressing mode
        if(rm.mod == 0b11)
        {   
            //the operands are registers

            if (!isMoveInstructionIO_mem(instruction.opcode))
            {
                mov->setRegToReg(true);
                mov->setRegToMem(false);
                mov->setMemToReg(false);

            }
            else 
            {
                //nothing to do because is immmediate operand with destination register
            }
        

        }
        
        else if (rm.mod == 0b00)
        {
            if(rm.r_m == 0b100)
            {
                //there is SIB
                mov->setHasSIB(true);
                SIB sib = decodeSIB(instruction.instruction[position]);
                position++;
                mov->setSIB(sib);
                if(sib.base == 0b101)
                {
                    //the operand is a displacement
                    mov->setHasDisplacement(true);

                    displacement = 0;

                    for (int i = 0; i < 4; i++)
                    {
                        displacement |= instruction.instruction[position + i] << (i * 8);
                    
                    }
                    position += 4;

                    mov->setSIBdisplacement(static_cast<uint32_t>(displacement));
                }
        

            }
            else if (rm.r_m == 0b101)
            {
                //the operand is a displacement
                mov->setHasDisplacement(true);
                for (int i = 0; i < 4; i++)
                {
                    displacement += instruction.instruction[position + i] << (i * 8);
                    position++;
                }

                mov->setDisplacement(displacement);
            }
            else 
            {
                //the operand is a register/mem without displacement
            }
            
        }
        
        else if (rm.mod == 0b01)
        {
            //the operand is a register/mem with 8 bit displacement
            mov->setHasDisplacement(true);
            
            if (rm.r_m == 0b100)
            {
                //there is SIB
                mov->setHasSIB(true);
                SIB sib = decodeSIB(instruction.instruction[position]);
                position++;
                mov->setSIB(sib);

            }

            displacement = 0;
            displacement += instruction.instruction[position];
            mov->setDisplacement(displacement);
            position++;

        }

        else if (rm.mod == 0b10)
        {
            //the operand is a register/mem with 32 bit displacement
            mov->setHasDisplacement(true);
            if (rm.r_m == 0b100)
            {
                //there is SIB
                mov->setHasSIB(true);
                SIB sib = decodeSIB(instruction.instruction[position]);
                position++;
                mov->setSIB(sib);

            }

            displacement = 0;

            for (int i = 0; i < 4; i++)
            {
                std::cout << "Byte: " << std::hex<<static_cast<int>(instruction.instruction[position + i]) << std::endl;
                displacement |= static_cast<uint32_t>(instruction.instruction[position + i]) << (i * 8);
            }

            position += 4;

            mov->setDisplacement(displacement);
            std::cout <<std::hex<< "Displacement: " << displacement << std::endl;
        }

        if(instruction.hasImmediate)
        {
            value = 0;
            //decode the immediate value
            for (int i = 0; i < instruction.operandLength; i++)
            {
                value |= instruction.instruction[position + i] << (i * 8);
            }

            position += instruction.operandLength;

            mov->setHasImmediate(true);
            mov->setValue(value);
        }
        

    }

    return mov;
}



SIB Decoder::decodeSIB(uint8_t sib)
{
    SIB sibStruct;
    sibStruct.byte_sib = sib;
    sibStruct.base = sib & 0b111;
    sibStruct.index = (sib >> 3) & 0b111;
    sibStruct.scale = (sib >> 6) & 0b11;

    return sibStruct;
}

r_m Decoder::decodeRM(uint8_t R_M)
{
    r_m rm;
    rm.byte_r_m = R_M;
    rm.mod = (R_M >> 6) & 0b11;
    rm.reg = (R_M >> 3) & 0b111;
    rm.r_m = (R_M & 0b111);

    return rm;
}