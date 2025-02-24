
#include <string>
#include "instruction.hpp"
#include "opcode_map.cpp"
#include "decoder.hpp"
#include <iostream>
#include "addressingMode.hpp"


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
        
        
       
       
    }

    if (rex and (rexprefix & 0x08))
    {
        //the lenght is increased by 4 bytes 
        info.totalLength += (4* info.numOperands);
        info.additionalBytes += (4* info.numOperands);
        info.operandLength += 4;
        info.rex = true;
           
    }

    return info;
    

   
}


Instruction* Decoder::decodeInstruction(InstructionInfo instruction)
{
    uint8_t prefix[4];
    int position = 0;

    //searching the prefix
    if (instruction.prefixCount > 0)
    {
        for (int i = 0; i < instruction.prefixCount; i++)
        {
            prefix[i] = instruction.istruction[i];
        }
    }

    //method b
    //memcpy(prefix, instruction.istruction, instruction.prefixCount);

    setPrefix(prefix);
    setNumPrefixes(instruction.prefixCount);

    position = instruction.prefixCount;


    //searching the rex prefix
    if (instruction.rex)
    {
        rex = true;
        setRexprefix(instruction.istruction[position]);
        position++;
    }

    //searching the opcode
    setOpcode(instruction.opcode);

    if (getOpcode() >= 0xB8 && getOpcode() <= 0xBF)
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



    //if there is immediate operand
    if(getOpcode() >= 0xB8 && getOpcode() <= 0xBF)
    {
        //getting the dimension of operands
        dimOperands = instruction.operandLength;

        //setting the value of the operands
        for (int i = 0; i < dimOperands; i++)
        {
            value += instruction.istruction[position + i] << (8 * i);
        }

        //casting the value
        if (!rex)
        {
            value= static_cast<int32_t>(value);

            
            for (int i = 0; i < numPrefixes; i++)
            {
                if (prefix[i] == 0x66)
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