//for decodification of the instructions

// decoder.hpp
#ifndef DECODER_HPP
#define DECODER_HPP

#include <string>
#include "instruction.hpp"
#include <vector>
#include "helpers.hpp"

struct InstructionInfo {
    size_t totalLength;  // Lunghezza totale dell'istruzione
    size_t opcodeLength; // Lunghezza dell'opcode (1, 2 o 3 byte)
    size_t prefixCount;  // Numero di prefissi
    uint8_t prefix[4];   // Prefissi
    bool rex;            // Flag REX
    uint16_t rexprefix;   // Prefisso REX
    uint32_t opcode;     // Opcode completo (1, 2 o 3 byte)
    size_t additionalBytes; // Byte aggiuntivi (ModR/M, SIB, displacement, immediate)
    size_t numOperands; // Numero di operandi
    size_t operandLength; // Lunghezza degli operandi
    bool hasModRM;            // Presenza del byte ModR/M
    bool hasSIB;              // Presenza del byte SIB
    bool hasDisplacement;     // Presenza di un displacement
    bool hasImmediate;        // Presenza di un valore immediato
    std::vector<uint8_t> instruction;
    const char* description; // Descrizione dell'istruzione (opzionale)

};




class Decoder {
    
    public:
        Decoder();
        ~Decoder();
        
        //decode the instruction
        InstructionInfo LenghtOfInstruction(uint32_t opcode, uint8_t prefix[4], int numPrefixes, bool rex, uint16_t rexprefix);
        Instruction* decodeInstruction(InstructionInfo instruction, CU* controlUnit);
         //decode the  r/m operand
        r_m decodeRM(uint8_t r_m);
        //decode the SIB operand
        SIB decodeSIB(uint8_t sib);
        //decode immediate value
        void decodeImmediateValue(InstructionInfo instructionInfo, Instruction* instruction, int position);
        void settingInstructionParameters(Instruction* instruction, InstructionInfo instructionInfo);
        uint64_t decodeDisplacement(InstructionInfo instruction, int& position, int size);


        
        

        
    private:

        //decode the instruction
        Instruction* decodeAdd(InstructionInfo instruction);
        Instruction* decodeSub(InstructionInfo instruction);
        Instruction* decodeMul(InstructionInfo instruction);
        Instruction* decodeMov(InstructionInfo instruction, int position, CU* controlUnit);

       


    
        //etc
};

#endif // DECODER_HPP
