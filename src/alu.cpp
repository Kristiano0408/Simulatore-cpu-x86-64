#include "alu.hpp"
#include "registerFile.hpp"
/*
 * ============================================================================
 * ARITHMETIC FLAGS CALCULATION — x86-64 ISA
 * ============================================================================
 *
 * RIFERIMENTI:
 * - Intel 64 and IA-32 Architectures Software Developer's Manual (SDM),
 *   Volume 1, Section 3.4.3.1 — Status Flags.
 * - Intel SDM, Volume 2 — Specification of each instruction, sections
 *   "Operation" and "Flags Affected".
 * - Intel SDM Specification Style Guide, "Example instruction specification":
 *   https://intel.github.io/SDM/style_guide.html
 *
 * PRINCIPIO GENERALE
 * ------------------
 * L'ISA definisce il significato architetturale dei flag e stabilisce quali
 * istruzioni li aggiornano, li preservano o li lasciano indefiniti.
 *
 * Le formule utilizzate dall'ALU sono una traduzione matematica di queste
 * definizioni. Non tutte le istruzioni x86-64 condividono le stesse regole:
 * le formule aritmetiche sottostanti sono applicabili alle operazioni di
 * addizione e sottrazione, mentre le altre famiglie richiedono regole
 * specifiche dell'istruzione.
 *
 * CONVENZIONI
 * -----------
 * n    = larghezza dell'operando in bit (8, 16, 32 o 64).
 * M    = 2^n - 1, maschera della larghezza dell'operando.
 * S    = 2^(n-1), maschera del bit di segno.
 * a    = operando di destinazione, limitato a n bit.
 * b    = operando sorgente, limitato a n bit.
 * r    = risultato effettivo, troncato a n bit.
 * c    = carry-in per ADC oppure borrow-in per SBB (0 oppure 1).
 *
 * Gli operandi e il risultato devono essere normalizzati alla larghezza
 * dell'operazione prima di calcolare i flag.
 *
 * FORMULE DEI FLAG
 * ----------------
 *
 * ZF — Zero Flag
 * Impostato se il risultato troncato è zero.
 *
 *     ZF = (r == 0)
 *
 * SF — Sign Flag
 * Copia il bit più significativo del risultato, che rappresenta il segno
 * nell'interpretazione signed in complemento a due.
 *
 *     SF = ((r & S) != 0)
 *
 * PF — Parity Flag
 * Impostato se gli 8 bit meno significativi del risultato contengono un
 * numero pari di bit a 1. La parità riguarda sempre il byte inferiore,
 * indipendentemente dalla larghezza dell'operando.
 *
 *     PF = (parity(r & 0xFF) == even)
 *
 * AF — Auxiliary Carry Flag
 * Indica un carry o borrow attraverso il confine tra il bit 3 e il bit 4.
 * Per le operazioni aritmetiche di addizione e sottrazione può essere
 * calcolato mediante la seguente relazione bitwise:
 *
 *     AF = (((a ^ b ^ r) & 0x10) != 0)
 *
 * La costante 0x10 seleziona il bit 4. La formula si applica anche ad ADC
 * e SBB, purché r includa l'effetto del carry-in o borrow-in.
 *
 * CF — Carry Flag
 * Indica un carry oltre la larghezza dell'operando nelle addizioni oppure
 * un borrow nelle sottrazioni. Rileva l'overflow nell'interpretazione
 * unsigned e non va confuso con l'overflow signed segnalato da OF.
 *
 * Addizione (ADD):
 *
 *     CF = (a + b > M)
 *
 * Addizione con carry (ADC):
 *
 *     CF = (a + b + c > M)
 *
 * Sottrazione (SUB, CMP):
 *
 *     CF = (a < b)
 *
 * Sottrazione con borrow (SBB):
 *
 *     CF = (a < b + c)
 *
 * Nell'implementazione C++, queste condizioni possono essere riscritte
 * usando confronti e sottrazioni che evitano di calcolare una somma
 * intermedia oltre la capacità di uint64_t.
 *
 * OF — Overflow Flag
 * Indica che il risultato matematico non è rappresentabile nella larghezza
 * dell'operando interpretato come intero signed in complemento a due.
 *
 * Addizione (ADD, ADC):
 *
 *     OF = ((~(a ^ b) & (a ^ r) & S) != 0)
 *
 * Sottrazione (SUB, SBB, CMP):
 *
 *     OF = (((a ^ b) & (a ^ r) & S) != 0)
 *
 * Le espressioni sono formule booleane equivalenti alla verifica
 * dell'overflow signed, espresse in termini di segni degli operandi e
 * del risultato effettivo.
 *
 * EFFETTI SPECIFICI DELLE ISTRUZIONI
 * ----------------------------------
 * Le formule matematiche non determinano, da sole, quali flag debbano
 * essere scritti: questa decisione dipende dalla semantica dell'istruzione.
 *
 * - INC e DEC aggiornano AF, OF, SF, ZF e PF, ma preservano CF.
 * - NEG equivale aritmeticamente a 0 - operando; CF vale 1 se l'operando
 *   è diverso da zero.
 * - CMP calcola i flag come una sottrazione, senza scrivere il risultato
 *   nell'operando di destinazione.
 * - Le istruzioni logiche, gli shift, le rotazioni, le moltiplicazioni
 *   e le divisioni hanno regole specifiche per i flag.
 *
 * Pertanto, l'ALU separa il calcolo matematico dei flag dalla gestione
 * degli effetti architetturali specifici dell'istruzione.
 * ============================================================================
 */


