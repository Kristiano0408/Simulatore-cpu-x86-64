//for decodification of the instructions

// decoder.hpp
#ifndef DECODER_HPP
#define DECODER_HPP

#include <string>
#include "instruction.hpp"
#include <vector>
#include "helpers.hpp"






class Decoder {
    
    public:
        Decoder();
        ~Decoder();
        
        //decode the instruction
        InstructionInfo LenghtOfInstruction(uint32_t opcode, uint8_t prefix[4], int numPrefixes, bool rex, uint16_t rexprefix);
        void fixTotalLengthPrefix(InstructionInfo& info);
        Instruction* decodeInstruction(InstructionInfo instruction, CU* controlUnit);
         //decode the  r/m operand
        static r_m decodeRM(uint8_t r_m);
        //decode the SIB operand
        static SIB decodeSIB(uint8_t sib);

        static void decodeInstructionOI(Instruction* instruction, const InstructionInfo& instructionInfo, int position);
        static void decodeInstructionMI(Instruction* instruction, const InstructionInfo& instructionInfo, int position);
        static void decodeInstructionMR(Instruction* instruction, const InstructionInfo& instructionInfo, int position);
        static void decodeInstructionRM(Instruction* instruction, const InstructionInfo& instructionInfo, int position);
        static void decodeInstructionFD(Instruction* instruction, const InstructionInfo& instructionInfo, int position);
        static void decodeInstructionTD(Instruction* instruction, const InstructionInfo& instructionInfo, int position);
        


        
        

        
    private:

        //decode the instruction
        Instruction* ConstructorCreation(typeofInstruction type_instruction);
        
        Instruction* decodeAdd(InstructionInfo instruction);
        Instruction* decodeSub(InstructionInfo instruction);
        Instruction* decodeMul(InstructionInfo instruction);
        Instruction* decodeMov(InstructionInfo instruction, int position, CU* controlUnit);

        //decode immediate value
        static void decodeImmediateValue(InstructionInfo instructionInfo, Instruction* instruction, int position);
        //setting the parameters of the instruction
        void settingInstructionParameters(Instruction* instruction, InstructionInfo instructionInfo);
        //decode the displacement value
        static uint64_t decodeDisplacement(InstructionInfo instruction, int& position, int size);
        //decode the RM type instruction
        static void decode_RM_instruction(Instruction* instruction, InstructionInfo instructionInfo, int& position);


       


    
        //etc
};

#endif // DECODER_HPP
