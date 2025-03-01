#include <cstdint>
#include "helpers.hpp"

bool isPrefix(uint8_t byte)
{
    if (byte == 0x66 || byte == 0x67 || byte == 0xF0 || byte == 0xF2 || byte == 0xF3 || byte == 0x2E || byte == 0x3E || byte == 0x26 || byte == 0x64 || byte == 0x65 || byte == 0x36)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool isMoveInstruction(uint32_t opcode)
{
    if(isMoveInstructionIO(opcode))
    {
        return true;
    }
    else if (isMoveInstructionR_M(opcode))
    {
        return true;
    }

    return false;
    

}

bool isMoveInstructionIO(uint32_t opcode)
{
    if(opcode >= 0xB8 && opcode <= 0xBF)
    {
        return true;
    }

    return false;
}

bool isMoveInstructionR_M(uint32_t opcode)
{
    if(isMoveR_M_reg_mem(opcode))
    {
        return true;
    }
    else if(isMoveR_M_mem_reg(opcode))
    {
        return true;
    }

    return false;
}


bool isMoveR_M_reg_mem(uint32_t opcode)
{
    if(opcode == 0x88 || opcode == 0x89)
    {
        return true;
    }

    return false;
}

bool isMoveR_M_mem_reg(uint32_t opcode)
{
    if(opcode == 0x8A || opcode == 0x8B)
    {
        return true;
    }

    return false;
}