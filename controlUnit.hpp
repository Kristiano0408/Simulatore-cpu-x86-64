#ifndef CONTROL_UNIT_HPP
#define CONTROL_UNIT_HPP

#include "decoder.hpp"
#include "addressingMode.hpp"
#include "registerFile.hpp"
#include "alu.hpp"


//struct for simplifign decoding after getting some information out of fetchinstruction
struct InstructionInfo {
    size_t totalLength;  // Lunghezza totale dell'istruzione
    size_t opcodeLength; // Lunghezza dell'opcode (1, 2 o 3 byte)
    size_t prefixCount;  // Numero di prefissi
    uint32_t opcode;     // Opcode completo (1, 2 o 3 byte)
};


class CU
{
    public:
        CU(Memory* memory);
        ~CU();

        int64_t fetchInstruction();
        Instruction* decodeInstruction(uint64_t instruction);
        void executeInstruction(Instruction* instruction);

        //getters for the registers
        RegisterFile& getRegisters();
        ALU& getALU();


        

    private:
        Decoder* decoder;
        AddressingMode* addressingMode;
        RegisterFile registers; 
        ALU alu;
        Memory* memory;
        

};









#endif // CONTROL_UNIT_HPP