void ALU::calculateCF_add(temporaryValues& tempValues, uint64_t dest, uint64_t src, uint64_t mask,  bool carryIn) {
    tempValues.CF = (dest > (mask - src)) || (carryIn && dest == (mask - src));
    tempValues.flagsMask |= (1ULL << static_cast<uint8_t>(Flagbit::CF)); // Mark CF as affected
}

void ALU::calculateCF_sub(temporaryValues& tempValues, uint64_t dest, uint64_t src, bool carryIn) {
    tempValues.CF = (dest < src) || (carryIn && dest == src);
    tempValues.flagsMask |= (1ULL << static_cast<uint8_t>(Flagbit::CF)); // Mark CF as affected
}

void ALU::calculateOF_add(temporaryValues& tempValues, uint64_t dest, uint64_t src, uint64_t result, uint64_t signBit) {
    tempValues.OF = ((~(dest^ src)) & (dest ^ result) & signBit) != 0;
    tempValues.flagsMask |= (1ULL << static_cast<uint8_t>(Flagbit::OF)); // Mark OF as affected
}

void ALU::calculateOF_sub(temporaryValues& tempValues, uint64_t dest, uint64_t src, uint64_t result, uint64_t signBit) {
    tempValues.OF = ((dest ^ src) & (dest ^ result) & signBit) != 0;
    tempValues.flagsMask |= (1ULL << static_cast<uint8_t>(Flagbit::OF)); // Mark OF as affected
}


void ALU::calculateAF(temporaryValues& tempValues, uint64_t destMasked, uint64_t srcMasked, uint64_t resultMasked) {
    tempValues.AF = ((destMasked ^ srcMasked ^ resultMasked) & 0x10) != 0;
    tempValues.flagsMask |= (1ULL << static_cast<uint8_t>(Flagbit::AF)); // Mark AF as affected
}

void ALU::calculateZF(temporaryValues& tempValues, uint64_t resultMasked) {
    tempValues.ZF = (resultMasked == 0);
    tempValues.flagsMask |= (1ULL << static_cast<uint8_t>(Flagbit::ZF)); // Mark ZF as affected
}

void ALU::calculateSF(temporaryValues& tempValues, uint64_t resultMasked, uint64_t signBit) {
    tempValues.SF = (resultMasked & signBit) != 0;
    tempValues.flagsMask |= (1ULL << static_cast<uint8_t>(Flagbit::SF)); // Mark SF as affected
}

void ALU::calculatePF(temporaryValues& tempValues, uint64_t resultMasked) {
    // Parity flag is set if the number of set bits in the result is even
    tempValues.PF = static_cast<bool>(__builtin_parity(resultMasked & 0xFF) == 0);
    tempValues.flagsMask |= (1ULL << static_cast<uint8_t>(Flagbit::PF)); // Mark PF as affected
}







