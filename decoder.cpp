
#include <string>
#include "instruction.hpp"
#include "opcode_map.cpp"
#include "decoder.hpp"
#include <iostream>
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
    }
    else
    {
        //if the opcode is not found
        info.totalLength = 0;
        info.opcodeLength = 0;
        info.additionalBytes = 0;
        info.numOperands = 0;
        info.operandLength = 0;
        info.description = "Unknown instruction";
        return info;
    }

    for (int i = 0; i < numPrefixes; i++)
    {
        //if there is the prefix for 16bits opernads
        if(prefix[i] == 0x66)
        {
            //the lenght is reduced by 4 bytes 
            info.totalLength -= (2* info.numOperands);
            info.additionalBytes -= (2* info.numOperands);
            info.operandLength -= 2;
            
            break;
        }

        info.prefix[i] = prefix[i];
        
    }

    if (rex and (rexprefix & 0x08))
    {
        //the lenght is increased by 4 bytes 
        info.totalLength += (4* info.numOperands);
        info.additionalBytes += (4* info.numOperands);
        info.operandLength += 4;
        info.rex = true;
        info.rexprefix = rexprefix;
           
    }


    return info;
    

   
}


Instruction* Decoder::decodeInstruction(InstructionInfo instruction)
{
    uint8_t prefix[4];
    int position = 0;

    
    position = instruction.prefixCount;


    //searching the rex prefix
    if (instruction.rex)
    {
        position++;
    }

    position += instruction.opcodeLength;

    

    if (instruction.opcode >= 0xB8 && instruction.opcode <= 0xBF)
    {
        return decodeMov(instruction, position);
    }


    return nullptr;
   
    






}
       
Instruction* Decoder::decodeMov(InstructionInfo instruction, int position)
{
    //create the instruction
    MoveInstruction* mov = new MoveInstruction();
    int dimOperands = 0;
    int64_t value = 0;

    //setting the parameters of instruction
    mov->setOpcode(instruction.opcode);
    mov->setPrefix(instruction.prefix);
    mov->setNumPrefixes(instruction.prefixCount);
    mov->setRex(instruction.rex);
    mov->setRexprefix(instruction.rexprefix);



    //if there is immediate operand
    if(instruction.opcode >= 0xB8 && instruction.opcode <= 0xBF)
    {
        //getting the dimension of operands
        dimOperands = instruction.operandLength;

        //setting the value of the operands
        for (int i = 0; i < dimOperands; i++)
        {
            value += instruction.istruction[position + i] << (8 * i);
        }

        //casting the value
        if (!instruction.rex)
        {
            value= static_cast<int32_t>(value);

            
            for (int i = 0; i < instruction.prefixCount; i++)
            {
                if (instruction.prefix[i] == 0x66)
                {
                    value = static_cast<int16_t>(value);
                    break;
                }
            }
        }

        mov->setValue(value);
    
    }

    return mov;


    

}