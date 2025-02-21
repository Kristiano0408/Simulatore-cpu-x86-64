#ifndef CONTROL_UNIT_HPP
#define CONTROL_UNIT_HPP

#include "decoder.hpp"
#include "adressingMode.hpp"


class CU
{
    public:
        CU();
        ~CU();

        Instruction* fetchInstruction();
        void decodeInstruction(Instruction* instruction);
        void executeInstruction(Instruction* instruction);

        

    private:
        Decoder* decoder;
        AddressingMode* adressingMode;

};









#endif // CONTROL_UNIT_HPP