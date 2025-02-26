
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
    else
    {
        info.rex = false;
        info.rexprefix = 0;
    }


    return info;
    

   
}


Instruction* Decoder::decodeInstruction(InstructionInfo instruction, CU* controlUnit)
{
    uint8_t prefix[4];
    int position = 0;

    
    position = instruction.prefixCount;

    std::cout << "Position: " << position << std::endl;


    //searching the rex prefix
    if (instruction.rex)
    {
        position++;
    }

    position += instruction.opcodeLength;

    std::cout <<instruction.opcodeLength << std::endl;

    std::cout << "Position: " << position << std::endl;

    

    if (instruction.opcode >= 0xB8 && instruction.opcode <= 0xBF)
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
    int dimOperands = 0;
    int64_t value = 0;

    //setting the parameters of instruction
    mov->setOpcode(instruction.opcode);
    std::cout << "Opcode: " << mov->getOpcode() << std::endl;
    mov->setPrefix(instruction.prefix);
    std::cout << "Prefix: " << mov->getPrefix() << std::endl;
    mov->setNumPrefixes(instruction.prefixCount);
    std::cout << "Num Prefixes: " << mov->getNumPrefixes() << std::endl;
    mov->setRex(instruction.rex);
    std::cout << "Rex: " << mov->getRex() << std::endl;
    mov->setRexprefix(instruction.rexprefix);
    std::cout << "Rex Prefix: " << mov->getRexprefix() << std::endl;



    //if there is immediate operand
    if(instruction.opcode >= 0xB8 && instruction.opcode <= 0xBF)
    {

        //craeting the object of addressing mode


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
        dimOperands = instruction.operandLength;
        std::cout << "Dim Operands: " << dimOperands << std::endl;

        std::cout << "Position: " << position << std::endl;
    

        //setting the value of the operands
        for (int i = 0; i < dimOperands; i++)
        {
            value |= (static_cast<int64_t> (instruction.istruction[position + i]))<< (8 * i);
            std::cout <<  std::hex <<"Value: " << value << std::endl;
         
        }

        //setting the value with the addressing mode (for immediate operand it actually does nothing, but is for using the sme methodology for all the adressing mode)
        value = addressingMode->immediateAddressing(value);

        
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

    return mov;


    

}