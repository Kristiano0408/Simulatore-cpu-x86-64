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
        //AdressingMode* getAddressingMode(uint8_t mode);

    private:
        Decoder* decoder;
        AdressingMode* adressingMode;
};









#endif // CONTROL_UNIT_HPP