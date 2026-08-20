#include "alu.hpp"
#include "cpu.hpp"
#include "registerFile.hpp"

ALU::ALU()
{
    //nothing to do here
}

ALU::~ALU()
{
    //nothing to do here
}


void ALU::executeOperation(temporaryValues& tempValues, TypeofInstruction type, uint8_t nbit)
{
    DEBUG_LOG(debugLog("ALU: Executing operation " + toStringTypeofInstruction(type) + " with source value " + to_string_hex(tempValues.srcValue) + " and destination value " + to_string_hex(tempValues.destValue) + "."));
    uint64_t dest = tempValues.destValue;
    uint64_t src  = tempValues.srcValue;
    uint64_t tmp  = 0;

    const uint64_t widthMask = (nbit == 64) ? ~0ULL : ((1ULL << nbit) - 1);
    const uint64_t signBit   = (1ULL << (nbit - 1));

    // Esegui le operazioni normalmente
    switch (type) {
        case TypeofInstruction::ADD:
            tmp = add(dest, src);
            tempValues.CF = ((tmp & widthMask) < (dest & widthMask));
            tempValues.AF = ((dest & 0xF) + (src & 0xF)) > 0xF;
            tempValues.OF = ((((dest & signBit) == (src & signBit)) &&
                             (((tmp & signBit) != (dest & signBit)))));
            break;

        case TypeofInstruction::SUB:
            tmp = sub(dest, src);
            tempValues.CF = ((dest & widthMask) < (src & widthMask));
            tempValues.AF = ((dest & 0xF) - (src & 0xF)) & 0x10;
            tempValues.OF = ((((dest & signBit) != (src & signBit)) &&
                             (((tmp & signBit) != (dest & signBit)))));
            break;

        default:
            throw std::runtime_error("ALU: operazione non supportata");
    }

    const uint64_t truncatedResult = tmp & widthMask;

    tempValues.ZF = (truncatedResult == 0);
    tempValues.SF = (truncatedResult & signBit) != 0;
    tempValues.PF = !__builtin_parity(truncatedResult & 0xFF);
    tempValues.resultValue = truncatedResult;
}


uint64_t ALU::sub(uint64_t dest, uint64_t src)
{
    return dest - src;
}


uint64_t ALU::add(uint64_t dest, uint64_t src)
{
    return dest + src;
}