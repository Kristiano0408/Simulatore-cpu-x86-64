#ifndef CONTROL_UNIT_HPP
#define CONTROL_UNIT_HPP

#include "decoder.hpp"
#include "addressingMode.hpp"


class CU
{
    public:
        CU();
        ~CU();

        int64_t fetchInstruction();
        Instruction* decodeInstruction(uint64_t instruction);
        void executeInstruction(Instruction* instruction);


        

    private:
        Decoder* decoder;
        AddressingMode* adressingMode;
        

};









#endif // CONTROL_UNIT_HPP