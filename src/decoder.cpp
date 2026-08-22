#include "decoder.hpp"
#include <cstddef>
#include <iostream>
#include <memory>
#include "opcode_map.hpp"



InstructionInfo Decoder::lenghtOfInstruction(uint32_t opcode,const uint8_t prefix[4],uint8_t numPrefixes, bool rex, uint8_t rexprefix)
{
    InstructionInfo info = {};
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
        info.additionalBytes = uint8_t(it->second.additionalBytes); //additional bytes standars for 32 bits
        info.description = it->second.description;
        info.numOperands = uint8_t(it->second.numOperands);
        info.operandLength = uint8_t(it->second.operandLength);
        info.hasModRM = it->second.hasModRM;
        info.hasDisplacement = it->second.hasDisplacement;
        info.hasImmediate = it->second.hasImmediate;
        info.src_operand_length = uint8_t(it->second.src_operand_length);
        info.dest_operand_length = uint8_t(it->second.dest_operand_length);
        info.bit_extension = uint8_t(it->second.bit_extension);
        info.rex_w_sensitive = it->second.rex_w_sensitive;

        
    }
    else
    {
        //if the opcode is not found
        info.description = "Unknown instruction";

        std::cerr << "Unknown instruction: " << std::hex << opcode << '\n';

        return info;
    }

    //setting the prefix and changing the lenght of the instruction if there is IO or offset
    for (uint8_t i = 0; i < numPrefixes; i++)
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

    if (rex and ((rexprefix & RexMask::REX_W) != 0) and (info.hasImmediate or info.hasDisplacement))
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

    uint8_t position = 0;

    position = instruction.prefixCount;


    //searching the rex prefix
    if (instruction.rex)
    {
        position++;
    }

    position += instruction.opcodeLength;

    //getting the type and adressing mode of the instruction
    
    auto mapIt = instructionMap.find(instruction.opcode);
    if (mapIt == instructionMap.end())
    {
        std::cerr << "Unknown instruction in instructionMap: " << std::hex << instruction.opcode << '\n';
        return nullptr;
    }
    auto& it = mapIt->second;

    DEBUG_LOG(debugLog("Decoding instruction: " + to_string_hex(instruction.opcode) + " - " + std::to_string(static_cast<int>(it.mode))));

    TypeofInstruction type_instruction = it.type;

    DEBUG_LOG(debugLog("Instruction type: " + toStringTypeofInstruction(type_instruction)));

    AddressingMode mode = it.mode;

    InstructionExecutionMode executionMode = it.executionMode;
    
    InstructionCore& core = instructionPtr->getCore();

    core.type = type_instruction;

    core.executionMode = executionMode;

    //setting the instruction parameters like the opcode, the prefix, the rex, etc
    settingInstructionParameters(instructionPtr.get(), instruction);


    //getting the corrisponding function for the addressing mode
    auto decoderFunction = addressingModes[static_cast<size_t>(mode)];
    //auto it2 = Addressing_modes.find(mode);

    if(decoderFunction != nullptr)
    {
        decoderFunction(instructionPtr.get(), instruction, position);

        core.addressingMode = mode;

        return instructionPtr;

    }
    
            DEBUG_LOG(debugLog("Unknown addressing mode"));
        return nullptr;
   




}

//methods for decoding the instructions based on the addressing mode
void Decoder::decodeInstructionOI(Instruction* instruction, const InstructionInfo& instructionInfo, uint8_t position)
{
    //decode the immediate value
    decodeImmediateValue(instructionInfo, instruction, position);
    InstructionFlags& flags = instruction->getFlags();
    flags.hasImmediate = 1;
    flags.regToReg = 1; //immediate to register (the flag is reused implicitly for this purpose for simplicity)
    
}

void Decoder::decodeInstructionMI(Instruction* instruction, const InstructionInfo& instructionInfo, uint8_t position)
{
    decodeRmInstruction(instruction, instructionInfo, position);
}

void Decoder::decodeInstructionMR(Instruction* instruction, const InstructionInfo& instructionInfo, uint8_t position)
{
    InstructionFlags& flags = instruction->getFlags();
    flags.regToMem = 1;
    decodeRmInstruction(instruction, instructionInfo, position);
}

void Decoder::decodeInstructionRM(Instruction* instruction, const InstructionInfo& instructionInfo, uint8_t position)
{
    InstructionFlags& flags = instruction->getFlags();
    flags.memToReg = 1;
    decodeRmInstruction(instruction, instructionInfo, position);
}

void Decoder::decodeInstructionFD(Instruction* instruction, const InstructionInfo& instructionInfo, uint8_t position)
{
    //decode 
    InstructionFlags& flags = instruction->getFlags();
    InstructionCore& core = instruction->getCore();
    flags.hasDisplacement = 1;
    flags.memToReg = 1;
    core.displacement = decodeDisplacement(instructionInfo, position, 4);
}

