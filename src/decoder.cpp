#include "decoder.hpp"
#include <iostream>
#include <memory>
#include "opcode_map.hpp"

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
        info.src_operand_length = it->second.src_operand_length;
        info.dest_operand_length = it->second.dest_operand_length;
        info.bit_extension = it->second.bit_extension;
        info.rex_w_sensitive = it->second.rex_w_sensitive;

        
    }
    else
    {
        //if the opcode is not found
        info.description = "Unknown instruction";

        std::cerr << "Unknown instruction: " << std::hex << opcode << std::endl;

        return info;
    }

    //setting the prefix and changing the lenght of the instruction if there is IO or offset
    for (int i = 0; i < numPrefixes; i++)
    {
        //if there is the prefix for 16bits opernads
        if(prefix[i] == LegacyPrefixMask::OPERAND_SIZE_OVERRIDE and (info.hasImmediate))
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

    if (rex and (rexprefix & RexMask::REX_W) and (info.hasImmediate or info.hasDisplacement))
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

    for (size_t i = 0; i < info.prefixCount; i++)
    {
        if (info.prefix[i] != 0)
        {
            info.totalLength += 1;
        }
    }

}



std::unique_ptr<Instruction> Decoder::decodeInstruction(InstructionInfo instruction)
{
    //creating the constructor of the instruction based on the type of instruction
    std::unique_ptr<Instruction> instructionPtr = std::make_unique<Instruction>();

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

    DEBUG_LOG(debugLog("Decoding instruction: " + to_string_hex(instruction.opcode) + " - " + std::to_string(static_cast<int>(it.mode))));

    TypeofInstruction type_instruction = it.type;

    DEBUG_LOG(debugLog("Instruction type: " + toStringTypeofInstruction(type_instruction)));

    AddressingMode mode = it.mode;


    
    InstructionCore& core = instructionPtr->getCore();

    core.type = type_instruction;

    //setting the instruction parameters like the opcode, the prefix, the rex, etc
    settingInstructionParameters(instructionPtr.get(), instruction);


    //getting the corrisponding function for the addressing mode
    auto it2 = Addressing_modes.find(mode);

    

    if (it2 != Addressing_modes.end())
    {
        DecodeFunc decodeFunc = it2->second;
        //calling the function for decoding the instruction
        decodeFunc(instructionPtr.get(), instruction, position);

        //setting the addressing mode of the instruction
        core.addressingMode = mode;

        return instructionPtr;
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
    InstructionFlags& flags = instruction->getFlags();
    flags.hasImmediate = 1;
    flags.regToReg = 1; //immediate to register (the flag is reused implicitly for this purpose for simplicity)
    
}

void Decoder::decodeInstructionMI(Instruction* instruction, const InstructionInfo& instructionInfo, int position)
{
    decode_RM_instruction(instruction, instructionInfo, position);
}

void Decoder::decodeInstructionMR(Instruction* instruction, const InstructionInfo& instructionInfo, int position)
{
    InstructionFlags& flags = instruction->getFlags();
    flags.regToMem = 1;
    decode_RM_instruction(instruction, instructionInfo, position);
}

void Decoder::decodeInstructionRM(Instruction* instruction, const InstructionInfo& instructionInfo, int position)
{
    InstructionFlags& flags = instruction->getFlags();
    flags.memToReg = 1;
    decode_RM_instruction(instruction, instructionInfo, position);
}

void Decoder::decodeInstructionFD(Instruction* instruction, const InstructionInfo& instructionInfo, int position)
{
    //decode 
    InstructionFlags& flags = instruction->getFlags();
    InstructionCore& core = instruction->getCore();
    flags.hasDisplacement = 1;
    flags.memToReg = 1;
    core.displacement = decodeDisplacement(instructionInfo, position, 4);
}

void Decoder::decodeInstructionTD(Instruction* instruction, const InstructionInfo& instructionInfo, int position)
{
    //decode 
    InstructionFlags& flags = instruction->getFlags();
    InstructionCore& core = instruction->getCore();
    flags.hasDisplacement = 1;
    flags.regToMem = 1;
    core.displacement = decodeDisplacement(instructionInfo, position, 4);
}

void Decoder::decodeInstructionI(Instruction* instruction, const InstructionInfo& instructionInfo, int position)
{
    InstructionFlags& flags = instruction->getFlags();
    flags.hasImmediate = 1;
    flags.regToReg = 1;  //immediate to register (the flag is reused implicitly for this purpose for simplicity)
    decodeImmediateValue(instructionInfo, instruction, position);
    
}





//helper methods for decoding the instruction

SIB Decoder::decodeSIB(uint8_t sib)
{
    SIB sibStruct;
    sibStruct.byte_sib = sib;
    sibStruct.base =SIBMask::extractBase(sib);
    sibStruct.index = SIBMask::extractIndex(sib);
    sibStruct.scale = SIBMask::extractScale(sib);

    return sibStruct;
}

r_m Decoder::decodeRM(uint8_t R_M)
{
    r_m rm;
    rm.byte_r_m = R_M;
    rm.mod = ModRMMask::extractMod(R_M);
    rm.reg = ModRMMask::extractReg(R_M);
    rm.r_m = ModRMMask::extractRM(R_M);

    return rm;
}

void Decoder::decodeImmediateValue(InstructionInfo instructionInfo, Instruction* instruction, int position)
{
    uint64_t value = 0;
    //decode the immediate value
    for (size_t i = 0; i < instructionInfo.operandLength; i++)
    {
        value |= static_cast<uint64_t>(instructionInfo.instruction[position + i]) << (i * 8);
    }

    //position += instructionInfo.operandLength;
    InstructionCore& core = instruction->getCore();
    core.value = value;


    
}

void Decoder::settingInstructionParameters(Instruction* instruction, InstructionInfo instructionInfo)
{
    //setting the parameters of instruction
    InstructionCore& core = instruction->getCore();
    core.opcode = instructionInfo.opcode;
    core.prefix = instructionInfo.prefix;
    core.numPrefixes = instructionInfo.prefixCount;
    core.rexprefix = instructionInfo.rexprefix;
    core.InstructionId = instructionInfo.instructionId;

    InstructionFlags& flags = instruction->getFlags();
    flags.rex = instructionInfo.rex;
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
    InstructionCore& core = instruction->getCore();
    InstructionFlags& flags = instruction->getFlags();
    uint64_t displacement = 0;

    //decode the ModRM
    r_m rm = decodeRM(instructionInfo.instruction[position]);

    position++;

    core.rm = rm;
    flags.hasModRM = 1;
   //control of the varius cases of addressing mode
    if(rm.mod == 0b11)
    {   
        //the operand is a register
        flags.hasDisplacement = 0;
        flags.hasSIB = 0;
        flags.regToReg = 1;
        flags.memToReg = 0;
        flags.regToMem = 0;
    }
    else if (rm.mod == 0b00)
    {

        if(rm.r_m == 0b100)
        {
            //there is SIB
            flags.hasSIB = 1;
            SIB sib = decodeSIB(instructionInfo.instruction[position]);
            position++;
            core.sib = sib;
            if(sib.base == 0b101)
            {
                //the operand is a displacement
                flags.hasDisplacement = 1;
                //the displacement is 32 bit
                displacement = decodeDisplacement(instructionInfo, position, 4);
                //set the displacement
                core.SIBdisplacement = static_cast<uint32_t>(displacement);
            }
   

        }
        else if (rm.r_m == 0b101)
        {
            //the operand is a displacement
            flags.hasDisplacement = 1;
            //the displacement is 32 bit
            displacement = decodeDisplacement(instructionInfo, position, 4);
            //set the displacement
            core.displacement = displacement;
        }
        else 
        {
           //the operand is a register/mem without displacement
        }
       
    }
   
    else if (rm.mod == 0b01)
    {
        //the operand is a register/mem with 8 bit displacement
        flags.hasDisplacement = 1;
       
        if (rm.r_m == 0b100)
        {
           //there is SIB
           flags.hasSIB = 1;
           SIB sib = decodeSIB(instructionInfo.instruction[position]);
           position++;
           core.sib = sib;

        }

        displacement = 0;
        displacement += instructionInfo.instruction[position];
        position++;
        core.displacement = displacement;
       
    }

    else if (rm.mod == 0b10)
    {
        //the operand is a register/mem with 32 bit displacement
        flags.hasDisplacement = 1;
        if (rm.r_m == 0b100)
        {
           //there is SIB
           flags.hasSIB = 1;
           SIB sib = decodeSIB(instructionInfo.instruction[position]);
           position++;
           core.sib = sib;

        }

        displacement = 0;

        displacement = decodeDisplacement(instructionInfo, position, 4);

        core.displacement = displacement;
    }

    if(instructionInfo.hasImmediate)
    {
        decodeImmediateValue(instructionInfo, instruction, position);
    }


   

}