void ALU::executeOperation(temporaryValues& tempValues, TypeofInstruction type, uint8_t srcNbit, uint8_t destNbit, uint8_t auxNbit, uint8_t  immediateNbit, uint8_t nOperands)
{
   
    DEBUG_LOG(debugLog("ALU: Executing operation " + toStringTypeofInstruction(type) + " with source value " + to_string_hex(tempValues.srcValue) + " and destination value " + to_string_hex(tempValues.destValue) + "."));
    uint64_t dest = tempValues.destValue;
    uint64_t destHigh = tempValues.destHighValue;
    uint64_t src  = tempValues.srcValue;
    uint64_t tmp  = 0;

    auto signExtend = [](uint64_t value, uint8_t bits) -> int64_t {
        const uint64_t mask =
            (bits == 64) ? ~0ULL : ((1ULL << bits) - 1);

        value &= mask;

        const uint64_t signBit = 1ULL << (bits - 1);

        if ((value & signBit) == 0)
            return static_cast<int64_t>(value);

        // Sottraendo 2^bits si ottiene il valore signed.
        // __int128 evita problemi con lo shift a 64 bit.
        const __int128 signedValue = static_cast<__int128>(value) -  (static_cast<__int128>(1) << bits);

        return static_cast<int64_t>(signedValue);
    };

    auto signExtendSrcImmediateRex = [](uint64_t value, bool needsSignExtension) -> uint64_t {
        if (needsSignExtension) {
            return static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(value)));
        } 
        else {
            return value;
        }
    };

    auto signExtendSrcImmediate = [](uint64_t value, uint8_t finalBits, uint8_t originalBits) -> uint64_t {
        
        uint8_t signBit = value >> (originalBits - 1) & 1;

        uint64_t mask;

        mask = signBit ? (~(0ULL) << originalBits) : 0ULL;

        uint64_t finalMask = (finalBits == 64) ? ~(0ULL) : ((1ULL << finalBits) - 1);

        return (value | mask) & finalMask;


    };

    const uint64_t srcWidthMask = (srcNbit == 64) ? ~0ULL : ((1ULL << srcNbit) - 1);
    const uint64_t destWidthMask = (destNbit == 64) ? ~0ULL : ((1ULL << destNbit) - 1);
    const uint64_t auxWidthMask = (auxNbit == 64) ? ~0ULL : ((1ULL << auxNbit) - 1);
    const uint64_t immediateWidthMask = (immediateNbit == 64) ? ~0ULL : ((1ULL << immediateNbit) - 1);
    const uint64_t signBitSrc   = (1ULL << (srcNbit - 1));
    const uint64_t signBitDest  = (1ULL << (destNbit - 1));
    const uint64_t c         = tempValues.carryIn ? 1ULL : 0ULL;
    uint64_t destMasked = dest & destWidthMask;
    uint64_t destHighMasked = destHigh & destWidthMask;
    uint64_t srcMasked  = src & srcWidthMask;

    // Esegui le operazioni normalmente
    switch (type) {
        case TypeofInstruction::ADD:
            srcMasked = signExtendSrcImmediateRex(srcMasked, tempValues.srcImmediateNeedsSignExtensionRex);
            tmp = add(destMasked, srcMasked);
            calculateCF_add(tempValues, dest, src, srcWidthMask, false);
            calculateAF(tempValues, destMasked, srcMasked, tmp & destWidthMask);
            calculateOF_add(tempValues, dest, src, tmp, signBitDest);
            break;

        case TypeofInstruction::SUB:
        case TypeofInstruction::CMP:
            // CMP calcola dest - src esattamente come SUB:
            // il risultato viene troncato e usato solo per i flag,
            // la scrittura sull'operando di destinazione e' soppressa nella pipeline
            srcMasked = signExtendSrcImmediateRex(srcMasked, tempValues.srcImmediateNeedsSignExtensionRex);
            tmp = sub(destMasked, srcMasked);
            calculateCF_sub(tempValues, dest, src, false);
            calculateAF(tempValues, destMasked, srcMasked, tmp & destWidthMask);
            calculateOF_sub(tempValues, dest, src, tmp, signBitDest);
            break;

        case TypeofInstruction::ADC: {
            srcMasked = signExtendSrcImmediateRex(srcMasked, tempValues.srcImmediateNeedsSignExtensionRex);
            tmp = adc(destMasked, srcMasked, c);
            calculateCF_add(tempValues, dest, src, srcWidthMask, c);
            calculateAF(tempValues, destMasked, srcMasked, tmp & destWidthMask);
            calculateOF_add(tempValues, dest, src, tmp, signBitDest);
            break;
        }

        case TypeofInstruction::SBB: {
            srcMasked = signExtendSrcImmediateRex(srcMasked, tempValues.srcImmediateNeedsSignExtensionRex);
            tmp = sbb(destMasked, srcMasked, c);
            calculateCF_sub(tempValues, dest, src, c);
            calculateAF(tempValues, destMasked, srcMasked, tmp & destWidthMask);
            calculateOF_sub(tempValues, dest, src, tmp, signBitDest);
            break;
        }

        case TypeofInstruction::INC:
            tmp = add(destMasked, 1);
            calculateAF(tempValues, destMasked, 1, tmp & destWidthMask);
            // CF unchanged for INC
            // OF: overflow if positive → negative (max positive to most negative of the width)
            calculateOF_add(tempValues, dest, 1, tmp, signBitDest);
            break;

        case TypeofInstruction::DEC:
            tmp = sub(destMasked, 1);
            calculateAF(tempValues, destMasked, 1, tmp & destWidthMask);
            // CF unchanged for DEC
            // OF: overflow if negative → positive (most negative to max positive of the width)
            calculateOF_sub(tempValues, dest, 1, tmp, signBitDest);
            break;

        case TypeofInstruction::NEG: {
            // NEG dest = 0 - dest = (~dest) + 1
            tmp = (~destMasked) + 1ULL;

            // CF: 1 for every operand different from 0 (0 - dest needs a borrow)
            calculateCF_sub(tempValues, 0, dest, false);

            // AF: borrow from bit 4 in 0 - (dest & 0xF)
            calculateAF(tempValues, 0, destMasked, tmp & destWidthMask);
            calculateOF_sub(tempValues, 0, dest, tmp, signBitDest);
            break;
        }

        case TypeofInstruction::MUL: {
            if (destNbit != 64) {
                tmp = mul(destMasked, srcMasked);
                tempValues.resultValue = tmp & destWidthMask;
                tempValues.resultHighValue  = (tmp >> (destNbit)) & destWidthMask;
                tempValues.CF = tempValues.OF = (tempValues.resultHighValue != 0);
                tempValues.flagsMask |= (1ULL << static_cast<uint8_t>(Flagbit::CF)); // Mark CF as affected
                tempValues.flagsMask |= (1ULL << static_cast<uint8_t>(Flagbit::OF)); // Mark OF as affected
            } else {
                unsigned __int128 d128 = dest;
                unsigned __int128 s128 = src;
                unsigned __int128 prod = d128 * s128;
                tempValues.resultValue = static_cast<uint64_t>(prod);
                tempValues.resultHighValue  = static_cast<uint64_t>(prod >> 64);
                tempValues.CF = tempValues.OF = (tempValues.resultHighValue != 0);
                tempValues.flagsMask |= (1ULL << static_cast<uint8_t>(Flagbit::CF)); // Mark CF as affected
                tempValues.flagsMask |= (1ULL << static_cast<uint8_t>(Flagbit::OF)); // Mark OF as affected
            }
            return;
        }

        case TypeofInstruction::IMUL: {
            int64_t srcSigned = signExtend(srcMasked, srcNbit);
            int64_t destSigned = signExtend(destMasked, destNbit);
            // For the 3-op form (RMI) the multiplier is the sign-extended immediate stored
            // in auxValue by ExecuteEngine: its width is immediateNbit (imm8/imm16/imm32),
            // NOT auxNbit (which is 0 for RMI and would truncate the immediate to 0).
            const uint8_t auxWidth = (nOperands == 3) ? immediateNbit : auxNbit;
            int64_t auxSigned = signExtend(tempValues.auxValue, auxWidth);
            if(nOperands == 3) {
                if(destNbit != 64) {
                    int64_t tmpSigned = srcSigned * auxSigned;
                    tempValues.resultValue = tmpSigned & destWidthMask;
                    tempValues.resultHighValue  = (tmpSigned >> (destNbit)) & destWidthMask;
                    int64_t sext = signExtend(tempValues.resultValue, destNbit);
                    tempValues.CF = tempValues.OF = (tmpSigned != sext);
                    tempValues.flagsMask |= (1ULL << static_cast<uint8_t>(Flagbit::CF)); // Mark CF as affected
                } else {
                
                    __int128 s128 = srcSigned;
                    __int128 a128 = auxSigned;
                    __int128 prod =s128 * static_cast<__int128>(auxSigned);
                    tempValues.resultValue = static_cast<uint64_t>(prod);
                    tempValues.resultHighValue  = static_cast<uint64_t>(prod >> 64);
                    __int128 sext = signExtend(tempValues.resultValue, destNbit);
                    tempValues.CF = tempValues.OF = (prod != sext);
                    tempValues.flagsMask |= (1ULL << static_cast<uint8_t>(Flagbit::CF)); // Mark CF as affected 
                }

            }
            else {
                 if(destNbit != 64) {
                    int64_t tmpSigned = srcSigned * destSigned;
                    tempValues.resultValue = tmpSigned & destWidthMask;
                    tempValues.resultHighValue  = (tmpSigned >> (destNbit)) & destWidthMask;
                    int64_t sext =signExtend(tempValues.resultValue, destNbit);
                    tempValues.CF = tempValues.OF = (tmpSigned != sext);
                    tempValues.flagsMask |= (1ULL << static_cast<uint8_t>(Flagbit::CF)); // Mark CF as affected
                    tempValues.flagsMask |= (1ULL << static_cast<uint8_t>(Flagbit::OF)); // Mark OF as affected
                } else {
                    __int128 s128 = static_cast<__int128>(srcSigned);
                    __int128 d128 = static_cast<__int128>(destSigned);
                    __int128 prod =s128 * d128;
                    unsigned __int128 unsignedProd = static_cast<unsigned __int128>(prod);
                    tempValues.resultValue = static_cast<uint64_t>(unsignedProd);
                    tempValues.resultHighValue  = static_cast<uint64_t>(unsignedProd >> 64);
                    __int128 sext = static_cast<__int128>(signExtend(tempValues.resultValue, destNbit));
                    tempValues.CF = tempValues.OF = (prod != sext);
                    tempValues.flagsMask |= (1ULL << static_cast<uint8_t>(Flagbit::CF)); // Mark CF as affected
                    tempValues.flagsMask |= (1ULL << static_cast<uint8_t>(Flagbit::OF)); // Mark OF as affected
                }

            }
            return;
        }

        case TypeofInstruction::DIV: {
            if (src == 0) {
                //gestione errore "ALU: Division by zero (#DE)"
                return;
            }
            if(destNbit != 64) {
                uint64_t dividend = (destHighMasked << destNbit) | destMasked;
                uint64_t divisor  = src;
                uint64_t quot     = dividend / divisor;
                uint64_t rem      = dividend % divisor;
                if (quot > destWidthMask) {
                    //gestione errore "ALU: Divide overflow (#DE)"
                }
                tempValues.resultValue = quot; // RAX/EAX/AX/AL
                tempValues.resultHighValue  = rem;  // RDX/EDX/DX/DL

           
            } else {
                unsigned __int128 dividend = (static_cast<unsigned __int128>(tempValues.destHighValue) << 64) | static_cast<unsigned __int128>(dest);
                unsigned __int128 divisor  = static_cast<unsigned __int128>(src);
                unsigned __int128 quot     = dividend / divisor;
                unsigned __int128 rem      = dividend % divisor;
                if (quot > ~0ULL) {
                    //gestione errore "ALU: Divide overflow (#DE)"
                }
                tempValues.resultValue = static_cast<uint64_t>(quot); // RAX
                tempValues.resultHighValue  = static_cast<uint64_t>(rem);  // RDX

            }
            return;
        }

        case TypeofInstruction::IDIV: {

            int64_t srcSigned = signExtend(srcMasked, srcNbit);
            int64_t destSigned = signExtend(destMasked, destNbit);
            int64_t destHighSigned = signExtend(destHighMasked, auxNbit);

            if (srcSigned  == 0) {
                //gestione errore "ALU: Division by zero (#DE)"
            }

            if(destNbit != 64) {
                int64_t dividend = (destHighSigned << destNbit) | destSigned;
                int64_t divisor  = srcSigned;
                int64_t quot     = dividend / divisor;
                int64_t rem      = dividend % divisor;
                int64_t minIntN  = (1LL << (destNbit - 1));
                int64_t maxIntN  = ~(1LL << (destNbit - 1));
                if (quot < minIntN || quot > maxIntN) {
                    //gestione errore "ALU: Signed divide overflow (#DE)"
                }
                tempValues.resultValue = static_cast<uint64_t>(quot); // RAX/EAX/AX/AL
                tempValues.resultHighValue  = static_cast<uint64_t>(rem);  // RDX/EDX/DX/DL

            

        
            } else {
                __int128 dividend = (static_cast<__int128>(destHighSigned) << 64) |  static_cast<__int128>(destSigned);
                __int128 divisor  = static_cast<__int128>(static_cast<int64_t>(srcSigned));
                __int128 quot     = dividend / divisor;
                __int128 rem      = dividend % divisor;
                __int128 minInt64 = static_cast<__int128>(INT64_MIN);
                __int128 maxInt64 = static_cast<__int128>(INT64_MAX);
                if (quot < minInt64 || quot > maxInt64) {
                    //gestione errore "ALU: Signed divide overflow (#DE)"
                }
                tempValues.resultValue = static_cast<uint64_t>(quot); // RAX
                tempValues.resultHighValue  = static_cast<uint64_t>(rem);  // RDX

            }
            return;
           
        }
        
        case TypeofInstruction::AND: {

            srcMasked = signExtendSrcImmediateRex(srcMasked, tempValues.srcImmediateNeedsSignExtensionRex);
            if(tempValues.srcNeedSignExtension)
                srcMasked = signExtendSrcImmediate(srcMasked, destNbit, srcNbit);
            tmp = destMasked & srcMasked;
            tempValues.CF = false;
            tempValues.setFlagMask(static_cast<uint8_t>(Flagbit::CF), true); // Mark CF as affected
            tempValues.OF = false;
            tempValues.setFlagMask(static_cast<uint8_t>(Flagbit::OF), true); // Mark OF as affected
            break;

        }

        case TypeofInstruction::OR: {

            srcMasked = signExtendSrcImmediateRex(srcMasked, tempValues.srcImmediateNeedsSignExtensionRex);
            if(tempValues.srcNeedSignExtension)
                srcMasked = signExtendSrcImmediate(srcMasked, destNbit, srcNbit);
            tmp = destMasked | srcMasked;
            tempValues.CF = false;
            tempValues.setFlagMask(static_cast<uint8_t>(Flagbit::CF), true); // Mark CF as affected
            tempValues.OF = false;
            tempValues.setFlagMask(static_cast<uint8_t>(Flagbit::OF), true); // Mark OF as affected
            break;

        }

        case TypeofInstruction::XOR: {
            srcMasked = signExtendSrcImmediateRex(srcMasked, tempValues.srcImmediateNeedsSignExtensionRex);
            if(tempValues.srcNeedSignExtension)
                srcMasked = signExtendSrcImmediate(srcMasked, destNbit, srcNbit);
            tmp = destMasked ^ srcMasked;
            tempValues.CF = false;
            tempValues.setFlagMask(static_cast<uint8_t>(Flagbit::CF), true); // Mark CF as affected
            tempValues.OF = false;
            tempValues.setFlagMask(static_cast<uint8_t>(Flagbit::OF), true); // Mark OF as affected
            break;

        }

        case TypeofInstruction::NOT: {

            tmp = ~destMasked;
            tempValues.resultValue = (tmp & destWidthMask);
            return; 
        }

        default:
                throw std::runtime_error("ALU: operazione non supportata");
    }


        const uint64_t truncatedResult = tmp & destWidthMask;
        calculateZF(tempValues, truncatedResult);
        calculateSF(tempValues, truncatedResult, signBitDest);
        calculatePF(tempValues, truncatedResult);
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


int64_t ALU::imul(uint64_t dest, uint64_t src)
{
    return static_cast<int64_t>(dest) * static_cast<int64_t>(src);
}

uint64_t ALU::mul(uint64_t dest, uint64_t src)
{
    return static_cast<uint64_t>(dest) * static_cast<uint64_t>(src);
}
