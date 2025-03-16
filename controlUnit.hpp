#ifndef CONTROL_UNIT_HPP
#define CONTROL_UNIT_HPP

#include "decoder.hpp"
#include "registerFile.hpp"
#include "alu.hpp"
//#include "addressingMode.hpp"
#include "memory.hpp"


//class AddressingMode;

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
        AddressingMode& getAddressingMode();

        //helpers function for making the code more readable
        void fixTotalLengthPrefix(InstructionInfo* info);
        void searchingSIB_Displacement(std::vector<uint8_t>& bytes, InstructionInfo* info, int* byteCounter, r_m* rm, uint64_t index);
        void fetchOpcode(std::vector<uint8_t>& bytes, int* byteCounter, uint64_t index, uint32_t* opcode);
        

    private:
        Decoder* decoder;
        AddressingMode addressingMode;
        RegisterFile registers; 
        ALU alu;
        Memory* memory;

        
        

};









#endif // CONTROL_UNIT_HPP