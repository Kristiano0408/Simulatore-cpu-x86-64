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
        void executeInstruction(Instruction* instruction);

        //getters for the registers
        RegisterFile& getRegisters();
        ALU& getALU();


        

    private:
        Decoder* decoder;
        //AddressingMode* addressingMode;
        RegisterFile registers; 
        ALU alu;
        Memory* memory;
        

};









#endif // CONTROL_UNIT_HPP