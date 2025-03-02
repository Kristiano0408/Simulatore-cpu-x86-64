//for decodification of the instructions

// decoder.hpp
#ifndef DECODER_HPP
#define DECODER_HPP

#include <string>
#include "instruction.hpp"
#include <vector>
#include "addressingMode.hpp"

//struct for simplifign decoding after getting some information out of fetchinstruction
struct InstructionInfo {
    size_t totalLength;  // Lunghezza totale dell'istruzione
    size_t opcodeLength; // Lunghezza dell'opcode (1, 2 o 3 byte)
    size_t prefixCount;  // Numero di prefissi
    uint8_t prefix[4];   // Prefissi
    bool rex;            // Flag REX
    int16_t rexprefix;   // Prefisso REX
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

struct r_m {
    uint8_t r_m : 3;
    uint8_t mod : 2;
    uint8_t reg : 3;
    uint8_t byte_r_m;
};

struct SIB {
    uint8_t base : 3;
    uint8_t index : 3;
    uint8_t scale : 2;
    uint8_t byte_sib;
};



class Decoder {
    
    public:
        Decoder();
        ~Decoder();
        
        //decode the instruction
        InstructionInfo LenghtOfInstruction(int32_t opcode, uint8_t prefix[4], int numPrefixes, bool rex, int16_t rexprefix);
        Instruction* decodeInstruction(InstructionInfo instruction, CU* controlUnit);
         //decode the  r/m operand
        r_m decodeRM(int8_t r_m);
        //decode the SIB operand
        SIB decodeSIB(int8_t sib);
        //decode the register
        const std::string& decodeRegisterReg(uint8_t reg, InstructionInfo info);
        const std::string& decodeRegisterRM(uint8_t reg, InstructionInfo info);


        
        

        
    private:





        

        //decode the instruction
        Instruction* decodeAdd(InstructionInfo instruction);
        Instruction* decodeSub(InstructionInfo instruction);
        Instruction* decodeMul(InstructionInfo instruction);
        Instruction* decodeMov(InstructionInfo instruction, int position, CU* controlUnit);

       


    
        //etc
};

#endif // DECODER_HPP
