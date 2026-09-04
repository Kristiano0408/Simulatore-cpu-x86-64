#include <cstdint>
#include <vector>
#include "helpers.hpp"
#include <string>
#include <iostream>
#include "registerFile.hpp"

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


Register decodeRegisterReg(uint8_t reg, uint8_t rexprefix)
{
    if (rexprefix & 0x04)
    {
        reg += 8;
    }

    
    switch (reg)
    {
        case 0:
            return Register::RAX;
        case 1:
            return Register::RCX;
        case 2:
            return Register::RDX;
        case 3:
            return Register::RBX;
        case 4:
            return Register::RSP;
        case 5:
            return Register::RBP;
        case 6:
            return Register::RSI;
        case 7:
            return Register::RDI;
        case 8:
            return Register::R8;
        case 9:
            return Register::R9;
        case 10:
            return Register::R10;
        case 11:
            return Register::R11;
        case 12:
            return Register::R12;
        case 13:
            return Register::R13;
        case 14:
            return Register::R14;
        case 15:
            return Register::R15;
        default:
            return Register::DummyRegister; // Unknown register
    }
}

Register decodeRegisterRM(uint8_t reg, uint8_t rexprefix, bool hasSIB)
{
    if (rexprefix & 0x01 && !hasSIB)
    {
        reg += 8;
    }
    
    switch (reg)
    {
        case 0:
            return Register::RAX;
        case 1:
            return Register::RCX;
        case 2:
            return Register::RDX;
        case 3:
            return Register::RBX;
        case 4:
            return Register::RSP;
        case 5:
            return Register::RBP;
        case 6:
            return Register::RSI;
        case 7:
            return Register::RDI;
        case 8:
            return Register::R8;
        case 9:
            return Register::R9;
        case 10:
            return Register::R10;
        case 11:
            return Register::R11;
        case 12:
            return Register::R12;
        case 13:
            return Register::R13;
        case 14:
            return Register::R14;
        case 15:
            return Register::R15;
        default:
            return Register::DummyRegister; // Unknown register
    }
}

Register decodeRegisterSIB_base(uint8_t reg, uint8_t rexprefix, bool hasSIB)
{
    if (rexprefix & 0x01 && hasSIB)
    {
        reg += 8;
    }

    switch (reg)
    {
        case 0:
            return Register::RAX;
        case 1:
            return Register::RCX;
        case 2:
            return Register::RDX;
        case 3:
            return Register::RBX;
        case 4:
            return Register::RSP;
        case 5:
            return Register::RBP;
        case 6:
            return Register::RSI;
        case 7:
            return Register::RDI;
        case 8:
            return Register::R8;
        case 9:
            return Register::R9;
        case 10:
            return Register::R10;
        case 11:
            return Register::R11;
        case 12:
            return Register::R12;
        case 13:
            return Register::R13;
        case 14:
            return Register::R14;
        case 15:
            return Register::R15;
        default:
            return Register::DummyRegister; // Unknown register
    }

}

Register decodeRegisterSIB_index(uint8_t reg, uint8_t rexprefix, bool hasSIB)
{
    if (rexprefix & 0x02 && hasSIB)
    {
        reg += 8;
    }

    switch (reg)
    {
        case 0:
            return Register::RAX;
        case 1:
            return Register::RCX;
        case 2:
            return Register::RDX;
        case 3:
            return Register::RBX;
        case 4:
            return Register::RSP;
        case 5:
            return Register::RBP;
        case 6:
            return Register::RSI;
        case 7:
            return Register::RDI;
        case 8:
            return Register::R8;
        case 9:
            return Register::R9;
        case 10:
            return Register::R10;
        case 11:
            return Register::R11;
        case 12:
            return Register::R12;
        case 13:
            return Register::R13;
        case 14:
            return Register::R14;
        case 15:
            return Register::R15;
        default:
            return Register::DummyRegister; // Unknown register
    }

}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, const ComponentType& type)
{
    switch (type) {
    case ComponentType::CACHE: os << "CACHE"; break;
    case ComponentType::CACHE_L1: os << "CACHE_L1"; break;
    case ComponentType::CACHE_L2: os << "CACHE_L2"; break;
    case ComponentType::CACHE_L3: os << "CACHE_L3"; break;
    case ComponentType::RAM: os << "RAM"; break;
    case ComponentType::OPERAND: os << "OPERAND"; break;
    case ComponentType::ALU: os << "ALU"; break;
    case ComponentType::FPU: os << "FPU"; break;
    default: os << "UNKNOWN"; break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const EventType& type)
{
    switch (type) {
    case EventType::NONE: os << "NONE"; break;
    case EventType::CACHE_HIT: os << "CACHE_HIT"; break;
    case EventType::CACHE_MISS: os << "CACHE_MISS"; break;
    case EventType::RAM_ACCESS: os << "RAM_ACCESS"; break;
    case EventType::ERROR: os << "ERROR"; break;
    case EventType::CACHE_READ_ERROR: os << "CACHE_READ_ERROR"; break;
    case EventType::CACHE_WRITE_ERROR: os << "CACHE_WRITE_ERROR"; break;
    case EventType::RAM_READ_ERROR: os << "RAM_READ_ERROR"; break;
    case EventType::RAM_WRITE_ERROR: os << "RAM_WRITE_ERROR"; break;
    default: os << "UNKNOWN"; break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const ErrorType& type)
{
    switch (type) {
    case ErrorType::NONE: os << "NONE"; break;
    case ErrorType::INVALID_ADDRESS: os << "INVALID_ADDRESS"; break;
    case ErrorType::INVALID_SIZE: os << "INVALID_SIZE"; break;
    case ErrorType::OUT_OF_BOUNDS: os << "OUT_OF_BOUNDS"; break;
    case ErrorType::WRITE_FAIL: os << "WRITE_FAIL"; break;
    case ErrorType::READ_FAIL: os << "READ_FAIL"; break;
    default: os << "UNKNOWN"; break;
    }
    return os;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

std::string toStringTypeofInstruction(TypeofInstruction type)
{
    switch (type) {
        case TypeofInstruction::MOV: return "MOV";
        case TypeofInstruction::ADD: return "ADD";
        case TypeofInstruction::SUB: return "SUB";
        case TypeofInstruction::LEA: return "LEA";
        default: return "UNKNOWN";
        }
}

ComponentType getComponentTypeFromCacheLevelType(CacheLevelType type)
{
    switch (type) {
        case CacheLevelType::L1I:
            return ComponentType::CACHE_L1;
        case CacheLevelType::L1D:
            return ComponentType::CACHE_L1;
        case CacheLevelType::L2:
            return ComponentType::CACHE_L2;
        case CacheLevelType::L3:
            return ComponentType::CACHE_L3;
        default:
            return ComponentType::CACHE; // Default to CACHE for NONE or unknown types
    }
}


//////////////////////////////////////////////////////////////////////////////////////