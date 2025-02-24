//for decodification of the instructions

// decoder.hpp
#ifndef DECODER_HPP
#define DECODER_HPP

#include <string>
#include "instruction.hpp"
#include <vector>

//struct for simplifign decoding after getting some information out of fetchinstruction
struct InstructionInfo {
    size_t totalLength;  // Lunghezza totale dell'istruzione
    size_t opcodeLength; // Lunghezza dell'opcode (1, 2 o 3 byte)
    size_t prefixCount;  // Numero di prefissi
    bool rex;            // Flag REX
    uint32_t opcode;     // Opcode completo (1, 2 o 3 byte)
    size_t additionalBytes; // Byte aggiuntivi (ModR/M, SIB, displacement, immediate)
    size_t numOperands; // Numero di operandi
    size_t operandLength; // Lunghezza degli operandi
    std::vector<uint8_t> istruction;
    const char* description; // Descrizione dell'istruzione (opzionale)

};



class Decoder {
    
    public:
        Decoder();
        ~Decoder();
        
        //decode the instruction
        InstructionInfo LenghtOfInstruction(int32_t opcode, uint8_t prefix[4], int numPrefixes, bool rex, int16_t rexprefix);
        Instruction* decodeInstruction(std::string instruction);
        
    private:
        //decode the instruction
        Instruction* decodeAdd(std::string instruction);
        Instruction* decodeSub(std::string instruction);
        Instruction* decodeMul(std::string instruction);
        //etc
};

#endif // DECODER_HPP
