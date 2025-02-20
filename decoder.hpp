//for decodification of the instructions

// decoder.hpp
#ifndef DECODER_HPP
#define DECODER_HPP

#include <string>
#include "instruction.hpp"

class Decoder {
    
    public:
        Decoder();
        ~Decoder();
        
        //decode the instruction
        Instruction* decodeInstruction(std::string instruction);
        
    private:
        //decode the instruction
        Instruction* decodeAdd(std::string instruction);
        Instruction* decodeSub(std::string instruction);
        Instruction* decodeMul(std::string instruction);
        //etc
};

#endif // MEMORY_HPP