void Decoder::decodeInstructionTD(Instruction* instruction, const InstructionInfo& instructionInfo, uint8_t position)
{
    //decode 
    InstructionFlags& flags = instruction->getFlags();
    InstructionCore& core = instruction->getCore();
    flags.hasDisplacement = 1;
    flags.regToMem = 1;
    core.displacement = decodeDisplacement(instructionInfo, position, 4);
}

void Decoder::decodeInstructionI(Instruction* instruction, const InstructionInfo& instructionInfo, uint8_t position)
{
    InstructionFlags& flags = instruction->getFlags();
    flags.hasImmediate = 1;
    flags.regToReg = 1;  //immediate to register (the flag is reused implicitly for this purpose for simplicity)
    decodeImmediateValue(instructionInfo, instruction, position);
    
}





//helper methods for decoding the instruction

SIB Decoder::decodeSIB(std::byte sib)
{
    SIB sibStruct;
    sibStruct.byte_sib = sib;
    sibStruct.base =SIBMask::extractBase(sib);
    sibStruct.index = SIBMask::extractIndex(sib);
    sibStruct.scale = SIBMask::extractScale(sib);

    return sibStruct;
}

r_m Decoder::decodeRM(std::byte R_M)
{
    r_m rm;
    rm.byte_r_m = R_M;
    rm.mod = ModRMMask::extractMod(R_M);
    rm.reg = ModRMMask::extractReg(R_M);
    rm.r_m = ModRMMask::extractRM(R_M);

    return rm;
}

void Decoder::decodeImmediateValue(InstructionInfo instructionInfo, Instruction* instruction, uint8_t position)
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

uint64_t Decoder::decodeDisplacement(InstructionInfo instruction, uint8_t& position, uint8_t size)
{
    uint64_t displacement = 0;
    for (uint8_t i = 0; i < size; i++)
    {
        displacement |= static_cast<uint64_t>(instruction.instruction[position + i]) << (i * 8);
    }

    position += size;

    return displacement;
}


/// MACROS FOR SIMPLIFYING THE CODE AND AVOIDING REPETITION (do-while(0) is used to ensure the macro behaves like a single statement)
#define DECODE_SIB_IF_NEEDED() \
    do { \
        flags.hasSIB = 1; \
        SIB sib = decodeSIB(instructionInfo.instruction[position]); \
        position++; \
        core.sib = sib; \
    } while (0)

#define DECODE_DISPLACEMENT_IF_NEEDED(size) \
    do { \
        flags.hasDisplacement = 1; \
        displacement = decodeDisplacement(instructionInfo, position, size); \
        core.displacement = displacement; \
    } while (0)

#define DECODE_SIB_DISPLACEMENT_IF_NEEDED(size) \
    do { \
        flags.hasDisplacement = 1; \
        displacement = decodeDisplacement(instructionInfo, position, size); \
        core.SIBdisplacement = static_cast<uint32_t>(displacement); \
    } while (0)


//////////////////////////////////////////////////////////////////////////////////////////////////////

void Decoder::decodeRmInstruction(Instruction* instruction, InstructionInfo instructionInfo, uint8_t& position)
{
    InstructionCore& core = instruction->getCore();
    InstructionFlags& flags = instruction->getFlags();
    uint64_t displacement = 0;

    //decode the ModRM
    r_m rm = decodeRM(instructionInfo.instruction[position]);

    position++;

    core.rm = rm;
    flags.hasModRM = 1;

    switch (rm.mod)
    {
        case 0b11:
            flags.hasDisplacement = 0;
            flags.hasSIB = 0;
            flags.regToReg = 1;
            flags.memToReg = 0;
            flags.regToMem = 0;
            break;

        case 0b00:
            if (rm.r_m == 0b100) 
            {
                DECODE_SIB_IF_NEEDED();

                if (core.sib.base == 0b101) 
                    DECODE_SIB_DISPLACEMENT_IF_NEEDED(4);
            } 
            else if (rm.r_m == 0b101) 
                DECODE_DISPLACEMENT_IF_NEEDED(4);
        
            break;
        
        case 0b01:
            //the operand is a register/mem with 8 bit displacement
            if (rm.r_m == 0b100)
                DECODE_SIB_IF_NEEDED();

            DECODE_DISPLACEMENT_IF_NEEDED(1);

            break;

        case 0b10:
            //the operand is a register/mem with 32 bit displacement
            if (rm.r_m == 0b100)
                DECODE_SIB_IF_NEEDED();

            DECODE_DISPLACEMENT_IF_NEEDED(4);

            break;

        
        default:
            break;
    

    }
   

    if(instructionInfo.hasImmediate)
    {
        decodeImmediateValue(instructionInfo, instruction, position);
    }


   

}
