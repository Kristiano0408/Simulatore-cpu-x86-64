#include <cstdint>
#include <vector>
#include "helpers.hpp"
#include <string>

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
    else if (isMoveInstructionIO_mem(opcode))
    {
        return true;
    }
    else if(isMoveInstructionOffset(opcode))
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
    else if(isMoveInstructionIO_8bit(opcode))
    {
        return true;
    }

    return false;
}

bool isMoveInstructionIO_8bit(uint32_t opcode)
{
    if(opcode >= 0xB0 && opcode <= 0xB7)
    {
        return true;
    }

    return false;
}


bool isMoveInstructionIO_mem(uint32_t opcode)
{
    if(opcode == 0xC6 || opcode == 0xC7)
    {
        return true;
    }

    return false;
}


bool isMoveInstructionOffset(uint32_t opcode)
{
    if(isMoveInstructionOffsetRAX_mem(opcode))
    {
        return true;
    }
    else if(isMoveInstructionOffsetMem_RAx(opcode))
    {
        return true;
    }

    return false;
}

bool isMoveInstructionOffsetRAX_mem(uint32_t opcode)
{
    if(opcode == 0xA2 || opcode == 0xA3)
    {
        return true;
    }

    return false;
}

bool isMoveInstructionOffsetMem_RAx(uint32_t opcode)
{
    if(opcode == 0xA0 || opcode == 0xA1)
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

const std::string& decodeRegisterReg(uint8_t reg, uint8_t rexprefix)
{
    if (rexprefix & 0x04)
    {
        reg += 8;
    }
    
    switch (reg)
    {
        case 0:
            return "RAX";
            break;
        case 1:
            return "RCX";
            break;
        case 2:
            return "RDX";
            break;
        case 3:
            return "RBX";
            break;
        case 4:
            return "RSP";
            break;
        case 5:
            return "RBP";
            break;
        case 6:
            return "RSI";
            break;
        case 7:
            return "RDI";
            break;
        case 8:
            return "R8";
            break;
        case 9:
            return "R9";
            break;
        case 10:
            return "R10";
            break;
        case 11:
            return "R11";
            break;
        case 12:
            return "R12";
            break;
        case 13:
            return "R13";
            break;
        case 14:
            return "R14";
            break;
        case 15:
            return "R15";
            break;
        default:
            return "Unknown register";
            break;
    }
}

const std::string& decodeRegisterRM(uint8_t reg, uint8_t rexprefix, bool hasSIB)
{
    if (rexprefix & 0x01 && !hasSIB)
    {
        reg += 8;
    }
    
    switch (reg)
    {
        case 0:
            return "RAX";
            break;
        case 1:
            return "RCX";
            break;
        case 2:
            return "RDX";
            break;
        case 3:
            return "RBX";
            break;
        case 4:
            return "RSP";
            break;
        case 5:
            return "RBP";
            break;
        case 6:
            return "RSI";
            break;
        case 7:
            return "RDI";
            break;
        case 8:
            return "R8";
            break;
        case 9:
            return "R9";
            break;
        case 10:
            return "R10";
            break;
        case 11:
            return "R11";
            break;
        case 12:
            return "R12";
            break;
        case 13:
            return "R13";
            break;
        case 14:
            return "R14";
            break;
        case 15:
            return "R15";
            break;
        default:
            return "Unknown register";
            break;
    }
}

const std::string& decodeRegisterSIB_base(uint8_t reg, uint8_t rexprefix, bool hasSIB)
{
    if (rexprefix & 0x01 && hasSIB)
    {
        reg += 8;
    }

    switch (reg)
    {
        case 0:
            return "RAX";
            break;
        case 1:
            return "RCX";
            break;
        case 2:
            return "RDX";
            break;
        case 3:
            return "RBX";
            break;
        case 4:
            return "RSP";
            break;
        case 5:
            return "RBP";
            break;
        case 6:
            return "RSI";
            break;
        case 7:
            return "RDI";
            break;
        case 8:
            return "R8";
            break;
        case 9:
            return "R9";
            break;
        case 10:
            return "R10";
            break;
        case 11:
            return "R11";
            break;
        case 12:
            return "R12";
            break;
        case 13:
            return "R13";
            break;
        case 14:
            return "R14";
            break;
        case 15:
            return "R15";
            break;
        default:
            return "Unknown register";
            break;
    }

}

const std::string& decodeRegisterSIB_index(uint8_t reg, uint8_t rexprefix, bool hasSIB)
{
    if (rexprefix & 0x02 && hasSIB)
    {
        reg += 8;
    }

    switch (reg)
    {
        case 0:
            return "RAX";
            break;
        case 1:
            return "RCX";
            break;
        case 2:
            return "RDX";
            break;
        case 3:
            return "RBX";
            break;
        case 4:
            return "RSP";
            break;
        case 5:
            return "RBP";
            break;
        case 6:
            return "RSI";
            break;
        case 7:
            return "RDI";
            break;
        case 8:
            return "R8";
            break;
        case 9:
            return "R9";
            break;
        case 10:
            return "R10";
            break;
        case 11:
            return "R11";
            break;
        case 12:
            return "R12";
            break;
        case 13:
            return "R13";
            break;
        case 14:
            return "R14";
            break;
        case 15:
            return "R15";
            break;
        default:
            return "Unknown register";
            break;
    }
}