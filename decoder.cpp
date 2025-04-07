#include "decoder.hpp"
#include <iostream>
#include <memory>
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
    info.rex = rex;
    info.rexprefix = rexprefix;

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

        std::cerr << "Unknown instruction: " << std::hex << opcode << std::endl;

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
           
    }
   

    //chaning the lenght of the instruction due to the prefix(only presence without considering the type of prefix)
    fixTotalLengthPrefix(info);

    return info;
    

   
}

void Decoder::fixTotalLengthPrefix(InstructionInfo& info)
{
    if (info.rex)
    {
        info.totalLength += 1;
    }

    for (int i = 0; i < info.prefixCount; i++)
    {
        if (info.prefix[i] != 0)
        {
            info.totalLength += 1;
        }
    }

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

    //getting the type and adressing mode of the instruction
    auto it = instructionMap.find(instruction.opcode)->second;

    typeofInstruction type_instruction = it.type;

    AddressingMode mode = it.mode;


    //creating the constructor of the instruction based on the type of instruction
    Instruction* inst = ConstructorCreation(type_instruction);

    

    //setting the instruction parameters like the opcode, the prefix, the rex, etc
    settingInstructionParameters(inst, instruction);


    //getting the corrisponding function for the addressing mode
    auto it2 = Addressing_modes.find(mode);

    

    if (it2 != Addressing_modes.end())
    {
        DecodeFunc decodeFunc = it2->second;
        //calling the function for decoding the instruction
        decodeFunc(inst, instruction, position);

        //setting the addressing mode of the instruction
        inst->setAddressingMode(mode);

        return inst;
    }
    else
    {
        std::cerr << "Unknown addressing mode" << std::endl;
        return nullptr;
    }




}

//methods for decoding the instructions based on the addressing mode
void Decoder::decodeInstructionOI(Instruction* instruction, const InstructionInfo& instructionInfo, int position)
{
    //decode the immediate value
    decodeImmediateValue(instructionInfo, instruction, position);
    instruction->setHasImmediate(true);

    
}

void Decoder::decodeInstructionMI(Instruction* instruction, const InstructionInfo& instructionInfo, int position)
{
    decode_RM_instruction(instruction, instructionInfo, position);
}

void Decoder::decodeInstructionMR(Instruction* instruction, const InstructionInfo& instructionInfo, int position)
{
    instruction->setRegToMem(true);
    decode_RM_instruction(instruction, instructionInfo, position);
}

void Decoder::decodeInstructionRM(Instruction* instruction, const InstructionInfo& instructionInfo, int position)
{
    instruction->setMemToReg(true);
    decode_RM_instruction(instruction, instructionInfo, position);
}

void Decoder::decodeInstructionFD(Instruction* instruction, const InstructionInfo& instructionInfo, int position)
{
    //decode 
    instruction->setHasDisplacement(true);
    instruction->setMemToReg(true);
    instruction->setDisplacement(decodeDisplacement(instructionInfo, position, 4));
}

void Decoder::decodeInstructionTD(Instruction* instruction, const InstructionInfo& instructionInfo, int position)
{
    //decode 
    instruction->setHasDisplacement(true);
    instruction->setRegToMem(true);
    instruction->setDisplacement(decodeDisplacement(instructionInfo, position, 4));
}


//constructor of the instruction based on the type of instruction
Instruction* Decoder::ConstructorCreation(typeofInstruction type_instruction)
{
    Instruction* instruction = nullptr;

    auto it = instructionConstructors.find(type_instruction);

    if (it != instructionConstructors.end())
    {
        ConstructorFunc constructorFunc = it->second;
        instruction = constructorFunc(); //create the instruction based on the type of instruction
    }
    else
    {
        std::cerr << "Unknown instruction type" << std::endl;
        return nullptr;
    }
    return instruction;
}
       



//helper methods for decoding the instruction

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

