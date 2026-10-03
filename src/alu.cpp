#include "alu.hpp"


void ALU::executeOperation(temporaryValues& tempValues, TypeofInstruction type, uint8_t nbit)
{
    DEBUG_LOG(debugLog("ALU: Executing operation " + toStringTypeofInstruction(type) + " with source value " + to_string_hex(tempValues.srcValue) + " and destination value " + to_string_hex(tempValues.destValue) + "."));
    uint64_t dest = tempValues.destValue;
    uint64_t src  = tempValues.srcValue;
    uint64_t tmp  = 0;

    const uint64_t widthMask = (nbit == 64) ? ~0ULL : ((1ULL << nbit) - 1);
    const uint64_t signBit   = (1ULL << (nbit - 1));
    const uint64_t c         = tempValues.carryIn ? 1ULL : 0ULL;

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
        case TypeofInstruction::CMP:
            // CMP calcola dest - src esattamente come SUB:
            // il risultato viene troncato e usato solo per i flag,
            // la scrittura sull'operando di destinazione e' soppressa nella pipeline
            tmp = sub(dest, src);
            tempValues.CF = ((dest & widthMask) < (src & widthMask));
            tempValues.AF = ((((dest & 0xF) - (src & 0xF)) & 0x10) != 0U);
            tempValues.OF = ((((dest & signBit) != (src & signBit)) &&
                             (((tmp & signBit) != (dest & signBit)))));
            break;

        case TypeofInstruction::ADC: {
            tmp = adc(dest, src, c);
            if (nbit == 64) {
                const uint64_t sum1 = dest + src;
                const bool c1 = (sum1 < dest);
                const uint64_t sum2 = sum1 + c;
                const bool c2 = (sum2 < sum1);
                tempValues.CF = c1 || c2;
            } else {
                tempValues.CF = (((dest & widthMask) + (src & widthMask) + c) > widthMask);
            }
            tempValues.AF = (((dest & 0xFULL) + (src & 0xFULL) + c) > 0xFULL);
            tempValues.OF = (((dest & signBit) == (src & signBit)) &&
                             (((tmp & signBit) != (dest & signBit))));
            break;
        }

        case TypeofInstruction::SBB: {
            tmp = sbb(dest, src, c);
            if (nbit == 64) {
                const bool b1 = (dest < src);
                const uint64_t diff1 = dest - src;
                const bool b2 = (diff1 < c);
                tempValues.CF = b1 || b2;
            } else {
                tempValues.CF = ((dest & widthMask) < ((src & widthMask) + c));
            }
            tempValues.AF = ((((dest & 0xFULL) - (src & 0xFULL) - c) & 0x10ULL) != 0U);
            tempValues.OF = (((dest & signBit) != (src & signBit)) &&
                             (((tmp & signBit) != (dest & signBit))));
            break;
        }

        case TypeofInstruction::INC:
            tmp = add(dest, 1);
            tempValues.AF = ((dest & 0xF) + 1) > 0xF;
            // CF unchanged for INC
            // OF: overflow if positive → negative (max positive to most negative of the width)
            tempValues.OF = (((dest & signBit) == 0) && ((tmp & signBit) != 0));
            break;

        case TypeofInstruction::DEC:
            tmp = sub(dest, 1);
            tempValues.AF = ((((dest & 0xF) - 1) & 0x10) != 0U);
            // CF unchanged for DEC
            // OF: overflow if negative → positive (most negative to max positive of the width)
            tempValues.OF = (((dest & signBit) != 0) && ((tmp & signBit) == 0));
            break;

        case TypeofInstruction::NEG: {
            // NEG dest = 0 - dest = (~dest) + 1
            tmp = (~dest) + 1ULL;

            // CF: 1 for every operand different from 0 (0 - dest needs a borrow)
            tempValues.CF = ((dest & widthMask) != 0ULL);

            // AF: borrow from bit 4 in 0 - (dest & 0xF)
            tempValues.AF = ((dest & 0xFULL) != 0ULL);

            // OF: 1 only if dest is the most negative representable value (-2^(nbit-1))
            tempValues.OF = ((dest & widthMask) == signBit);
            break;
        }

        default:
            throw std::runtime_error("ALU: operazione non supportata");
    }

    const uint64_t truncatedResult = tmp & widthMask;

    tempValues.ZF = (truncatedResult == 0);
    tempValues.SF = (truncatedResult & signBit) != 0;
    tempValues.PF = (__builtin_parity(truncatedResult & 0xFF) == 0);
    tempValues.resultValue = truncatedResult;
}


uint64_t ALU::sub(uint64_t dest, uint64_t src)
{
    return dest - src;
}

uint64_t ALU::adc(uint64_t dest, uint64_t src, uint64_t c)
{
    return dest + src + c;
}

uint64_t ALU::sbb(uint64_t dest, uint64_t src, uint64_t c)
{
    return dest - src - c;
}


uint64_t ALU::add(uint64_t dest, uint64_t src)
{
    return dest + src;
}