#ifndef CONTROL_UNIT_HPP
#define CONTROL_UNIT_HPP

#include "decoder.hpp"
#include "registerFile.hpp"
#include "alu.hpp"
#include "memory.hpp"


class CU
{
    public:
        CU(Memory* memory);
        ~CU();

        InstructionInfo fetchInstruction();
        Instruction* decodeInstruction(InstructionInfo instruction);
        void OperandFetch(Instruction* instruction);
        void executeInstruction(Instruction* instruction);

        //getters for the registers
        RegisterFile& getRegisters();
        ALU& getALU();

        
        

    private:
        Decoder* decoder;
        RegisterFile registers; 
        ALU alu;
        Memory* memory;

        //helpers function for making the code more readable
        void searchingSIB_Displacement(std::vector<uint8_t>& bytes, InstructionInfo& info, int& byteCounter, r_m& rm, uint64_t index);
        void fetchOpcode(std::vector<uint8_t>& bytes, int& byteCounter, uint64_t index, uint32_t& opcode);
        uint8_t fetchByte(uint64_t index, int& bytecounter);
        void fetchPrefix(uint64_t index, int& byteCounter, uint8_t (&prefix)[4], int& numbersOfPrefix, std::vector<uint8_t>& bytes);
        void fetchREX(uint8_t byte, bool& rex, uint8_t& rexprefix, int& byteCounter, std::vector<uint8_t>& bytes);
        

};









#endif // CONTROL_UNIT_HPP