void Decoder::decodeImmediateValue(InstructionInfo instructionInfo, Instruction* instruction, int position)
{
    uint64_t value = 0;
    //decode the immediate value
    for (int i = 0; i < instructionInfo.operandLength; i++)
    {
        value |= static_cast<uint64_t>(instructionInfo.instruction[position + i]) << (i * 8);
    }

    //position += instructionInfo.operandLength;

    instruction->setValue(value);


    
}

void Decoder::settingInstructionParameters(Instruction* instruction, InstructionInfo instructionInfo)
{
    //setting the parameters of instruction
    instruction->setOpcode(instructionInfo.opcode);
    
    instruction->setPrefix(instructionInfo.prefix);

    instruction->setNumPrefixes(instructionInfo.prefixCount);

    instruction->setRex(instructionInfo.rex);

    instruction->setRexprefix(instructionInfo.rexprefix);
    
}

uint64_t Decoder::decodeDisplacement(InstructionInfo instruction, int& position, int size)
{
    uint64_t displacement = 0;
    for (int i = 0; i < size; i++)
    {
        displacement |= static_cast<uint64_t>(instruction.instruction[position + i]) << (i * 8);
    }

    position += size;

    return displacement;
}

void Decoder::decode_RM_instruction(Instruction* instruction, InstructionInfo instructionInfo, int& position)
{
    uint64_t displacement = 0;
    uint64_t value = 0;



    //decode the ModRM
    r_m rm = decodeRM(instructionInfo.instruction[position]);


    position++;
    instruction->setRM(rm);
    instruction->setHasModRM(true);

   //control of the varius cases of addressing mode
   if(rm.mod == 0b11)
    {   
        //the operand is a register
        instruction->setHasDisplacement(false);
        instruction->setHasSIB(false);
        instruction->setRegToReg(true);
        instruction->setMemToReg(false);
        instruction->setRegToMem(false);

    }
   
    else if (rm.mod == 0b00)
    {

        if(rm.r_m == 0b100)
        {
            //there is SIB
            instruction->setHasSIB(true);
            SIB sib = decodeSIB(instructionInfo.instruction[position]);
            position++;
            instruction->setSIB(sib);
            if(sib.base == 0b101)
            {
                //the operand is a displacement
                instruction->setHasDisplacement(true);
                //the displacement is 32 bit
                displacement = decodeDisplacement(instructionInfo, position, 4);
                //set the displacement
                instruction->setSIBdisplacement(static_cast<uint32_t>(displacement));
            }
   

        }
        else if (rm.r_m == 0b101)
        {
            //the operand is a displacement
            instruction->setHasDisplacement(true);
            //the displacement is 32 bit
            displacement = decodeDisplacement(instructionInfo, position, 4);
            //set the displacement
            instruction->setDisplacement(displacement);
        }
        else 
        {
           //the operand is a register/mem without displacement
        }
       
    }
   
    else if (rm.mod == 0b01)
    {
        //the operand is a register/mem with 8 bit displacement
        instruction->setHasDisplacement(true);
       
        if (rm.r_m == 0b100)
        {
           //there is SIB
           instruction->setHasSIB(true);
           SIB sib = decodeSIB(instructionInfo.instruction[position]);
           position++;
           instruction->setSIB(sib);

        }

        displacement = 0;
        displacement += instructionInfo.instruction[position];
        position++;
        instruction->setDisplacement(displacement);
       
    }

    else if (rm.mod == 0b10)
    {
        //the operand is a register/mem with 32 bit displacement
        instruction->setHasDisplacement(true);
        if (rm.r_m == 0b100)
        {
           //there is SIB
           instruction->setHasSIB(true);
           SIB sib = decodeSIB(instructionInfo.instruction[position]);
           position++;
           instruction->setSIB(sib);

        }

        displacement = 0;

        displacement = decodeDisplacement(instructionInfo, position, 4);

        instruction->setDisplacement(displacement);
    }

    if(instructionInfo.hasImmediate)
    {
        decodeImmediateValue(instructionInfo, instruction, position);
    }


   

}
