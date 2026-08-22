//for decodification of the instructions

// decoder.hpp
#ifndef DECODER_HPP
#define DECODER_HPP

#include "instruction.hpp"
#include "device.hpp"
#include <cstddef>




class Decoder : public FaultDevice
{
    
    public:
        Decoder() = default;
        ~Decoder() = default;
        
        //decode the instruction
        InstructionInfo lenghtOfInstruction(uint32_t opcode, const uint8_t prefix[4], uint8_t numPrefixes, bool rex, uint8_t rexprefix);
        static void fixTotalLengthPrefix(InstructionInfo& info);
        std::unique_ptr<Instruction> decodeInstruction(InstructionInfo instruction);
         //decode the  r/m operand
        static r_m decodeRM(std::byte r_m);
        //decode the SIB operand
        static SIB decodeSIB(std::byte sib);

        static void decodeInstructionOI(Instruction* instruction, const InstructionInfo& instructionInfo, uint8_t position);
        static void decodeInstructionMI(Instruction* instruction, const InstructionInfo& instructionInfo, uint8_t position);
        static void decodeInstructionMR(Instruction* instruction, const InstructionInfo& instructionInfo, uint8_t position);
        static void decodeInstructionRM(Instruction* instruction, const InstructionInfo& instructionInfo, uint8_t position);
        static void decodeInstructionFD(Instruction* instruction, const InstructionInfo& instructionInfo, uint8_t position);
        static void decodeInstructionTD(Instruction* instruction, const InstructionInfo& instructionInfo, uint8_t position);
        static void decodeInstructionI(Instruction* instruction, const InstructionInfo& instructionInfo, uint8_t position);


        
    private:

        //decode immediate value
        static void decodeImmediateValue(InstructionInfo instructionInfo, Instruction* instruction, uint8_t position);
        //setting the parameters of the instruction
        static void settingInstructionParameters(Instruction* instruction, InstructionInfo instructionInfo);
        //decode the displacement value
        static uint64_t decodeDisplacement(InstructionInfo instruction, uint8_t& position, uint8_t size);
        //decode the RM type instruction
        static void decodeRmInstruction(Instruction* instruction, InstructionInfo instructionInfo, uint8_t& position);


       


    
        //etc
};

#endif // DECODER_HPP
